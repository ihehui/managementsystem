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

ClientInfo::ClientInfo(const QString &computerName, QObject *parent)
    :QObject(parent)
{
    this->computerName = computerName;
    workgroup = "";
    network = "";
    users = "";
    os = "";
    usbSDStatus = MS::USBSTOR_Unknown;
    programsEnabled = false;
    administrators = "";
    lastOnlineTime = QDateTime();
    clientVersion = "";

    summaryInfoSavedTODatabase = false;
    updateOSInfoStatement = "";



    installationDate = "";
    osKey = "";
    cpu = "";
    memory = "";
    motherboardName = "";
    video = "";
    monitor = "";
    audio = "";
    storage = "";
    m_isJoinedToDomain = false;

    updateHardwareInfoStatement = "";
    detailedInfoSavedTODatabase = false;

    //installedSoftwaresInfo.clear();
    installedSoftwaresCount = 0;
    updateInstalledSoftwaresInfoStatement = "";
    installedSoftwaresInfoSavedTODatabase = false;


    lastHeartbeatTime = QDateTime();

    online = false;

    clientUDTListeningAddress = "";
    clientUDTListeningPort = 0;

    onlineUsers = "";

}

ClientInfo::~ClientInfo() {
    // TODO Auto-generated destructor stub
}

ClientInfo & ClientInfo::operator = (const ClientInfo &clientInfo){

    computerName = clientInfo.getComputerName();
    workgroup = clientInfo.getWorkgroup();
    network = clientInfo.getNetwork();
    users = clientInfo.getUsers();
    os = clientInfo.getOs();
    usbSDStatus = clientInfo.getUsbSDStatus();
    programsEnabled = clientInfo.getProgramsEnabled();
    administrators = clientInfo.getAdministrators();
    lastOnlineTime = clientInfo.getLastOnlineTime();
    ipInfo = clientInfo.getIPInfo();
    clientVersion = clientInfo.getClientVersion();

    updateOSInfoStatement = clientInfo.getUpdateOSInfoStatement();
    summaryInfoSavedTODatabase = clientInfo.getSummaryInfoSavedTODatabase();


    installationDate = clientInfo.getInstallationDate();
    osKey = clientInfo.getOsKey();

    cpu = clientInfo.getCpu();
    memory = clientInfo.getMemory();
    motherboardName = clientInfo.getMotherboardName();

    video = clientInfo.getVideo();
    monitor = clientInfo.getMonitor();
    audio = clientInfo.getAudio();
    storage = clientInfo.getStorage();

    m_isJoinedToDomain = clientInfo.isJoinedToDomain();


    updateHardwareInfoStatement = clientInfo.getUpdateHardwareInfoStatement();
    detailedInfoSavedTODatabase = clientInfo.getDetailedInfoSavedTODatabase();



    installedSoftwaresCount = clientInfo.getinstalledSoftwaresCount();
    updateInstalledSoftwaresInfoStatement = clientInfo.getUpdateInstalledSoftwaresInfoStatement();
    installedSoftwaresInfoSavedTODatabase = clientInfo.isInstalledSoftwaresInfoSavedTODatabase();


    lastHeartbeatTime = clientInfo.getLastHeartbeatTime();

    online = clientInfo.getOnline();

    clientUDTListeningAddress = clientInfo.getClientUDTListeningAddress();
    clientUDTListeningPort = clientInfo.getClientUDTListeningPort();

    onlineUsers = clientInfo.getOnlineUsers();



    return *this;
}

bool ClientInfo::isValid(){
    return computerName.trimmed().size();

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
        computerName = osObj["ComputerName"].toString();
        os = osObj["OS"].toString();
        installationDate = osObj["InstallDate"].toString();
        osKey = osObj["Key"].toString();
        workgroup = osObj["Workgroup"].toString();
        m_isJoinedToDomain = osObj["JoinedToDomain"].toBool();
        users = osObj["Users"].toString();
        administrators = osObj["Admins"].toString();
        ipInfo = osObj["IPInfo"].toString();
        clientVersion = osObj["Version"].toString();
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

}

QByteArray ClientInfo::getJsonData() const{

    QJsonObject osObj;
    osObj["ComputerName"] = computerName;
    osObj["OS"] = os;
    osObj["InstallDate"] = installationDate;
    osObj["Key"] = osKey;
    osObj["Workgroup"] = workgroup;
    osObj["JoinedToDomain"] = m_isJoinedToDomain?"1":"0";
    osObj["Users"] = users;
    osObj["Admins"] = administrators;
    osObj["IPInfo"] = ipInfo;
    osObj["Version"] = clientVersion;

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
    object["OS"] = osObj;
    object["Hardware"] = hwObj;

    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Compact);

}
















} //namespace HEHUI
