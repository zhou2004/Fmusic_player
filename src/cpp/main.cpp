#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>

#include <QQmlContext>
#include <MusicPlayer.h>

#include <QProcess>

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


    //启动node.js-api子进程
    auto* subProcess = new QProcess();
    subProcess->start("app_win.exe");

    //启动图形化界面主进程
    QApplication app(argc, argv);

    // 连接信号和槽，当主进程退出时，结束子进程
    QObject::connect(&app, &QApplication::aboutToQuit, [subProcess]() {
        qDebug() << "主进程即将退出，尝试结束子进程...";
        subProcess->terminate();
        if (!subProcess->waitForFinished(3000)) { // 等待 3 秒
            qDebug() << "正常终止失败，强制结束子进程...";
            subProcess->kill();
            subProcess->waitForFinished(); // 确保子进程已结束
        }
        qDebug() << "子进程已结束。";
    });

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

    QJson qjson;

    FileDownloader downloader;




    // 将 myController 注册到 QML 上下文
    engine.rootContext()->setContextProperty("myController", new MyController(&engine));

    engine.rootContext()->setContextProperty("musicPlayer", &musicPlayer);

    // 注册 FAlbumArtManager
    engine.rootContext()->setContextProperty("albumartManager", &albumArtManager);

    // 注册 lyrics
    engine.rootContext()->setContextProperty("lyrics", &lyrics);

    // 注册 analyzer
    engine.rootContext()->setContextProperty("analyzer", &analyzer);

    //注册json类
    engine.rootContext()->setContextProperty("qjson", &qjson);

    //注册download类
    engine.rootContext()->setContextProperty("downloader", &downloader);



    // Mysql mysql;
    //
    // if (mysql.init_db_connection()) {
    //     mysql.addUserCollections(1,3);
    // }

    // QJson qjson;
    // qjson.get_token();
    // qjson.kugou_search_music("唯一");
    // qjson.kugou_recommend();


    // 要写入的数据
    // QString data = R"({
    //     "data": {
    //         "is_vip": 1,
    //         "servertime": "2025-06-29 16:37:58",
    //         "roam_type": 0,
    //         "t1": "19c11068ffae0f0e61957bbd3cd86c2adfc60f6757573db2110f4388444dd3c86abc139966301b186096d48a74da926d5f2d4c8e0c493a53d994dcb990c98db3726233ddcc3c401c994c83f1f1b23686",
    //         "reg_time": "2015-05-23 21:46:00",
    //         "vip_type": 6,
    //         "birthday_mmdd": "",
    //         "expire_time": "2021-07-03 00:26:55",
    //         "userid": 634877158,
    //         "listen_end_time": "",
    //         "su_vip_end_time": "2025-07-09 21:36:20",
    //         "su_vip_y_endtime": "",
    //         "user_type": 29,
    //         "username": "kgopen634877158",
    //         "qq": 1,
    //         "exp": 0,
    //         "m_end_time": "2025-07-09 21:36:20",
    //         "score": 0,
    //         "bookvip_valid": 1,
    //         "bookvip_end_time": "2025-07-09 21:36:21",
    //         "arttoy_avatar": "",
    //         "totp_server_timestamp": 1751186278,
    //         "secu_params": "3c13545b0e6d6e34aa5b604b822dbd1ab136b55fa95bab1cc6a8d738cb396abdebcb0696ce7f09f7aeffb0382fde1d00a4dd795f9c5ac79db7299ac70efc22d8a9c71ac03840b87e4662ecbb4a5c08d7",
    //         "roam_end_time": "",
    //         "totp_key": "MPKWKKTN4GK5ALJ5DRVI4B4SCFCPAYT4",
    //         "su_vip_begin_time": "2025-06-08 21:36:20",
    //         "sex": 1,
    //         "roam_begin_time": "",
    //         "vip_end_time": "2025-07-09 21:36:20",
    //         "vip_token": "1605f1613cb551c1644e693872124e91",
    //         "listen_type": 0,
    //         "m_begin_time": "2025-06-08 21:36:20",
    //         "nickname": "feelzhou",
    //         "mobile": 1,
    //         "su_vip_clearday": "2025-06-08 21:36:20",
    //         "m_type": 1,
    //         "listen_begin_time": "",
    //         "bookvip_rankvip": [
    //             {
    //                 "extend": {
    //                     "day_limit": 1
    //                 },
    //                 "sub_biz_type": "member",
    //                 "vip_valid": 0,
    //                 "product_type": "tvip",
    //                 "vip_begin_time": "2022-06-29 13:44:29",
    //                 "discount_info": {},
    //                 "vip_end_time": "2022-06-29 14:14:29",
    //                 "free_period": 0,
    //                 "give_vip": 0,
    //                 "rank_level": 2,
    //                 "busi_type": "bookvip"
    //             }
    //         ],
    //         "y_type": 0,
    //         "roam_list": {},
    //         "user_y_type": 0,
    //         "pic": "<url id=\"d1gfp2dkfv3t44ibcia0\" type=\"url\" status=\"failed\" title=\"\" wc=\"0\">http://imge.kugou.com/kugouicon/165/20170813/20170813174914702919.jpg</url> ",
    //         "bc_code": "",
    //         "t_expire_time": 1753778278,
    //         "vip_begin_time": "2025-06-08 21:36:20",
    //         "birthday": "",
    //         "m_is_old": 0,
    //         "wechat": 1,
    //         "token": "55e3003fdca031aa498d78dbb77aa942059137e0ddf5447cfda103666c8dead0"
    //     },
    //     "status": 1,
    //     "error_code": 0
    // })";

    // 写入数据到文件
    // qjson.writeDataToFile(data);

    qjson.extractToken();
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