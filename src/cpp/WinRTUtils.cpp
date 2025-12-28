#include "WinRTUtils.h"

#ifdef ENABLE_WINRT

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
#include <windows.h>
#include <SystemMediaTransportControlsInterop.h>
#include <winrt/base.h>
#include <winrt/Windows.System.Profile.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Security.ExchangeActiveSyncProvisioning.h>

// 使用 WinRT 命名空间简化代码
using namespace winrt;
using namespace winrt::Windows::System::Profile;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Security::ExchangeActiveSyncProvisioning;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

static inline TimeSpan msToTimeSpan(qint64 ms)
{
    if (ms < 0) ms = 0;
    return TimeSpan{ ms * 10000 }; // 100ns ticks
}


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

static QString toNativeWindowsPathForCover(const QString& coverRaw)
{
    if (coverRaw.trimmed().isEmpty()) return {};

    QString absPath;

    if (coverRaw.startsWith("qrc:/", Qt::CaseInsensitive) || coverRaw.startsWith(":/")) {
        const QString tempFile = materializeQtResourceToTempFile(coverRaw);
        if (tempFile.isEmpty()) return {};
        absPath = QFileInfo(tempFile).absoluteFilePath();
    }

    else if (coverRaw.startsWith("file:", Qt::CaseInsensitive)) {
        QUrl u(coverRaw);
        const QString local = u.toLocalFile();
        if (local.isEmpty()) return {};
        absPath = QFileInfo(local).absoluteFilePath();
    }

    else {
        absPath = QFileInfo(QDir::cleanPath(coverRaw)).absoluteFilePath();
    }

    absPath = QDir::toNativeSeparators(absPath);
    return absPath;
}

WinSMTCController::WinSMTCController(QObject* parent) : QObject(parent)
{
    initSMTC();
}

WinSMTCController::~WinSMTCController()
{
    shutdownSMTC();
}

bool WinSMTCController::ensureSmtcReady()
{
    if (m_smtc) return true;

    HWND hwnd = findMainHwnd();
    if (!hwnd) {
        qWarning() << "WinRT SMTC No HWND yet. Call after QML window is created/shown.";
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

void WinSMTCController::updateMetadata(const QJsonObject& info)
{
    m_lastMetadata = info;

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

        // 4) 专辑图
        const QString coverRaw = info.value("cover").toString();
        const QString nativePath = toNativeWindowsPathForCover(coverRaw);

        if (!nativePath.isEmpty() && QFileInfo::exists(nativePath)) {
            // qDebug() << "[SMTC] Cover native path:" << nativePath;

            const QString coverKey = coverRaw;
            QPointer<WinSMTCController> self(this);

            auto op = StorageFile::GetFileFromPathAsync(nativePath.toStdWString());
            op.Completed([self, coverKey, nativePath](auto const& async, auto const& status) {
                if (!self) return;

                try {
                    auto asyncInfo = async.template as<Windows::Foundation::IAsyncInfo>();
                    const HRESULT hr = asyncInfo.ErrorCode();

                    if (status != AsyncStatus::Completed) {
                         QMetaObject::invokeMethod(self.data(), [hr, nativePath]() {
                            qWarning() << "[SMTC] GetFileFromPathAsync NOT completed. HRESULT="
                                       << Qt::hex << (qulonglong)hr << " path=" << nativePath;
                         }, Qt::QueuedConnection);
                         return;
                    }
                } catch (...) {}

                try {
                    auto file = async.GetResults();
                    auto thumb = RandomAccessStreamReference::CreateFromFile(file);

                    QMetaObject::invokeMethod(self.data(), [self, coverKey, thumb]() {
                        if (!self) return;
                        if (!self->m_smtc) return;
                        // 切歌校验
                        if (self->m_lastMetadata.value("cover").toString() != coverKey) return;

                        try {
                            self->m_updater.Thumbnail(thumb);
                            self->m_updater.Update();
                        } catch (...) {}
                    }, Qt::QueuedConnection);

                } catch (winrt::hresult_error const& e) {
                    QMetaObject::invokeMethod(self.data(), [msg = QString::fromWCharArray(e.message().c_str())]() {
                         qWarning() << "[SMTC] WinRT error:" << msg;
                    }, Qt::QueuedConnection);
                } catch (...) {}
            });
        }

        m_lastUpdateTime = 0;
        m_lastSentMs = 0;

    } catch (winrt::hresult_error const& e) {
        qWarning() << "[SMTC] updateMetadata WinRT error:" << QString::fromWCharArray(e.message().c_str());
    } catch (...) {}
}

void WinSMTCController::setPosition(qint64 ms)
{
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
}

void WinSMTCController::setPlaybackState(bool playing)
{
    if (!ensureSmtcReady()) return;
    try {
        m_smtc.PlaybackStatus(playing ? MediaPlaybackStatus::Playing : MediaPlaybackStatus::Paused);
        m_lastUpdateTime = 0;
    } catch (...) {}
}

void WinSMTCController::initSMTC()
{
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        qDebug() << "WinRT SMTC support element successfully initialized.";
    } catch (...) {
        qWarning() << "WinRT SMTC support element initialize failed.";
    }
}

void WinSMTCController::shutdownSMTC()
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
        qDebug() << "WinRT SMTC support element shutdown.";
    } catch (...) {}
}

QString DeviceInfo::getWindowsDeviceId()
{
    try {
        // 1. 获取系统标识 (针对当前发布者)
        // 注意：这需要你的应用有合法的 Identity (AppxManifest.xml)
        SystemIdentificationInfo systemIdInfo = SystemIdentification::GetSystemIdForPublisher();

        if (!systemIdInfo) {
            return "WinRT DeviceInfo Error: SystemIdInfo is null";
        }

        // 2. 拿到原始的二进制 buffer
        IBuffer buffer = systemIdInfo.Id();

        // 3. 使用 WinRT 工具转换成 Hex 字符串
        hstring hexString = CryptographicBuffer::EncodeToHexString(buffer);

        // 4. 转换为 Qt 的 QString (hstring -> wchar_t* -> QString)
        QString deviceID = QString::fromWCharArray(hexString.c_str());
        qDebug() << "WinRT DeviceInfo Device ID: " << deviceID;
        return deviceID;

    } catch (const hresult_error& ex) {
        // 捕获 WinRT 特有的异常
        QString errorMsg = QString::fromWCharArray(ex.message().c_str());
        qWarning() << "WinRT DeviceInfo Error getting Device ID:" << errorMsg;
        return "Unknown-Device-ID";
    } catch (...) {
        return "Unknown-Device-ID";
    }
}


#endif