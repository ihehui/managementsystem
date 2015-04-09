/*
 ****************************************************************************
 * serverservice.cpp
 *
 * Created on: 2010-7-12
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-9-3
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QtCore>
#include "serverservice.h"

#include "../app_constants.h"
#include "../settings.h"





namespace HEHUI {


ServerService::ServerService(int argc, char **argv, const QString &serviceName, const QString &description )
    :Service(argc, argv, serviceName, description)
{


    setStartupType(QtServiceController::AutoStartup);
    //    setServiceFlags(CanBeSuspended);

    //    networkManager = NetworkManager::instance();
    //    serverPacketsParser = 0;
    //    databaseUtility = new DatabaseUtility(this);
    //    mainServiceStarted = false;

    resourcesManager = 0;
    serverPacketsParser = 0;

    m_udpServer = 0;
    m_rtp = 0;
//    m_udtProtocol = 0;

    databaseUtility = 0;
    query = 0;
    mainServiceStarted = false;

    clientInfoHash.clear();

    onlineAdminsCount = 0;



}

ServerService::~ServerService(){
    qDebug()<<"ServerService::~ServerService()";

    if(resourcesManager){
        //resourcesManager->closeAllServers();
    }

    if(serverPacketsParser){
        delete serverPacketsParser;
        serverPacketsParser = 0;
    }

//    ResourcesManagerInstance::cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

//    delete m_udpServer;
//    delete m_udtProtocol;

    PacketHandlerBase::clean();


    QList<ClientInfo*> clientInfoList = clientInfoHash.values();
    clientInfoHash.clear();
    foreach(ClientInfo *info, clientInfoList){
        delete info;
        info = 0;
    }

    delete databaseUtility;
    databaseUtility = 0;
    delete query;
    query = 0;

    mainServiceStarted = false;


}

bool ServerService::startMainService(){
    qDebug()<<"----ServerService::startMainService()";

    if(mainServiceStarted){
        qWarning()<<"Main service has already started!";
        return true;
    }


    if(openDatabase()){
        getRecordsInDatabase();
    }


    QString errorMessage = "";
//    m_udpServer = resourcesManager->startIPMCServer(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), &errorMessage);
//    if(!m_udpServer){
//        logMessage(QString("Can not start IP Multicast listening on address '%1', port %2! %3").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
//        m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
//    }else{
//        qWarning()<<QString("IP Multicast listening on address '%1', port %2!").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT);
//    }

    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
    if(!m_udpServer){
        logMessage(QString("Can not start UDP listening on port %1! %2").arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(IP_MULTICAST_GROUP_PORT);
    }


    m_rtp = resourcesManager->startRTP(QHostAddress::Any, UDT_LISTENING_PORT, true, &errorMessage);
    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)), Qt::QueuedConnection);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    m_udtProtocol->startWaitingForIOInOneThread(30);
    //m_udtProtocol->startWaitingForIOInSeparateThread(10, 500);



    serverPacketsParser = new ServerPacketsParser(resourcesManager, this);
    //connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));
    //connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));

    connect(serverPacketsParser, SIGNAL(signalClientLogReceived(const QString&, const QString&, quint8, const QString&, const QString&)), this, SLOT(saveClientLog(const QString&, const QString&, quint8, const QString&, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientInfoPacketReceived(const QString &, const QByteArray &, quint8)), this, SLOT(clientInfoPacketReceived(const QString &, const QByteArray &, quint8)));

    connect(serverPacketsParser, SIGNAL(signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID, const QByteArray &, bool, bool, bool, bool, bool)), this, SLOT(processRequestChangeProcessMonitorInfoPacket(SOCKETID, const QByteArray &, bool, bool, bool, bool, bool)), Qt::QueuedConnection);


//    connect(serverPacketsParser, SIGNAL(signalHeartbeatPacketReceived(const QString &, const QString&)), this, SLOT(processHeartbeatPacket(const QString &, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientOnlineStatusChanged(SOCKETID, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(SOCKETID, const QString&, bool)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalAdminOnlineStatusChanged(SOCKETID, const QString&, const QString&, bool)), this, SLOT(processAdminOnlineStatusChangedPacket(SOCKETID, const QString&, const QString&, bool)), Qt::QueuedConnection);

    //Single Process Thread
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::run);

    //IMPORTANT For Multi-thread
    //QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startparseIncomingPackets);
    //QtConcurrent::run(serverPacketsParser, &ServerPacketsParser::startprocessOutgoingPackets);


    //sendServerOnlinePacket();

    //serverPacketsParser->sendServerDeclarePacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());
    //serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT), networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), networkManager->hostName());
    sendServerOnlinePacket();

    sendServerOnlinePacketTimer = new QTimer(this);
    sendServerOnlinePacketTimer->setSingleShot(false);
    sendServerOnlinePacketTimer->setInterval(15*60000);
    connect(sendServerOnlinePacketTimer, SIGNAL(timeout()), this, SLOT(sendServerOnlinePacket()));
    sendServerOnlinePacketTimer->start();

    mainServiceStarted = true;

    return true;
}

void ServerService::saveClientLog(const QString &computerName, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime){
    //qWarning()<<"ServerService::saveClientLog(...) IP:"<<clientAddress<<" log:"<<log;

    if(!query){
        if(!openDatabase()){
            return;
        }
    }else{
        query->clear();
    }

    query->prepare("INSERT INTO logs (ComputerName, Users, IPAddress, Type, Content, ClientTime) "
                   "VALUES (:ComputerName, :Users, :IPAddress, :Type, :Content, :ClientTime)");

    query->bindValue(":ComputerName", computerName);
    query->bindValue(":Users", "");
    query->bindValue(":IPAddress", clientAddress);
    query->bindValue(":Type", logType);
    query->bindValue(":Content", log);
    query->bindValue(":ClientTime", clientTime);

    if(!query->exec()){
        QSqlError error = query->lastError();
        QString msg = QString("Can not write log to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        logMessage(msg, QtServiceBase::Error);
        qCritical()<< msg;

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query->clear();
            openDatabase(true);
        }

    }

    query->clear();

}

void ServerService::sendServerOnlinePacket(){
    qDebug()<<"----ServerService::sendServerOnlinePacket()";

    serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT));
    serverPacketsParser->sendServerDeclarePacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT));

    //serverPacketsParser->sendServerOnlinePacket();
    
    updateOrSaveAllClientsInfoToDatabase();

}

bool ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info){
    //    qWarning()<<"ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info)";

    if(!info){
        return false;
    }

    QString computerName = info->getComputerName();

    QString summaryStatement = "";
    QString detailedStatement = "";
    QString updateInstalledSoftwaresInfoStatement = "";

    //    if(isRecordExistInDB(info->getComputerName())){
    if(!info->getSummaryInfoSavedTODatabase()){
        summaryStatement = info->getUpdateOSInfoStatement();
    }
    if(!info->getDetailedInfoSavedTODatabase()){
        detailedStatement = info->getUpdateHardwareInfoStatement();
        //qWarning()<<"detailedStatement:"<<detailedStatement;
    }
    if(!info->isInstalledSoftwaresInfoSavedTODatabase()){
        updateInstalledSoftwaresInfoStatement = info->getUpdateInstalledSoftwaresInfoStatement();
    }


    //    }
    //    else{
    //        summaryStatement = "START TRANSACTION;";
    //        //statement += QString("delete from clientinfo where ComputerName = '%1'; ").arg(computerName);
    //        summaryStatement += QString("INSERT INTO summaryinfo (ComputerName, Workgroup, Network, Users, OS, USBSD, Programes, Administrators, ClientVersion) "
    //                            "VALUES ('%1', '%2', '%3', '%4', '%5', %6, %7, '%8', '%9'); ")
    //                .arg(computerName).arg(info->getJoinInformation()).arg(info->getNetwork()).arg(info->getUsers()).arg(info->getOs())
    //                .arg(QVariant(info->getUsbSDEnabled()).toUInt()).arg(QVariant(info->getProgramsEnabled()).toUInt())
    //                .arg(info->getAdministrators()).arg(info->getClientVersion());

    //        summaryStatement += QString("INSERT INTO detailedinfo (ComputerName) "
    //                            "VALUES ('%1'); ").arg(computerName);

    //        summaryStatement += "COMMIT;";

    //    }


    if(!query){
        if(!openDatabase()){
            return false;
        }
    }else{
        query->clear();
    }

    if(!summaryStatement.trimmed().isEmpty()){
        if(!query->exec(summaryStatement)){
            QSqlError error = query->lastError();
            QString msg = QString("Can not write client summary info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;
            qCritical()<<"summaryStatement:";
            qCritical()<<summaryStatement;
            qCritical()<<"";
            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Summary Statement For "<<computerName<<": "<<summaryStatement;

        info->setOSInfoSavedTODatabase(true);
        info->setUpdateOSInfoStatement("");
        qWarning()<<"Client summary info from "<<computerName<<" has been saved!";

        query->clear();
    }

    if(!detailedStatement.trimmed().isEmpty()){
        if(!query->exec(detailedStatement)){
            QSqlError error = query->lastError();
            QString msg = QString("Can not write client detailed info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;

            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Detailed Statement For "<<computerName<<": "<<detailedStatement;

        info->setHardwareInfoSavedTODatabase(true);
        info->setUpdateHardwareInfoStatement("");
        qWarning()<<"Client detailed info from "<<computerName<<" has been saved!";

        query->clear();

    }

    if(!updateInstalledSoftwaresInfoStatement.trimmed().isEmpty()){
        query->exec(updateInstalledSoftwaresInfoStatement);
        QSqlError error = query->lastError();
        if(error.type() != QSqlError::NoError){
            //QSqlError error = query->lastError();
            QString msg = QString("Can not write client installed softwares info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
            logMessage(msg, QtServiceBase::Error);
            qCritical()<<msg;

            //MySQL数据库重启，重新连接
            if(error.number() == 2006){
                query->clear();
                openDatabase(true);
            }else{
                getRecordsInDatabase();
            }

            return false;
        }
//        qWarning()<<" Installed Softwares Statement For "<<computerName<<": "<<updateInstalledSoftwaresInfoStatement;

        info->setInstalledSoftwaresInfoSavedTODatabase(true);
        info->setUpdateInstalledSoftwaresInfoStatement("");
        qWarning()<<"Client installed softwares info from "<<computerName<<" has been saved!";

        query->clear();

    }



    if(!isRecordExistInDB(computerName)){
        recordsInDatabase.append(computerName.toLower());
    }

    return true;

}

void ServerService::updateOrSaveAllClientsInfoToDatabase(){

    getRecordsInDatabase();

    foreach(ClientInfo *info, clientInfoHash.values()){
        updateOrSaveClientInfoToDatabase(info);
    }

}

void ServerService::clientInfoPacketReceived(const QString &computerName, const QByteArray &clientInfo, quint8 infoType){

    qWarning()<<"Client detailed info has been received From "<< computerName;
    //    qWarning()<<"-----clientDetailedInfoPacketReceived";

    if(computerName.trimmed().isEmpty()){
        qCritical()<<"Invalid Computer Name!";
        return;
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(computerName)){
        info = clientInfoHash.value(computerName);
    }else{
        info = new ClientInfo(computerName, this);
        clientInfoHash.insert(computerName, info);
    }

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
        processOSInfo(info, clientInfo);
          break;
    case quint8(MS::SYSINFO_HARDWARE):
        processHardwareInfo(info, clientInfo);
        break;

    case quint8(MS::SYSINFO_SOFTWARE):
        processSoftwareInfo(info, clientInfo);
        break;
//    case quint8(MS::SYSINFO_SERVICES):
//        updateServicesInfo(object);
//        break;
    default:
        break;
    }


}

void ServerService::processOSInfo(ClientInfo *info, const QByteArray &osData){
    if(!info){
        return;
    }

    QString osInfo = info->getOs();
    QString installationDate = info->getInstallationDate();
    QString osKey = info->getOsKey();
    QString workgroupName = info->getWorkgroup();
    bool isJoinedToDomain = info->isJoinedToDomain();
    QString usersInfo = info->getUsers();
    QString admins = info->getAdministrators();
    QString ipInfo = info->getIPInfo();
    QString clientVersion = info->getClientVersion();

    info->setJsonData(osData);
    QString computerName = info->getComputerName();

    QString statement;
    if(isRecordExistInDB(computerName)){
        qDebug()<<"Client Info Exists!";
        //info = clientInfoHash.value(computerName);
        statement = "UPDATE os SET LastOnlineTime = NULL ";

        QString newOSInfo = info->getOs();
        if(osInfo != newOSInfo){
            statement += QString(", OS = '%1' ").arg(newOSInfo);
        }

        QString newinstallationDate = info->getNetwork();
        if(installationDate != newinstallationDate){
            statement += QString(", InstallationDate = '%1' ").arg(newinstallationDate);
        }

        QString newOSKey = info->getOsKey();
        if(osKey != newOSKey){
            statement += QString(", OSKey = '%1' ").arg(newOSKey);
        }

        QString newworkgroupName = info->getWorkgroup();
        if(workgroupName != newworkgroupName){
            statement += QString(", Workgroup = '%1' ").arg(newworkgroupName);
        }

        bool newJoinedToDomain = info->isJoinedToDomain();
        if(isJoinedToDomain != newJoinedToDomain){
            statement += QString(", JoinedToDomain = %1 ").arg(newJoinedToDomain?1:0);
        }

        QString newUsers = info->getUsers();
        if(usersInfo != newUsers){
            QString tempUsersInfo = newUsers;
            tempUsersInfo.replace("\\", "\\\\");
            statement += QString(", Users = '%1' ").arg(tempUsersInfo);
        }

        QString newadmins = info->getAdministrators();
        if(admins != newadmins){
            QString tempAdminsInfo = newadmins;
            tempAdminsInfo.replace("\\", "\\\\");
            statement += QString(", Administrators = '%1' ").arg(tempAdminsInfo);
        }

        QString newipInfo = info->getIPInfo();
        if(ipInfo != newipInfo){
            statement += QString(", IP = '%1' ").arg(newipInfo);
        }

        QString newclientVersion = info->getClientVersion();
        if(clientVersion != newclientVersion){
            statement += QString(", ClientVersion = '%1' ").arg(newclientVersion);
        }

        statement += QString("WHERE ComputerName = '%1'").arg(computerName);
        //qWarning()<<"Update Client Info For Computer "<<computerName << " "<<networkInfo;
        //qWarning()<<statement;

    }else{
        qDebug()<<"Client OS Info Not Exists!";

        statement = "START TRANSACTION;";
        statement += QString("INSERT INTO os (ComputerName, OS, InstallationDate, OSKey, Workgroup, JoinedToDomain, Users, Administrators, IP, ClientVersion) "
                             "VALUES ('%1', '%2', '%3', '%4', '%5', %6, '%7', '%8', '%9', '%10' ); ")
                .arg(computerName).arg(osInfo).arg(installationDate).arg(osKey).arg(workgroupName)
                .arg(QVariant(isJoinedToDomain).toUInt()).arg(usersInfo).arg(admins).arg(ipInfo).arg(clientVersion);

        statement += QString("INSERT INTO hardware (ComputerName) "
                             "VALUES ('%1'); ").arg(computerName);

        statement += "COMMIT;";
    }


    info->setOSInfoSavedTODatabase(false);
    info->setUpdateOSInfoStatement(statement);

    if(onlineAdminsCount > 0){
        if(updateOrSaveClientInfoToDatabase(info)){
            info->setLastHeartbeatTime(QDateTime::currentDateTime());
        }
    }

    qWarning();
    qWarning()<<QString(" '%1' Exists In:  Memory:%2  DB:%3 Version:%4").arg(computerName).arg(clientInfoHash.contains(computerName)?"YES":"NO").arg(isRecordExistInDB(computerName)?"YES":"NO").arg(clientVersion);



}

void ServerService::processHardwareInfo(ClientInfo *info, const QByteArray &hardwareData){

    if(!info){
        return;
    }

    QString cpu = info->getCpu();
    QString memory = info->getMemory();
    QString motherboardName = info->getMotherboardName();
    QString video = info->getVideo();
    QString monitor = info->getMonitor();
    QString audio = info->getAudio();
    QString storage = info->getStorage();
    QString network = info->getNetwork();

    info->setJsonData(hardwareData);

    QStringList changes;

    QString statement;
    //    if(isRecordExistInDB(computerName)){
    statement = "UPDATE hardware SET UpdateTime = NULL ";

    QString newCPU = info->getCpu();
    if(cpu != newCPU){
        statement += QString(", CPU = '%1' ").arg(newCPU);
        if(!cpu.isEmpty()){
            changes.append(QString("CPU Changed from '%1' to '%2'").arg(cpu).arg(newCPU));
        }
    }

    QString newMemory = info->getMemory();
    if(memory != newMemory){
        statement += QString(", Memory = '%1' ").arg(newMemory);
        if(!memory.isEmpty()){
            changes.append(QString("Memory Changed from '%1' to '%2'").arg(memory).arg(newCPU));
        }
    }

    QString newMotherboardName = info->getMotherboardName();
    if(motherboardName != newMotherboardName){
        statement += QString(", Motherboard = '%1' ").arg(newMotherboardName);
        if(!motherboardName.isEmpty()){
            changes.append(QString("Motherboard Changed from '%1' to '%2'").arg(motherboardName).arg(newMotherboardName));
        }
    }

    QString newVideo = info->getVideo();
    if(video != newVideo){
        statement += QString(", Video = '%1' ").arg(newVideo);
        if(!video.isEmpty()){
            changes.append(QString("Video Changed from '%1' to '%2'").arg(video).arg(newVideo));
        }
    }

    QString newMonitor = info->getMonitor();
    if(monitor != newMonitor){
        statement += QString(", Monitor = '%1' ").arg(newMonitor);
        if(!monitor.isEmpty()){
            changes.append(QString("Monitor Changed from '%1' to '%2'").arg(monitor).arg(newMonitor));
        }
    }

    QString newAudio = info->getAudio();
    if(audio != newAudio){
        statement += QString(", Audio = '%1' ").arg(newAudio);
        if(!audio.isEmpty()){
            changes.append(QString("Audio Changed from '%1' to '%2'").arg(audio).arg(newAudio));
        }
    }

    QString newStorage = info->getStorage();
    if(storage != newStorage){
        statement += QString(", Storage = '%1' ").arg(newStorage);
        if(!storage.isEmpty()){
            changes.append(QString("Storage Changed from '%1' to '%2'").arg(storage).arg(newStorage));
        }
    }

    QString newNIC = info->getNetwork();
    if(network != newNIC){
        statement += QString(", NIC = '%1' ").arg(newNIC);
        if(!network.isEmpty()){
            changes.append(QString("NIC Changed from '%1' to '%2'").arg(network).arg(newNIC));
        }
    }


    QString computerName = info->getComputerName();

    statement += QString("WHERE ComputerName = '%1'").arg(computerName);

    info->setHardwareInfoSavedTODatabase(false);
    info->setUpdateHardwareInfoStatement(statement);


    if(changes.isEmpty()){return;}

    QString alarmStatement ;
    foreach (QString change, changes) {
        alarmStatement += QString("INSERT INTO alarm(ComputerName, Type, Message) VALUES('%1', %2, '%3' ); ").arg(computerName).arg(1).arg(change);
    }
    info->setUpdateAlarmsInfoStatement(alarmStatement);


    qCritical()<<"!!!!!!!!!!!!!Hardware Changed!!!!!!!!!!!!!"<<changes.join("\n");
    qCritical()<<"Computer:"<<info->getComputerName();
    qCritical()<<changes.join("\n");



}

void ServerService::processSoftwareInfo(ClientInfo *info, const QByteArray &data){

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){return;}

    QJsonArray array = object["Software"].toArray();
//    if(array.isEmpty()){
//        return;
//    }

    int softwareCount = array.size();

    if(softwareCount != info->getinstalledSoftwaresCount()){
        QString updateInstalledSoftwaresInfoStatement = QString("START TRANSACTION; delete from installedsoftware where ComputerName = '%1'; ").arg(info->getComputerName());

        for(int i=0;i<softwareCount;i++){
            QJsonArray infoArray = array.at(i).toArray();
            if(infoArray.size() != 4){continue;}

            updateInstalledSoftwaresInfoStatement += QString(" insert into installedsoftware(ComputerName, SoftwareName, SoftwareVersion, Size, InstallationDate, Publisher) values('%1', '%2', '%3', '%4', '%5'); ").arg(info->getComputerName()).arg(infoArray.at(0).toString()).arg(infoArray.at(1).toString()).arg(infoArray.at(2).toString()).arg(infoArray.at(3).toString());
        }
        updateInstalledSoftwaresInfoStatement += "COMMIT;";

        info->setInstalledSoftwaresInfoSavedTODatabase(false);
        info->setUpdateInstalledSoftwaresInfoStatement(updateInstalledSoftwaresInfoStatement);
        //qWarning()<<"------0------updateInstalledSoftwaresInfoStatement:"<<updateInstalledSoftwaresInfoStatement;

    }


}

void ServerService::processRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules){
    //TODO
}

void ServerService::getRecordsInDatabase(){

    if(!query){
        if(!openDatabase()){
            return;
        }
    }else{
        query->clear();
    }

    recordsInDatabase.clear();
    if(query->exec("select ComputerName from summaryinfo")){
        while(query->next()){
            recordsInDatabase.append(query->value(0).toString().toLower());
        }
        qWarning()<<"Records In Database:"<<recordsInDatabase.size();
    }else{
        qCritical()<<QString("Error! Can not query records from database! %1").arg(query->lastError().text());
    }
    query->clear();

}

//void ServerService::processHeartbeatPacket(const QString &clientAddress, const QString &computerName){

//    ClientInfo *info = 0;
//    if(clientInfoHash.contains(computerName)){
//        info = clientInfoHash.value(computerName);
//        info->setLastHeartbeatTime(QDateTime::currentDateTime());
//        info->setOnline(true);
//        //qWarning()<<"Heartbeat Packet From:"<<computerName;
//    }else{
//        //serverPacketsParser->sendServerRequestClientSummaryInfoPacket("", computerName, "", clientAddress);
//        qWarning()<<QString("Unknown Heartbeat Packet From Computer '%1'! IP:%2").arg(computerName).arg(clientAddress);
//    }

//}

void ServerService::processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, bool online){
    qDebug()<<"ServerService::processClientOnlineStatusChangedPacket(...)"<<" socketID:"<<socketID<<" clientName:"<<clientName<<" online:"<<online;

    QString ip = "";
    quint16 port = 0;

    if(online){

        if(!m_rtp->getAddressInfoFromSocket(socketID, &ip, &port)){
            qCritical()<<m_rtp->lastErrorString();
            return;
        }

        if(clientSocketsHash.values().contains(clientName)){
            SOCKETID preSocketID = clientSocketsHash.key(clientName);
            qDebug()<<"---------preSocketID:"<<preSocketID<<" socketID:"<<socketID;
            if(preSocketID != socketID){
                m_rtp->closeSocket(preSocketID);
                //peerDisconnected(preSocketID);
                clientSocketsHash.remove(preSocketID);
                adminSocketsHash.remove(preSocketID);
            }

//            clientSocketsHash.remove(preSocketID);
//            m_udtProtocol->closeSocket(preSocketID);
        }
        clientSocketsHash.insert(socketID, clientName);

    }else{
        clientSocketsHash.remove(socketID);
        m_rtp->closeSocket(socketID);
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(clientName)){
        info = clientInfoHash.value(clientName);
        if(!online){
            ip = info->getClientUDTListeningAddress();
            port = info->getClientUDTListeningPort();
        }
    }else{
        info =  new ClientInfo(clientName, this);
        clientInfoHash.insert(clientName, info);

//        if(online){
//            serverPacketsParser->sendServerRequestClientSummaryInfoPacket(socketID, "", clientName, "");
//        }
        //qWarning()<<QString("Unknown Client '%1' %2 ! %3").arg(clientName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        //qWarning()<<QString("Unknown Client '%1' From %2:%3 %4 ! %5").arg(clientName).arg(ip).arg(port).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    }

    info->setOnline(online);
    if(online){
        info->setClientUDTListeningAddress(ip);
        info->setClientUDTListeningPort(port);
    }

    qWarning();
    qWarning()<<QString("Client '%1' From %2:%3 %4 via %5! Time:%6 Socket: %7").arg(clientName).arg(ip).arg(port).arg(online?"Online":"Offline").arg(m_rtp->socketProtocolString(socketID)).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(socketID);
    qWarning()<<QString("Total Online Clients:%1").arg(clientSocketsHash.size());


}

void ServerService::processAdminOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, const QString &adminName, bool online){

    QString ip = "";
    quint16 port = 0;

    if(!m_rtp->getAddressInfoFromSocket(socketID, &ip, &port)){
        qCritical()<<m_rtp->lastErrorString();

        return;
    }

    qWarning()<<QString(" Admin %1@%2 %3! %4").arg(adminName).arg(clientName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    if(online){
        adminSocketsHash.insert(socketID, adminName);
        updateOrSaveAllClientsInfoToDatabase();
    }else{
        adminSocketsHash.remove(socketID);
    }

}

void ServerService::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qDebug()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ServerService::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ServerService::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }


}

void ServerService::peerDisconnected(SOCKETID socketID){
    qDebug()<<"ServerService::peerDisconnected(...)"<<" socketID:"<<socketID;
    qDebug()<<"----------3----------"<<" socketID:"<<socketID<<" Time:"<<QDateTime::currentDateTime().toString("mm:ss:zzz")<<" ThreadID:"<<QThread::currentThreadId();

//    m_rtp->closeSocket(socketID);

    if(clientSocketsHash.contains(socketID)){
        QString address = "Unknown Address";
        ClientInfo *info = clientInfoHash.value(clientSocketsHash.value(socketID));
        if(info){
            address = info->getClientUDTListeningAddress() + ":" + QString::number(info->getClientUDTListeningPort());
        }
        qCritical()<<QString("ERROR! Client '%1' From %2 Closed Connection Unexpectedly! %3").arg(clientSocketsHash.value(socketID)).arg(address).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        clientSocketsHash.remove(socketID);
    }

    if(adminSocketsHash.contains(socketID)){
        qCritical()<<QString("ERROR! Admin '%1' Closed Connection Unexpectedly! %2").arg(adminSocketsHash.value(socketID)).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        adminSocketsHash.remove(socketID);
    }


}



bool ServerService::openDatabase(bool reopen){

    if(reopen){
        if(query){
            query->clear();
            delete query;
            query = 0;
        }
        databaseUtility->closeDBConnection(SERVERSERVICE_DB_CONNECTION_NAME);
    }

    //    Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME);
    //    qWarning()<<settings.getDBType();
    //    qWarning()<<settings.getDBDriver();
    //    qWarning()<<settings.getDBServerHost();
    //    qWarning()<<settings.getDBServerPort();
    //    qWarning()<<settings.getDBServerUserName();
    //    qWarning()<<settings.getDBServerUserPassword();
    //    qWarning()<<settings.getDBName();



    QSqlDatabase db = QSqlDatabase::database(SERVERSERVICE_DB_CONNECTION_NAME);
    if(!db.isValid()){
        QSqlError err;
        //        err = databaseUtility->openDatabase(MYSQL_DB_CONNECTION_NAME,
        //                                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
        //                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
        //                                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
        //                                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
        //                                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
        //                                            REMOTE_SITOY_COMPUTERS_DB_NAME,
        //                                            HEHUI::MYSQL);
        HEHUI::Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME, "./");
        err = databaseUtility->openDatabase(SERVERSERVICE_DB_CONNECTION_NAME,
                                            settings.getDBDriver(),
                                            settings.getDBServerHost(),
                                            settings.getDBServerPort(),
                                            settings.getDBServerUserName(),
                                            settings.getDBServerUserPassword(),
                                            settings.getDBName(),
                                            settings.getDBType());
        if (err.type() != QSqlError::NoError) {
            logMessage(QString("An error occurred when opening the database on '%1'! %2").arg(REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).arg(err.text()), QtServiceBase::Error);
            qCritical() << QString("XX An error occurred when opening the database: %1").arg(err.text());
            return false;
        }

    }

    db = QSqlDatabase::database(SERVERSERVICE_DB_CONNECTION_NAME);
    if(!db.isOpen()){
        qCritical()<<QString("Database is not open! %1").arg(db.lastError().text());
        return false;
    }

    if(!query){
        query = new QSqlQuery(db);

        //        recordsInDatabase.clear();
        //        if(query->exec("select ComputerName from clientinfo")){
        //            while(query->next()){
        //                recordsInDatabase.append(query->value(0).toString());
        //            }
        //            qWarning()<<"Records In Database:"<<recordsInDatabase.size();
        //        }
        //        query->clear();

    }


    return true;



}

bool ServerService::isRecordExistInDB(const QString &computerName){

    //    if(!query){
    //        if(!openDatabase()){
    //            return false;
    //        }
    //    }else{
    //        query->clear();
    //    }

    //    QString queryString = QString("select ComputerName from clientinfo where ComputerName = '%1'").arg(computerName);

    //    if(!query->exec(queryString)){
    //        qWarning()<<query->lastError().text();
    //        return false;
    //    }
    //    query->first();
    //    QString value = query->value(0).toString();
    //    qWarning()<<"value:"<<value;
    //    if(value == computerName){
    //        return true;
    //    }

    //    return false;

    return recordsInDatabase.contains(computerName, Qt::CaseInsensitive);

}


void ServerService::start()
{

    //return;

    qDebug()<<"----ServerService::start()";

    resourcesManager = ResourcesManagerInstance::instance();
    serverPacketsParser = 0;

    databaseUtility = new DatabaseUtility(this);

    mainServiceStarted = false;

    startMainService();

}

void ServerService::stop()
{

    if(serverPacketsParser){
        serverPacketsParser->sendServerOfflinePacket();
    }

    updateOrSaveAllClientsInfoToDatabase();

    databaseUtility->closeAllDBConnections();

    if(m_udpServer){
        m_udpServer->close();
    }
    if(m_rtp){
        m_rtp->stopServers();
    }

}

void ServerService::pause()
{

}

void ServerService::resume()
{

}

void ServerService::processCommand(int code)
{

    qDebug()<<"----ServerService::processCommand(int code)";
    qDebug()<<"code:"<<code;


    switch(code){
    case 0:
        serverPacketsParser->sendUpdateClientSoftwarePacket();
        break;
    case 1:
        serverPacketsParser->sendRequestClientInfoPacket();
        break;
    case 2:
        updateOrSaveAllClientsInfoToDatabase();
        break;
    case 3:
        serverPacketsParser->sendRequestClientInfoPacket("255.255.255.255", IP_MULTICAST_GROUP_PORT, "", false);
        break;
    default:
        qWarning()<<QString("Unknown Command Code '%1'!").arg(code);
        break;

    }


}





























} //namespace HEHUI

