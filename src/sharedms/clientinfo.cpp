/*
 * clientinfo.cpp
 *
 *  Created on: 2010-9-27
 *      Author: HeHui
 */


#include <QHostAddress>

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
    updateSummaryInfoStatement = "";



    installationDate = "";
    windowsDir = "";
    osKey = "";
    cpu = "";
    memory = "";
    motherboardName = "";
    dmiUUID = "";
    chipset = "";
    video = "";
    monitor = "";
    audio = "";
    storage = "";
    nic1Info = "";
    nic2Info = "";
    m_isJoinedToDomain = false;

    updateDetailedInfoStatement = "";
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
    clientVersion = clientInfo.getClientVersion();

    updateSummaryInfoStatement = clientInfo.getUpdateSummaryInfoStatement();
    summaryInfoSavedTODatabase = clientInfo.getSummaryInfoSavedTODatabase();


    installationDate = clientInfo.getInstallationDate();
    windowsDir = clientInfo.getWindowsDir();
    osKey = clientInfo.getOsKey();

    cpu = clientInfo.getCpu();
    memory = clientInfo.getMemory();
    motherboardName = clientInfo.getMotherboardName();
    dmiUUID = clientInfo.getDmiUUID();
    chipset = clientInfo.getChipset();
    video = clientInfo.getVideo();
    monitor = clientInfo.getMonitor();
    audio = clientInfo.getAudio();
    storage = clientInfo.getStorage();

    nic1Info = clientInfo.getNic1Info();
    nic2Info = clientInfo.getNic2Info();

    m_isJoinedToDomain = clientInfo.isJoinedToDomain();


    updateDetailedInfoStatement = clientInfo.getUpdateDetailedInfoStatement();
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
















} //namespace HEHUI
