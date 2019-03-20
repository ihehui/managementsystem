
#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "app_constants.h"
#include "clientservice/clientservice.h"

//#include "HHSharedCore/LogDebug"
#include "HHSharedCore/MessageLogger"
#include "HHSharedCore/CrashHandler"




#include <iostream>


int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);


//#if !defined(Q_WS_WIN)
//    // QtService stores service settings in SystemScope, which normally require root privileges.
//    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
//    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
//    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
//#endif

    //QCoreApplication app(argc, argv);


    //HEHUI::ClientService service(argc, argv, "ClientService", "Client Service For Computer Management System");
    HEHUI::ClientService service(argc, argv, SERVICE_NAME, APP_NAME);

    std::cout << qPrintable(QString(APP_NAME) + " Build " + QString(APP_VERSION)) << std::endl << std::endl;

    setupCrashHandler();


    //reset the message handler
    //qInstallMessageHandler(0);



    return service.exec();

}

