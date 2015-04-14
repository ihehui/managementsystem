

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "app_constants.h"
#include "serverservice/serverservice.h"

#include "HHSharedCore/hlogdebug.h"
#include "HHSharedCore/hglobal_core.h"

#include "../sharedms/settings.h"

#include <iostream>
using namespace std;


#include <QLibraryInfo>


int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);

//#if !defined(Q_WS_WIN)
//    // QtService stores service settings in SystemScope, which normally require root privileges.
//    // To allow testing this example as non-root, we change the directory of the SystemScope settings file.
//    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QDir::tempPath());
//    qWarning("(Example uses dummy settings file: %s/QtSoftware.conf)", QDir::tempPath().toLatin1().constData());
//#endif


    //QCoreApplication a(argc, argv);


    //HEHUI::ServerService service(argc, argv, "ServerService", "Server Service For Computer Management System");
    HEHUI::ServerService service(argc, argv, SERVICE_NAME, APP_NAME);
    cout<<qPrintable(QString(APP_NAME) + " Build " + QString(APP_VERSION))<<endl<<endl;



    //reset the message handler     
    qInstallMessageHandler(0);

    return service.exec();
}

