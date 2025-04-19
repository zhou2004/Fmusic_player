#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QQmlContext>
#include <MusicPlayer.h>

#include "Iconvert.h"
#include "Sql/mysql.h"

#include "MyController.h"
#include "FAlbumArtManager.h"
#include "Lyrics.h"
#include "AudioProcessor.h"
#include "BluetoothAudioMonitor.h"
#include "Decryptor/KRCDecryptor.h"
#include "Decryptor/KLyricsParser.h"

#include "Sql/touser.h"
#include "QJson.h"
int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif


    QApplication app(argc, argv);

// #ifdef _WIN32
//     // 分配控制台窗口
//     AllocConsole();
//     // 重定向标准输出到控制台
//     freopen("CONOUT$", "w", stdout);
//     freopen("CONOUT$", "w", stderr);
// #endif

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "nice_" + QLocale(locale).name();
        if (translator.load("./i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/App.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);


    // 注册类型
    qmlRegisterType<UserFullInfo>("CustomTypes", 1, 0, "UserFullInfo");

    //将qml的自定义组件注册到qml中
    engine.addImportPath("qrc:/");


    engine.load(url);

    MusicPlayer musicPlayer;

    FAlbumArtManager albumArtManager(&musicPlayer);  // 将 musicPlayer 的实例传递给 albumArtManager

    Lyrics lyrics(&musicPlayer); // 将 musicPlayer 的实例传递给 lyrics

    AudioSpectrumAnalyzer analyzer(&musicPlayer);  // 将 musicPlayer 的实例传递给 analyzer

    K_LyricParser parser;

    BluetoothAudioMonitor bluetooh(&musicPlayer);




    // 将 myController 注册到 QML 上下文
    engine.rootContext()->setContextProperty("myController", new MyController(&engine));

    engine.rootContext()->setContextProperty("musicPlayer", &musicPlayer);

    // 注册 FAlbumArtManager
    engine.rootContext()->setContextProperty("albumartManager", &albumArtManager);

    // 注册 lyrics
    engine.rootContext()->setContextProperty("lyrics", &lyrics);

    // 注册 analyzer
    engine.rootContext()->setContextProperty("analyzer", &analyzer);



    // Mysql mysql;
    //
    // if (mysql.init_db_connection()) {
    //     mysql.delUserCollections(1,3);
    // }

    QJson qjson;
    // qjson.get_token();
    qjson.kugou_search_music("唯一");


    // 用户操作测试
    // Mysql userManager;
    // UserCreateInfo newUser;
    // newUser.password = "password123";
    // newUser.phone = "13800138000";
    // newUser.email = "user1@example.com";
    // newUser.nickname = "Alice";
    //
    // qint64 userId = userManager.createUser(newUser);
    // if (userId == -1) {
    //     qCritical() << "创建用户失败";
    //     // return -1;
    // }

    // UserFullInfo userInfo = userManager.getUserById(1);



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