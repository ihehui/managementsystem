

#include <QHostInfo>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostAddress>

#include "./systeminfo.h"
#include "../sharedms/global_shared.h"

#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hhardwaremonitor.h"
#include "HHSharedWindowsManagement/WinUtilities"
#endif



namespace HEHUI {


bool SystemInfo::running = false;

SystemInfo::SystemInfo(QObject *parent) :
    QObject(parent)
{
    running = true;

}

SystemInfo::~SystemInfo() {
    qDebug()<<"SystemInfo::~SystemInfo()";

    running = false;
}

bool SystemInfo::isRunning(){
    return running;
}


//void SystemInfo::run(){
////    getOSInfo();
////    getHardwareInfo();
////    getInstalledSoftwareInfo();
////    getServicesInfo();

////    exit();
//}


//void SystemInfo::getSystemInfo(){
//    getOSInfo();
//    getHardwareInfo();

//}

QByteArray SystemInfo::getOSInfo(){

    QJsonObject obj;
    obj["ComputerName"] = QHostInfo::localHostName().toLower();

#ifdef Q_OS_WIN32
    HardwareMonitor hwm;

    hwm.getOSInfo(&obj);

    QStringList users = WinUtilities::localCreatedUsers();
    WinUtilities::getAllUsersLoggedOn(&users);
    users.removeDuplicates();
    obj["Users"] = users.join(";");

    QStringList admins = WinUtilities::getMembersOfLocalGroup("Administrators");
    obj["Admins"] = admins.join(";");


    bool isJoinedToDomain = false;
    QString joinInfo = WinUtilities::getJoinInformation(&isJoinedToDomain);
    if(isJoinedToDomain){
        WinUtilities::getComputerNameInfo(&joinInfo, 0, 0);
    }
    obj["Workgroup"] = joinInfo;
    obj["JoinedToDomain"] = QString::number(isJoinedToDomain?1:0);

#else


#endif

    QStringList ipAddresses;
    foreach (QNetworkInterface nic, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, nic.addressEntries()) {
            qDebug()<<"entry.ip():"<<entry.ip().toString();
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
                ipAddresses << entry.ip().toString() << "/" << nic.hardwareAddress();
            }
        }
    }

    obj["IPInfo"] = ipAddresses.join(";");



    QJsonObject object;
    object["OS"] = obj;
    QJsonDocument doc(object);
    //emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), socketID);

    return doc.toJson(QJsonDocument::Compact);
}

void SystemInfo::getHardwareInfo(SOCKETID socketID){

    QJsonObject obj;
    obj["ComputerName"] = QHostInfo::localHostName();

#ifdef Q_OS_WIN32
    HardwareMonitor hwm;

    hwm.getOSInfo(&obj);
    hwm.getBaseBoardInfo(&obj);
    hwm.getProcessorInfo(&obj);
    hwm.getPhysicalMemoryInfo(&obj);
    hwm.getDiskDriveInfo(&obj);
    hwm.getVideoControllerInfo(&obj);
    hwm.getSoundDeviceInfo(&obj);
    hwm.getMonitorInfo(&obj);
    hwm.getNetworkAdapterInfo(&obj);


    QStringList users = WinUtilities::localCreatedUsers();
    WinUtilities::getAllUsersLoggedOn(&users);
    users.removeDuplicates();
    obj["Users"] = users.join(";");

    bool isJoinedToDomain = false;
    QString joinInfo = WinUtilities::getJoinInformation(&isJoinedToDomain);
    if(isJoinedToDomain){
        WinUtilities::getComputerNameInfo(&joinInfo, 0, 0);
    }
    obj["Workgroup"] = joinInfo;

#else


#endif

    QJsonObject object;
    object["Hardware"] = obj;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact),  MS::SYSINFO_HARDWARE, socketID);

}

void SystemInfo::getInstalledSoftwareInfo(SOCKETID socketID){

    QString rootKey = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

    QStringList keys64;
    WinUtilities::regEnumKey(rootKey, &keys64, true);

    QStringList keys32;
    WinUtilities::regEnumKey(rootKey, &keys32, false);


    foreach (QString key, keys64) {
        if(keys32.contains(key)){
            keys64.removeAll(key);
        }
    }

    QJsonArray infoArray;
    getInstalledSoftwareInfo(&infoArray, keys32, false);
    getInstalledSoftwareInfo(&infoArray, keys64, true);

    QJsonObject object;
    object["Software"] = infoArray;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), MS::SYSINFO_SOFTWARE, socketID);

}

void SystemInfo::getInstalledSoftwareInfo(QJsonArray *infoArray, const QStringList &keys, bool on64BitView){

    if(!infoArray){return;}
    if(keys.isEmpty()){return;}

    QString rootKey = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

    foreach (QString key, keys) {
        QString path = rootKey + "\\" + key;
        QString displayName, displayVersion, installDate, publisher;
        bool ok = WinUtilities::regRead(path, "DisplayName", &displayName, on64BitView);
        if(!ok){continue;}

        QJsonArray jsonArray;
        jsonArray.append(displayName);


        WinUtilities::regRead(path, "DisplayVersion", &displayVersion, on64BitView);
        WinUtilities::regRead(path, "InstallDate", &installDate, on64BitView);
        WinUtilities::regRead(path, "Publisher", &publisher, on64BitView);

        jsonArray.append(displayVersion);
        jsonArray.append(installDate);
        jsonArray.append(publisher);

        infoArray->append(jsonArray);

    }

}

void SystemInfo::getServicesInfo(SOCKETID socketID){

    QJsonArray infoArray;
    HEHUI::WinUtilities::serviceGetAllServicesInfo(&infoArray);

    QJsonObject object;
    object["Service"] = infoArray;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), MS::SYSINFO_SERVICES, socketID);

}












} //namespace HEHUI
