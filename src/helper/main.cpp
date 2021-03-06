
#include <QtGui>
#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>

#include "helper.h"

//#include "HHSharedCore/hlogdebug.h"
#include "HHSharedCore/MessageLogger"
#include "HHSharedCore/CrashHandler"

#include "../sharedms/settings.h"

#ifdef Q_OS_WIN32
    #include "HHSharedSystemUtilities/WinUtilities"
#endif

void showSplashMessage(QSplashScreen *s, const QString &str)
{
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
    app.setQuitOnLastWindowClosed(false);

    app.addLibraryPath(QCoreApplication::applicationDirPath());
    qDebug() << "--Library Paths:" << app.libraryPaths ();


    setupCrashHandler();


    QStringList arguments;
    for(int i = 0; i < argc; i++) {
        arguments.append(QString(argv[i]));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////
    QString name = QString(APP_NAME).remove(" ");
    HEHUI::Settings settings(name);
    if(arguments.contains("-log", Qt::CaseInsensitive)){
        settings.enableLog(true, name);
    }else{
        settings.enableLog(false);
    }
    LOGWARNING<<"Application started.";
    /////////////////////////////////////////////////////////////////////////////////////////////////




#ifdef Q_OS_WIN32

    if(arguments.contains("-lock")) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("LOCK!"));

        HEHUI::WinUtilities::LockWindows();
        return 0;
    }
    if(arguments.contains("-logoff")) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("LOGOFF!"));

        HEHUI::WinUtilities::Logoff(true);
        return 0;
    }
#endif

    QDate date = QDate::currentDate();
    if(date.year() > 2019 ) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Application has expired!"));
        qDebug() << "Application has expired! Please update!";
        return 0;
    }

    //创建Splash Screen
    //Create Splash Screen
    //QSplashScreen *splash = new QSplashScreen( QPixmap(QString(RESOURCE_PATH)+QString(APP_SPLASH_IMAGE_PATH)) );
    //splash->show();

    //设置程序信息
    //Setup the program information
    //showSplashMessage(splash, QObject::tr("Setup Application Info"));

    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    //a.setWindowIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    app.setOrganizationName(APP_ORG);
    app.setOrganizationDomain(APP_ORG_DOMAIN);

    //showSplashMessage(splash, QString(APP_NAME) + " " + QString(APP_VERSION));


    //创建主窗口
    //Create the main window
    HEHUI::Helper helper;
    //showSplashMessage(splash, QObject::tr("Loading Plugins"));


    //结束Splash Screen
    //Finish the Splash Screen
    //splash->finish(&w);
    //delete splash;


    return app.exec();

}
