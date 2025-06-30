#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>


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



    engine.load(url);


    return app.exec();
}