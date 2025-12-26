#include <ApiClient.h>
#include <Http.h>

#include <QApplication> // FluentUI 依赖 Widgets，必须用 QApplication
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QLocale>
#include <QJsonObject>
#include <QDebug>
#include <QDir> // 【新增】用于处理路径

#include <MusicPlayer.h>
#if defined(ENABLE_SMTC) && defined(Q_OS_WIN)
#include "WinSMTCController.h"
#endif
#include "AudioProcessor.h"
#include "KRCDecryptor.h"
#include <QProcess>
#include <SystemMediaTransportControlsInterop.h>
#include "KLyricsParser.h"
#include <iostream>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shobjidl.h>
#endif

// 强制弹出控制台用于调试
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
// 设置 AUMID
static void setAumidEarly()
{
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
    setAumidEarly();
#endif

    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("FeelLiao");
    QCoreApplication::setOrganizationDomain("feelliao.com");
    QCoreApplication::setApplicationName("Fmusic");

    // =========================================================
    // 1. 翻译加载 (适配 CMake 的 copy_if_different 逻辑)
    // =========================================================
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    // 获取 exe 所在的绝对目录 (例如 build/Release)
    QString appPath = QCoreApplication::applicationDirPath();
    // 拼接 i18n 文件夹路径
    QString i18nPath = QDir::toNativeSeparators(appPath + "/i18n");

    qDebug() << "Searching for translations in:" << i18nPath;

    for (const QString &locale : uiLanguages)
    {
        // 你的 CMake 自动生成的通常是 zh_CN.qm, en_US.qm 等
        const QString baseName = QLocale(locale).name();

        // 尝试从 i18nPath 目录加载名为 baseName 的文件
        if (translator.load(baseName, i18nPath))
        {
            app.installTranslator(&translator);
            qDebug() << "Loaded translation:" << baseName;
            break;
        }
    }

    // =========================================================
    // 2. 初始化核心对象
    // =========================================================
    QQmlApplicationEngine engine;

    // 栈上对象，生命周期随 main 函数
    MusicPlayer musicPlayer;
    K_LyricParser lyricParser;
    ApiClient apiClient;
    AudioProcessor audioProcessor(&musicPlayer);


#if defined(ENABLE_SMTC) && defined(Q_OS_WIN)
    auto smtc = std::make_unique<WinSMTCController>(&engine);

    QObject::connect(smtc.get(), &WinSMTCController::playRequested,     &musicPlayer, &MusicPlayer::play);
    QObject::connect(smtc.get(), &WinSMTCController::pauseRequested,    &musicPlayer, &MusicPlayer::pause);
    QObject::connect(smtc.get(), &WinSMTCController::nextRequested,     &musicPlayer, &MusicPlayer::next);
    QObject::connect(smtc.get(), &WinSMTCController::previousRequested, &musicPlayer, &MusicPlayer::previous);
    QObject::connect(smtc.get(), &WinSMTCController::seekRequested,     &musicPlayer, &MusicPlayer::seek);

    QObject::connect(&musicPlayer, &MusicPlayer::currentTrackInfoChanged, smtc.get(), &WinSMTCController::updateMetadata);
    QObject::connect(&musicPlayer, &MusicPlayer::playingChanged,          smtc.get(), &WinSMTCController::setPlaybackState);
    QObject::connect(&musicPlayer, &MusicPlayer::positionChanged,         smtc.get(), &WinSMTCController::setPosition);
#endif

    // =========================================================
    // 3. 注册上下文属性
    // =========================================================
    // 注意：在新策略下，手动 addImportPath("qrc:/") 通常是不需要的，
    // Qt 会自动处理模块导入路径。

    engine.rootContext()->setContextProperty("musicPlayer", &musicPlayer);
    engine.rootContext()->setContextProperty("apiClient", &apiClient);
    engine.rootContext()->setContextProperty("lyricParser", &lyricParser);
    engine.rootContext()->setContextProperty("audioProcessor", &audioProcessor);

    // =========================================================
    // 4. 加载主界面 (适配 QTP0004 NEW 策略)
    // =========================================================
    // 你的 URI 是 "FMusic"，所以路径必须包含 /qt/qml/FMusic/
    const QUrl url(u"qrc:/qt/qml/FMusic/App.qml"_qs);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app,
        [&](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl) {
                qCritical() << "Engine failed to load QML file:" << url;
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection
    );

    engine.load(url);



    return app.exec();
}