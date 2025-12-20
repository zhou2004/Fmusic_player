#include "WinSMTCController.h"

#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QPointer>
#include <QStandardPaths>
#include <QUrl>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <SystemMediaTransportControlsInterop.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
#endif

#ifdef Q_OS_WIN
static inline TimeSpan msToTimeSpan(qint64 ms)
{
    if (ms < 0) ms = 0;
    return TimeSpan{ ms * 10000 }; // 100ns ticks
}

// 选一个最可能的主窗口 HWND（Qt Quick：topLevelWindows）
static HWND findMainHwnd()
{
    const auto wins = QGuiApplication::topLevelWindows();
    for (QWindow* w : wins) {
        if (w && w->isVisible()) {
            return reinterpret_cast<HWND>(w->winId());
        }
    }
    if (!wins.isEmpty() && wins.first()) {
        return reinterpret_cast<HWND>(wins.first()->winId());
    }
    return nullptr;
}

// qrc:/ 或 :/ 资源落地到临时文件（SMTC 不能直接读 Qt 资源协议）
static QString materializeQtResourceToTempFile(const QString& qrcPath)
{
    QString path = qrcPath;
    if (path.startsWith("qrc:/", Qt::CaseInsensitive)) {
        path = ":" + path.mid(QString("qrc").size()); // "qrc:/a/b" -> ":/a/b"
    }
    if (!path.startsWith(":/")) return {};

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};

    const QByteArray bytes = f.readAll();
    f.close();
    if (bytes.isEmpty()) return {};

    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempDir.isEmpty()) return {};

    const QString suffix = QFileInfo(path).suffix().isEmpty() ? "jpg" : QFileInfo(path).suffix();
    const QString outPath =
        QDir(tempDir).filePath(QString("FmusicCover_%1.%2")
                               .arg(QDateTime::currentMSecsSinceEpoch())
                               .arg(suffix));

    QFile out(outPath);
    if (!out.open(QIODevice::WriteOnly)) return {};
    out.write(bytes);
    out.close();

    return outPath;
}

// 关键：把 cover 输入最终变成 Windows 原生路径（C:\... 并且用反斜杠）
// 支持：
// - file:///E:/... => E:\...
// - 直接 E:/... 或 E:\... 或相对路径 => 绝对 + 反斜杠
// - qrc:/ 或 :/ => 临时文件绝对路径 + 反斜杠
static QString toNativeWindowsPathForCover(const QString& coverRaw)
{
    if (coverRaw.trimmed().isEmpty()) return {};

    QString absPath;

    // 1) Qt 资源 -> 临时文件
    if (coverRaw.startsWith("qrc:/", Qt::CaseInsensitive) || coverRaw.startsWith(":/")) {
        const QString tempFile = materializeQtResourceToTempFile(coverRaw);
        if (tempFile.isEmpty()) return {};
        absPath = QFileInfo(tempFile).absoluteFilePath();
    }
    // 2) file:///...
    else if (coverRaw.startsWith("file:", Qt::CaseInsensitive)) {
        QUrl u(coverRaw);
        const QString local = u.toLocalFile(); // Windows 下通常是 E:/... 或 E:\...
        if (local.isEmpty()) return {};
        absPath = QFileInfo(local).absoluteFilePath();
    }
    // 3) 普通本地路径
    else {
        absPath = QFileInfo(QDir::cleanPath(coverRaw)).absoluteFilePath();
    }

    // 强制转换为 Windows 原生分隔符：E:\Temp\xxx.jpg
    absPath = QDir::toNativeSeparators(absPath);
    return absPath;
}
#endif

WinSMTCController::WinSMTCController(QObject* parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    initWinRT();
#endif
}

WinSMTCController::~WinSMTCController()
{
#ifdef Q_OS_WIN
    shutdownWinRT();
#endif
}

