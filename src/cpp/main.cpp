#include <ApiClient.h>
#include <Http.h>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QLocale>
#include <QJsonObject>
#include <QDebug>

#include <MusicPlayer.h>
#include "WinSMTCController.h"
#include "AudioProcessor.h"
#include "Decryptor/KRCDecryptor.h"
#include <QProcess>
#include <SystemMediaTransportControlsInterop.h>
#include "KLyricsParser.h"
#include <iostream>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shobjidl.h>   // SetCurrentProcessExplicitAppUserModelID
#endif

// 强制弹出控制台用于调试（建议仅 Debug 使用）
static void openConsole()
{
#ifdef _WIN32
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    std::cout << "Console Initialized..." << std::endl;
}

#ifdef Q_OS_WIN
// 必须尽可能早：在创建任何窗口/UI 之前设置 AUMID
static void setAumidEarly()
{
    // 建议不要带版本号，保持长期稳定
    const wchar_t* appId = L"FeelLiao.Fmusic.Player";

    HRESULT hr = SetCurrentProcessExplicitAppUserModelID(appId);
    if (SUCCEEDED(hr)) {
        qDebug() << "Successfully set AUMID to:" << QString::fromWCharArray(appId);
    } else {
        qWarning() << "Failed to set AUMID. HRESULT =" << Qt::hex << (qulonglong)hr;
    }
}
#endif

int main(int argc, char *argv[])
{
    openConsole();
    qDebug() << "--- Program Starting ---";

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef Q_OS_WIN
    // 关键改动：放到 QApplication 之前
    setAumidEarly();
#endif

    QApplication app(argc, argv);

    // 建议补齐 Qt 应用身份（有助于某些系统 UI 显示一致性）
    QCoreApplication::setOrganizationName("FeelLiao");
    QCoreApplication::setOrganizationDomain("feelliao.com");
    QCoreApplication::setApplicationName("Fmusic");

    // 1) 翻译加载
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "nice_" + QLocale(locale).name();
        if (translator.load("./i18n/" + baseName))
        {
            app.installTranslator(&translator);
            break;
        }
    }

    // 2) 初始化引擎和播放器逻辑
    QQmlApplicationEngine engine;
    MusicPlayer musicPlayer;

#ifdef Q_OS_WIN
    // 改动：使用堆对象，确保生命周期贯穿整个应用
    auto smtc = std::make_unique<WinSMTCController>(&engine);

    // 连接系统媒体键 -> 播放器
    QObject::connect(smtc.get(), &WinSMTCController::playRequested,     &musicPlayer, &MusicPlayer::play);
    QObject::connect(smtc.get(), &WinSMTCController::pauseRequested,    &musicPlayer, &MusicPlayer::pause);
    QObject::connect(smtc.get(), &WinSMTCController::nextRequested,     &musicPlayer, &MusicPlayer::next);
    QObject::connect(smtc.get(), &WinSMTCController::previousRequested, &musicPlayer, &MusicPlayer::previous);
    QObject::connect(smtc.get(), &WinSMTCController::seekRequested,     &musicPlayer, &MusicPlayer::seek);

    // 播放器 -> SMTC
    QObject::connect(&musicPlayer, &MusicPlayer::currentTrackInfoChanged, smtc.get(), &WinSMTCController::updateMetadata);
    QObject::connect(&musicPlayer, &MusicPlayer::playingChanged,          smtc.get(), &WinSMTCController::setPlaybackState);
    QObject::connect(&musicPlayer, &MusicPlayer::positionChanged,         smtc.get(), &WinSMTCController::setPosition);
#endif

    // 3) 注册上下文属性
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty("musicPlayer", &musicPlayer);

    // 4) 加载主界面
    const QUrl url(QStringLiteral("qrc:/App.qml"));

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app,
        [&](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
                return;
            }

#ifdef Q_OS_WIN
            // 可选：若 MusicPlayer 在启动时已有“当前曲目/播放状态”，
            // 你可以在这里主动推送一次，避免 SMTC 初始为空。
            // 下面两行需要你在 MusicPlayer 提供相应 getter；若没有就保持注释，不影响编译。
            //
            // smtc->updateMetadata(musicPlayer.currentTrackInfo());
            // smtc->setPlaybackState(musicPlayer.isPlaying());
#endif
        },
        Qt::QueuedConnection
    );

    engine.load(url);

    // 5) 业务请求 (ApiClient)
    auto api = new ApiClient(&engine);
    QJsonObject body;
    body["pageNum"] = 1;
    body["pageSize"] = 10;

    api->getAllSongs(
        body,
        [=](const Http::Result &res) {
            if (res.code == 0) qDebug() << "Songs loaded success";
        },
        [=](const QString &msg, int status) {
            Q_UNUSED(status);
            qWarning() << "Network error:" << msg;
        }
    );

    return app.exec();
}
