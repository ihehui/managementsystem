/*
 * clientinfo.cpp
 *
 *  Created on: 2010-9-27
 *      Author: HeHui
 */


#include <QHostAddress>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "clientinfo.h"


namespace HEHUI {

ClientInfo::ClientInfo(const QString &assetNO, QObject *parent)
    :QObject(parent)
{

    this->assetNO = assetNO;
    computerName = "";
    osVersion = "";
    installationDate = "";
    osKey = "";
    workgroup = "";
    m_isJoinedToDomain = false;
    users = "";
    administrators = "";
    ip = "";
    clientVersion = "";
    processMonitorEnabled = false;
    lastOnlineTime = QDateTime();

    cpu = "";
    memory = "";
    motherboardName = "";
    video = "";
    monitor = "";
    audio = "";
    storage = "";
    network = "";
    usbSDStatus = MS::USBSTOR_Unknown;

    cpuLoad = 0;
    memoryLoad = 0;



    online = false;

    clientUDTListeningAddress = "";
    clientUDTListeningPort = 0;

    onlineUsers = "";

}

ClientInfo::~ClientInfo() {
    // TODO Auto-generated destructor stub
}

ClientInfo & ClientInfo::operator = (const ClientInfo &clientInfo){

    assetNO = clientInfo.getAssetNO();
    computerName = clientInfo.getComputerName();
    osVersion = clientInfo.getOSVersion();
    installationDate = clientInfo.getInstallationDate();
    osKey = clientInfo.getOsKey();
    workgroup = clientInfo.getWorkgroup();
    m_isJoinedToDomain = clientInfo.isJoinedToDomain();
    users = clientInfo.getUsers();
    administrators = clientInfo.getAdministrators();
    ip = clientInfo.getIP();
    clientVersion = clientInfo.getClientVersion();
    processMonitorEnabled = clientInfo.isProcessMonitorEnabled();
    lastOnlineTime = clientInfo.getLastOnlineTime();


    cpu = clientInfo.getCpu();
    memory = clientInfo.getMemory();
    motherboardName = clientInfo.getMotherboardName();
    video = clientInfo.getVideo();
    monitor = clientInfo.getMonitor();
    audio = clientInfo.getAudio();
    storage = clientInfo.getStorage();
    network = clientInfo.getNetwork();
    usbSDStatus = clientInfo.getUsbSDStatus();

    //cpu = clientInfo.getCPULoad();
    //memoryLoad = clientInfo.getMemoryLoad();

    online = clientInfo.isOnline();

    clientUDTListeningAddress = clientInfo.getClientUDTListeningAddress();
    clientUDTListeningPort = clientInfo.getClientUDTListeningPort();

    onlineUsers = clientInfo.getOnlineUsers();


    return *this;
}

bool ClientInfo::isValid(){
    return assetNO.trimmed().size();
}

void ClientInfo::setJsonData(const QByteArray &data){

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QJsonObject osObj = object["OS"].toObject();
    if(!osObj.isEmpty()){
        if(osObj.contains("AssetNO")){
            assetNO = osObj["AssetNO"].toString();
        }

        computerName = osObj["ComputerName"].toString();
        osVersion = osObj["OS"].toString();
        installationDate = osObj["InstallDate"].toString();
        osKey = osObj["Key"].toString();
        workgroup = osObj["Workgroup"].toString();
        m_isJoinedToDomain = osObj["JoinedToDomain"].toBool();
        users = osObj["Users"].toString();
        administrators = osObj["Admins"].toString();
        if(osObj.contains("IPInfo")){
            ip = osObj["IPInfo"].toString();
        }
        clientVersion = osObj["Version"].toString();
        processMonitorEnabled = osObj["ProcessMonitorEnabled"].toBool();
        usbSDStatus = MS::USBSTORStatus(osObj.value("USBSD").toString().toUShort());

    }

    QJsonObject hwObj = object["Hardware"].toObject();
    if(!hwObj.isEmpty()){
        cpu = hwObj["Processor"].toString();
        memory = hwObj.value("PhysicalMemory").toString();
        motherboardName = hwObj.value("BaseBoard").toString();
        video = hwObj.value("VideoController").toString();
        monitor = hwObj.value("Monitor").toString();
        audio = hwObj.value("SoundDevice").toString();
        storage = hwObj.value("DiskDrive").toString();
        network = hwObj.value("NetworkAdapter").toString();
    }

    QJsonObject resLoadObj = object["ResourcsesLoad"].toObject();
    if(!resLoadObj.isEmpty()){
        cpuLoad = resLoadObj.value("CPULoad").toString().toUInt();
        memoryLoad = resLoadObj.value("MemLoad").toString().toUInt();
    }

}

QByteArray ClientInfo::getOSJsonData() const{

    QJsonObject osObj;
    osObj["AssetNO"] = assetNO;
    osObj["ComputerName"] = computerName;
    osObj["OS"] = osVersion;
    osObj["InstallDate"] = installationDate;
    osObj["Key"] = osKey;
    osObj["Workgroup"] = workgroup;
    osObj["JoinedToDomain"] = m_isJoinedToDomain?"1":"0";
    osObj["Users"] = users;
    osObj["Admins"] = administrators;
    osObj["IPInfo"] = ip;
    osObj["Version"] = clientVersion;
    osObj["ProcessMonitorEnabled"] = processMonitorEnabled?"1":"0";
    osObj["USBSD"] = usbSDStatus;

//    QJsonObject hwObj;
//    hwObj["Processor"] = cpu;
//    hwObj["PhysicalMemory"] = memory;
//    hwObj["BaseBoard"] = motherboardName;
//    hwObj["VideoController"] =video;
//    hwObj["Monitor"] = monitor;
//    hwObj["SoundDevice"] = audio;
//    hwObj["DiskDrive"] = storage;
//    hwObj["NetworkAdapter"] = network;

    QJsonObject object;
    object["OS"] = osObj;
//    object["Hardware"] = hwObj;

    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Compact);

}

QByteArray ClientInfo::getHardwareJsonData() const{

//    QJsonObject osObj;
//    osObj["AssetNO"] = assetNO;
//    osObj["ComputerName"] = computerName;
//    osObj["OS"] = osVersion;
//    osObj["InstallDate"] = installationDate;
//    osObj["Key"] = osKey;
//    osObj["Workgroup"] = workgroup;
//    osObj["JoinedToDomain"] = m_isJoinedToDomain?"1":"0";
//    osObj["Users"] = users;
//    osObj["Admins"] = administrators;
//    //osObj["IPInfo"] = ip;
//    osObj["Version"] = clientVersion;
//    osObj["ProcessMonitorEnabled"] = processMonitorEnabled?"1":"0";


    QJsonObject hwObj;
    hwObj["Processor"] = cpu;
    hwObj["PhysicalMemory"] = memory;
    hwObj["BaseBoard"] = motherboardName;
    hwObj["VideoController"] =video;
    hwObj["Monitor"] = monitor;
    hwObj["SoundDevice"] = audio;
    hwObj["DiskDrive"] = storage;
    hwObj["NetworkAdapter"] = network;

    QJsonObject object;
    //object["OS"] = osObj;
    object["Hardware"] = hwObj;

    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Compact);

}


void ClientInfo::setAssetNO(const QString &assetNO){
    this->assetNO = assetNO;
}

QString ClientInfo::getAssetNO() const{
    return assetNO;
}
















} //namespace HEHUI
