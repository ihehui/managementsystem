
#include <QtGui>
#include <QApplication>
#include <QSplashScreen>

#include "./mainwindow/mainwindow.h"
#include "./shared/app_constants.h"

#include "HHSharedCore/hlogdebug.h"




void showSplashMessage(QSplashScreen *s, const QString &str){
    s->showMessage(str, Qt::AlignRight | Qt::AlignBottom, Qt::darkGreen);
}


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForCStrings(codec);
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForTr(codec);

    QApplication app(argc, argv);
    app.addLibraryPath(QCoreApplication::applicationDirPath());
    app.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(PLUGINS_MAIN_DIR));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + QDir::separator () + QString(MYLIBS_DIR));
    qDebug()<<"--Library Paths:"<<app.libraryPaths ();


    for(int i = 0; i < argc; i++){
        if(QString(argv[i]).toLower() == "-log"){
            qInstallMessageHandler(logDebug);
            qAddPostRoutine(closeDebugLog);
        }
    }


    QDate date = QDate::currentDate();
    if(date.year() > 2015 ){
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Application has expired!"));
        qDebug()<<"Application has expired! Please update!";
        return 0;
    }

    //创建Splash Screen
    //Create Splash Screen
    QSplashScreen *splash = new QSplashScreen( QPixmap(QString(RESOURCE_PATH)+QString(APP_SPLASH_IMAGE_PATH)) );
    splash->show();

    //设置程序信息
    //Setup the program information
    showSplashMessage(splash, QObject::tr("Setup Application Info"));
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setWindowIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG_DOMAIN);
    showSplashMessage(splash, QString(APP_NAME) + " " + QString(APP_VERSION));


    //创建主窗口
    //Create the main window
    HEHUI::MainWindow w;
    showSplashMessage(splash, QObject::tr("Loading Plugins"));
    w.loadPlugins();
    w.show();


    //结束Splash Screen
    //Finish the Splash Screen
    splash->finish(&w);
    delete splash;


    return app.exec();

}
