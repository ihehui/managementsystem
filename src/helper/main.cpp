
#include <QtGui>
#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>

#include "helper.h"

#include "HHSharedCore/hlogdebug.h"
#include "HHSharedWindowsManagement/WinUtilities"



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

    QApplication a(argc, argv);
    a.addLibraryPath(QCoreApplication::applicationDirPath());
    qDebug()<<"--Library Paths:"<<a.libraryPaths ();


    QStringList arguments;
    for(int i = 0; i < argc; i++){
        arguments.append(QString(argv[i]));
    }
    if(arguments.contains("-lock")){
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("LOCK!"));

        HEHUI::WinUtilities::LockWindows();
        return 0;
    }
    if(arguments.contains("-logoff")){
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("LOGOFF!"));

        HEHUI::WinUtilities::Logoff(true);
        return 0;
    }


    QDate date = QDate::currentDate();
    if(date.year() > 2015 ){
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Application has expired!"));
        qDebug()<<"Application has expired! Please update!";
        return 0;
    }

    //创建Splash Screen
    //Create Splash Screen
    //QSplashScreen *splash = new QSplashScreen( QPixmap(QString(RESOURCE_PATH)+QString(APP_SPLASH_IMAGE_PATH)) );
    //splash->show();

    //设置程序信息
    //Setup the program information
    //showSplashMessage(splash, QObject::tr("Setup Application Info"));

    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);
    //a.setWindowIcon(QIcon(QString(RESOURCE_PATH)+QString(APP_ICON_PATH)));
    a.setOrganizationName(APP_ORG);
    a.setOrganizationDomain(APP_ORG_DOMAIN);

    //showSplashMessage(splash, QString(APP_NAME) + " " + QString(APP_VERSION));


    //创建主窗口
    //Create the main window
    HEHUI::Helper h;
    //showSplashMessage(splash, QObject::tr("Loading Plugins"));


    //结束Splash Screen
    //Finish the Splash Screen
    //splash->finish(&w);
    //delete splash;


    return a.exec();

}
