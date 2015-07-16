

#include <QHostInfo>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostAddress>
#include <QtConcurrent>

#include "./systeminfo.h"
#include "../sharedms/global_shared.h"


#ifdef Q_OS_WIN32
#include "HHSharedSystemUtilities/hhardwaremonitor.h"
#include "HHSharedSystemUtilities/WinUtilities"
#endif

#include "HHSharedSystemUtilities/SystemUtilities"



namespace HEHUI {


//bool SystemInfo::running = false;

SystemInfo::SystemInfo(QObject *parent) :
    QObject(parent)
{
    //running = true;

    m_getRealTimeResourcesLoad = false;

}

SystemInfo::~SystemInfo() {
    qDebug()<<"SystemInfo::~SystemInfo()";

    //running = false;
}

//bool SystemInfo::isRunning(){
//    return running;
//}


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
    QString computerName = QHostInfo::localHostName().toLower();
    obj["ComputerName"] = computerName;

#ifdef Q_OS_WIN32
    HardwareMonitor hwm;
    hwm.getOSInfo(&obj);

    bool isJoinedToDomain = false;
    QString joinInfo = WinUtilities::getJoinInformation(&isJoinedToDomain);
    if(isJoinedToDomain){
        WinUtilities::getComputerNameInfo(&joinInfo, 0, 0);
    }
    obj["Workgroup"] = joinInfo;
    obj["JoinedToDomain"] = QString::number(isJoinedToDomain?1:0);


    QStringList users = WinUtilities::localCreatedUsers();
    WinUtilities::getAllUsersLoggedOn(&users);
    users.removeDuplicates();
    obj["Users"] = users.join(";");

    QStringList admins = WinUtilities::getMembersOfLocalGroup("Administrators");
    QString admisStr = admins.join(";").toLower();
    admisStr = admisStr.replace(computerName + "\\", "");
    obj["Admins"] = admisStr;




#else


#endif

//    QStringList ipAddresses;
//    foreach (QNetworkInterface nic, QNetworkInterface::allInterfaces()) {
//        foreach (QNetworkAddressEntry entry, nic.addressEntries()) {
//            qDebug()<<"entry.ip():"<<entry.ip().toString();
//            QHostAddress broadcastAddress = entry.broadcast();
//            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
//                ipAddresses << entry.ip().toString() << "/" << nic.hardwareAddress();
//            }
//        }
//    }
//    obj["IPInfo"] = ipAddresses.join(";");


    bool ok = false, readable = true, writeable = true;
    MS::USBSTORStatus status = MS::USBSTOR_ReadWrite;
    ok = WinUtilities::readUSBStorageDeviceSettings(&readable, &writeable);
    if(readable && writeable){
        status = MS::USBSTOR_ReadWrite;
    }else if(!readable){
        status = MS::USBSTOR_Disabled;
    }else{
        status = MS::USBSTOR_ReadOnly;
    }
    obj["USBSD"] = QString::number(quint8(status));




    QJsonObject object;
    object["OS"] = obj;
    QJsonDocument doc(object);
    //emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), socketID);

    return doc.toJson(QJsonDocument::Compact);
}

void SystemInfo::getHardwareInfo(SOCKETID socketID){
    qDebug()<<"--SystemInfo::getHardwareInfo(...)";

    QJsonObject obj;
    obj["ComputerName"] = QHostInfo::localHostName().toLower();

#ifdef Q_OS_WIN32
    HardwareMonitor hwm;

    hwm.getBaseBoardInfo(&obj);
    hwm.getProcessorInfo(&obj);
    hwm.getPhysicalMemoryInfo(&obj);
    hwm.getDiskDriveInfo(&obj);
    hwm.getVideoControllerInfo(&obj);
    hwm.getSoundDeviceInfo(&obj);
    hwm.getMonitorInfo(&obj);
    hwm.getNetworkAdapterInfo(&obj);

#else


#endif

    QJsonObject object;
    object["Hardware"] = obj;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact),  MS::SYSINFO_HARDWARE, socketID);

}

void SystemInfo::getInstalledSoftwaresInfo(SOCKETID socketID){

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
    getInstalledSoftwaresInfo(&infoArray, keys32, false);
    getInstalledSoftwaresInfo(&infoArray, keys64, true);

    QJsonObject object;
    object["Software"] = infoArray;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), MS::SYSINFO_SOFTWARE, socketID);

}

void SystemInfo::getInstalledSoftwaresInfo(QJsonArray *infoArray, const QStringList &keys, bool on64BitView){

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

void SystemInfo::getUsersInfo(SOCKETID socketID){
    qDebug()<<"--SystemInfo::getUsersInfo(...)";

    QJsonArray infoArray;
    HEHUI::WinUtilities::getAllUsersInfo(&infoArray);

    QJsonObject object;
    object["Users"] = infoArray;
    QJsonDocument doc(object);
    emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), MS::SYSINFO_OSUSERS, socketID);

}

void SystemInfo::startGetingRealTimeResourcesLoad(SOCKETID socketID){
    qDebug()<<"SystemInfo::startGetingRealTimeResourcesLoad(...)";

    if(m_getRealTimeResourcesLoad){return;}
    m_getRealTimeResourcesLoad = true;

    QThreadPool * pool = QThreadPool::globalInstance();
    int maxThreadCount = pool->maxThreadCount();
    if(pool->activeThreadCount() == pool->maxThreadCount()){
        pool->setMaxThreadCount(++maxThreadCount);
    }
    QtConcurrent::run(this, &SystemInfo::getRealTimeResourcseLoad, socketID);

}

void SystemInfo::stopGetingRealTimeResourcesLoad(){
    m_getRealTimeResourcesLoad = false;
}

void SystemInfo::getRealTimeResourcseLoad(SOCKETID socketID){
    qDebug()<<"--SystemInfo::getRealTimeResourcseLoad(...)";

    while (m_getRealTimeResourcesLoad) {
        int cpuLoad = SystemUtilities::getCPULoad();
        int memLoad = 0;
        SystemUtilities::getMemoryStatus(0, &memLoad);
        //qDebug()<<"CPU:"<<cpuLoad<<" Memory:"<<memLoad;

        QJsonObject obj;
        obj["CPULoad"] = QString::number(cpuLoad);
        obj["MemLoad"] = QString::number(memLoad);


        QJsonObject object;
        object["ResourcsesLoad"] = obj;
        QJsonDocument doc(object);
        emit signalSystemInfoResultReady(doc.toJson(QJsonDocument::Compact), MS::SYSINFO_REALTIME_INFO, socketID);
    }

}









} //namespace HEHUI
