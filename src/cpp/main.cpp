#include <ApiClient.h>
#include <Http.h>
#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>

#include <QQmlContext>
#include <MusicPlayer.h>

#include <QProcess>

#include "Iconvert.h"

#include "Lyrics.h"
#include "AudioProcessor.h"
#include "Decryptor/KRCDecryptor.h"
#include "Decryptor/KLyricsParser.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif


    //启动图形化界面主进程
    QApplication app(argc, argv);



    //进行翻译工作
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "nice_" + QLocale(locale).name();
        if (translator.load("./i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    //进行qml引擎注册
    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/App.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);


    //将qml的自定义组件注册到qml中
    engine.addImportPath("qrc:/");


    engine.load(url);



    // 用 engine 当 QObject 父对象，方便生命周期管理
    auto api = new ApiClient(&engine);

    QJsonObject body;
    body["pageNum"]   = 1;
    body["pageSize"]  = 10;
    body["songName"]  = "";
    body["artistName"] = "";
    body["album"]     = "";

    // 这里 main 里没有 this，直接用 [=] 或 [&] 捕获即可
    api->getAllSongs(
        body,
        // onSuccess
        [=](const Http::Result &res) {
            if (res.code == 0) {
                qDebug() << "获取成功:" << res.data;
                // TODO: 在这里处理数据，比如传给某个 model / 控制器
            } else {
                qWarning() << "获取失败:" << res.message;
            }
        },
        // onError
        [=](const QString &msg, int status) {
            qWarning() << "网络错误" << status << msg;
        }
    );




    MusicPlayer musicPlayer;
    //
    // FAlbumArtManager albumArtManager(&musicPlayer);  // 将 musicPlayer 的实例传递给 albumArtManager
    //
    // Lyrics lyrics(&musicPlayer); // 将 musicPlayer 的实例传递给 lyrics
    //
    // AudioSpectrumAnalyzer analyzer(&musicPlayer);  // 将 musicPlayer 的实例传递给 analyzer
    //
    // K_LyricParser parser;
    //
    // Http qjson;
    //
    // FileDownloader downloader;
    //
    //
    //
    //
    // // 将 myController 注册到 QML 上下文
    // // engine.rootContext()->setContextProperty("myController", new MyController(&engine));
    //
    engine.rootContext()->setContextProperty("musicPlayer", &musicPlayer);
    //
    // // 注册 FAlbumArtManager
    // engine.rootContext()->setContextProperty("albumartManager", &albumArtManager);
    //
    // // 注册 lyrics
    // engine.rootContext()->setContextProperty("lyrics", &lyrics);
    //
    // // 注册 analyzer
    // // engine.rootContext()->setContextProperty("analyzer", &analyzer);
    //
    // //注册json类
    // engine.rootContext()->setContextProperty("qjson", &qjson);

    //注册download类
    // engine.rootContext()->setContextProperty("downloader", &downloader);



    // std::string fileName = "D:/KuGou/KugouMusic/sanguo.krc";
    // KRCDecoder decoder;
    // decoder._load(fileName);
    // K_LyricParser KlyricParser;
    // std::string decodedData = decoder.getDecoded();
    // K_LyricData lyricData = KlyricParser.parseLyrics(decodedData);
    //
    // std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap = KlyricParser.buildTimestampMap(lyricData);
    //
    // int playbackPosition = 215000;
    //
    // KlyricParser.findLyricAtTime(playbackPosition);
    //
    // KlyricParser.Q_getLyrics();


    return app.exec();
}