#ifdef Q_OS_WIN
bool WinSMTCController::ensureSmtcReady()
{
    if (m_smtc) return true;

    HWND hwnd = findMainHwnd();
    if (!hwnd) {
        qWarning() << "[SMTC] No HWND yet. Call after QML window is created/shown.";
        return false;
    }

    try {
        auto factory =
            winrt::get_activation_factory<SystemMediaTransportControls, ISystemMediaTransportControlsInterop>();

        SystemMediaTransportControls smtc{ nullptr };
        winrt::check_hresult(factory->GetForWindow(
            hwnd,
            winrt::guid_of<SystemMediaTransportControls>(),
            winrt::put_abi(smtc)
        ));

        m_smtc = smtc;
        m_smtc.IsEnabled(true);
        m_smtc.IsPlayEnabled(true);
        m_smtc.IsPauseEnabled(true);
        m_smtc.IsNextEnabled(true);
        m_smtc.IsPreviousEnabled(true);

        m_btnToken = m_smtc.ButtonPressed([this](auto const&, SystemMediaTransportControlsButtonPressedEventArgs const& e) {
            try {
                switch (e.Button()) {
                case SystemMediaTransportControlsButton::Play:     emit playRequested(); break;
                case SystemMediaTransportControlsButton::Pause:    emit pauseRequested(); break;
                case SystemMediaTransportControlsButton::Next:     emit nextRequested(); break;
                case SystemMediaTransportControlsButton::Previous: emit previousRequested(); break;
                default: break;
                }
            } catch (...) {}
        });

        m_seekToken = m_smtc.PlaybackPositionChangeRequested([this](auto const&, PlaybackPositionChangeRequestedEventArgs const& e) {
            try {
                const auto ts = e.RequestedPlaybackPosition();
                const qint64 requestedMs = ts.count() / 10000;
                emit seekRequested(requestedMs);
            } catch (...) {}
        });

        m_updater = m_smtc.DisplayUpdater();
        m_updater.Type(MediaPlaybackType::Music);

        qDebug() << "[SMTC] Ready (GetForWindow).";
        return true;

    } catch (winrt::hresult_error const& e) {
        qWarning() << "[SMTC] GetForWindow failed:" << QString::fromWCharArray(e.message().c_str());
    } catch (...) {
        qWarning() << "[SMTC] GetForWindow failed: unknown error";
    }

    return false;
}
#endif

void WinSMTCController::updateMetadata(const QJsonObject& info)
{
    m_lastMetadata = info;

#ifdef Q_OS_WIN
    if (!ensureSmtcReady()) return;

    try {
        // 1) 文本元数据
        auto music = m_updater.MusicProperties();
        if (info.contains("title"))  music.Title(hstring{ info["title"].toString().toStdWString() });
        if (info.contains("artist")) music.Artist(hstring{ info["artist"].toString().toStdWString() });
        if (info.contains("album"))  music.AlbumTitle(hstring{ info["album"].toString().toStdWString() });

        // 2) 清空旧封面 + 先更新文字
        m_updater.Thumbnail(nullptr);
        m_updater.Update();

        // 3) Timeline（切歌 position=0）
        qint64 rawDur = info.value("duration").toVariant().toLongLong();
        qint64 durationMs = (rawDur > 10000) ? rawDur : (rawDur * 1000);
        if (durationMs <= 0) durationMs = 1000;

        SystemMediaTransportControlsTimelineProperties t{};
        t.StartTime(msToTimeSpan(0));
        t.Position(msToTimeSpan(0));
        t.EndTime(msToTimeSpan(durationMs));
        t.MinSeekTime(msToTimeSpan(0));
        t.MaxSeekTime(msToTimeSpan(durationMs));
        m_smtc.UpdateTimelineProperties(t);

        // 4) 专辑图：严格按你要求的 StorageFile 方式
        const QString coverRaw = info.value("cover").toString();
        const QString nativePath = toNativeWindowsPathForCover(coverRaw);

        if (!nativePath.isEmpty() && QFileInfo::exists(nativePath)) {
            qDebug() << "[SMTC] Cover raw:" << coverRaw;
            qDebug() << "[SMTC] Cover native path:" << nativePath;

            const QString coverKey = coverRaw;
            QPointer<WinSMTCController> self(this);

            auto op = StorageFile::GetFileFromPathAsync(nativePath.toStdWString());
            op.Completed([self, coverKey, nativePath](auto const& async, auto const& status) {
                if (!self) return;

                // 先打出 WinRT async 的错误码，避免“失败但沉默”
                try {
                    auto asyncInfo = async.template as<Windows::Foundation::IAsyncInfo>();
                    const HRESULT hr = asyncInfo.ErrorCode();

                    if (status != AsyncStatus::Completed) {
                        QMetaObject::invokeMethod(self.data(), [hr, nativePath]() {
                            qWarning() << "[SMTC] GetFileFromPathAsync NOT completed. HRESULT="
                                       << Qt::hex << (qulonglong)hr
                                       << " path=" << nativePath;
                        }, Qt::QueuedConnection);
                        return;
                    }
                } catch (...) {
                    // ignore
                }

                try {
                    auto file = async.GetResults();
                    auto thumb = RandomAccessStreamReference::CreateFromFile(file);

                    QMetaObject::invokeMethod(self.data(), [self, coverKey, thumb]() {
                        if (!self) return;
                        if (!self->m_smtc) return;

                        // 切歌校验：避免旧异步结果覆盖新封面
                        if (self->m_lastMetadata.value("cover").toString() != coverKey) return;

                        try {
                            self->m_updater.Thumbnail(thumb);
                            self->m_updater.Update();
                            qDebug() << "[SMTC] Cover updated via StorageFile(CreateFromFile).";
                        } catch (...) {
                            qWarning() << "[SMTC] Failed to set thumbnail (exception).";
                        }
                    }, Qt::QueuedConnection);

                } catch (winrt::hresult_error const& e) {
                    QMetaObject::invokeMethod(self.data(), [msg = QString::fromWCharArray(e.message().c_str()), nativePath]() {
                        qWarning() << "[SMTC] GetResults/CreateFromFile WinRT error:" << msg
                                   << " path=" << nativePath;
                    }, Qt::QueuedConnection);
                } catch (...) {
                    QMetaObject::invokeMethod(self.data(), [nativePath]() {
                        qWarning() << "[SMTC] GetResults/CreateFromFile unknown error. path=" << nativePath;
                    }, Qt::QueuedConnection);
                }
            });
        } else {
            if (!coverRaw.isEmpty()) {
                qWarning() << "[SMTC] Cover path invalid/not found. raw=" << coverRaw
                           << " native=" << nativePath;
            }
        }

        m_lastUpdateTime = 0;
        m_lastSentMs = 0;

        qDebug() << "[SMTC] Metadata updated:" << info.value("title").toString();

    } catch (winrt::hresult_error const& e) {
        qWarning() << "[SMTC] updateMetadata WinRT error:" << QString::fromWCharArray(e.message().c_str());
    } catch (...) {
        qWarning() << "[SMTC] updateMetadata unknown error";
    }
#endif
}

void WinSMTCController::setPosition(qint64 ms)
{
#ifdef Q_OS_WIN
    if (!ensureSmtcReady()) return;

    try {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (std::llabs(ms - m_lastSentMs) <= 2000 && (now - m_lastUpdateTime) <= 5000) {
            return;
        }
        m_lastUpdateTime = now;
        m_lastSentMs = ms;

        qint64 rawDur = m_lastMetadata.value("duration").toVariant().toLongLong();
        qint64 durationMs = (rawDur > 10000) ? rawDur : (rawDur * 1000);
        if (durationMs <= 0) return;

        SystemMediaTransportControlsTimelineProperties t{};
        t.StartTime(msToTimeSpan(0));
        t.EndTime(msToTimeSpan(durationMs));
        t.Position(msToTimeSpan(ms));
        t.MinSeekTime(msToTimeSpan(0));
        t.MaxSeekTime(msToTimeSpan(durationMs));
        m_smtc.UpdateTimelineProperties(t);

    } catch (...) {}
#endif
}

void WinSMTCController::setPlaybackState(bool playing)
{
#ifdef Q_OS_WIN
    if (!ensureSmtcReady()) return;

    try {
        m_smtc.PlaybackStatus(playing ? MediaPlaybackStatus::Playing : MediaPlaybackStatus::Paused);
        m_lastUpdateTime = 0;
    } catch (...) {}
#endif
}

#ifdef Q_OS_WIN
void WinSMTCController::initWinRT()
{
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        qDebug() << "[SMTC] WinRT apartment initialized.";
    } catch (...) {
        qWarning() << "[SMTC] WinRT init failed.";
    }
}

void WinSMTCController::shutdownWinRT()
{
    try {
        if (m_smtc) {
            if (m_btnToken.value) {
                m_smtc.ButtonPressed(m_btnToken);
                m_btnToken = {};
            }
            if (m_seekToken.value) {
                m_smtc.PlaybackPositionChangeRequested(m_seekToken);
                m_seekToken = {};
            }
        }

        m_updater = nullptr;
        m_smtc = nullptr;

        winrt::uninit_apartment();
        qDebug() << "[SMTC] WinRT apartment uninitialized.";
    } catch (...) {}
}
#endif
