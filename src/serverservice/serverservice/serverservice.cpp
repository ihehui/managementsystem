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
 * Last Modified on: 2015-4-10
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QtCore>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "serverservice.h"

#include "../app_constants.h"
#include "../sharedms/settings.h"



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

    m_isUsingMySQL = true;

    processArguments(argc, argv);

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

    getAllClientsInfoFromDB();

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


    m_rtp = resourcesManager->startRTP(QHostAddress::Any, RTP_LISTENING_PORT+1, true, &errorMessage);
    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)), Qt::QueuedConnection);

    //    m_udtProtocol = m_rtp->getUDTProtocol();
    //    m_udtProtocol->startWaitingForIOInOneThread(30);
    //m_udtProtocol->startWaitingForIOInSeparateThread(10, 500);



    serverPacketsParser = new ServerPacketsParser(resourcesManager, this);
    //connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));
    //connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)));

    connect(serverPacketsParser, SIGNAL(signalClientLogReceived(const QString&, const QString&, quint8, const QString&, const QString&)), this, SLOT(saveClientLog(const QString&, const QString&, quint8, const QString&, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientInfoPacketReceived(const QString &, const QByteArray &, quint8)), this, SLOT(clientInfoPacketReceived(const QString &, const QByteArray &, quint8)), Qt::QueuedConnection);

    connect(serverPacketsParser, SIGNAL(signalModifyAssetNOPacketReceived(SOCKETID, const QString &, const QString &, const QString &)), this, SLOT(processModifyAssetNOPacket(SOCKETID, const QString &, const QString &, const QString &)));

    connect(serverPacketsParser, SIGNAL(signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID, const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)), this, SLOT(processRequestChangeProcessMonitorInfoPacket(SOCKETID, const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)), Qt::QueuedConnection);

    connect(serverPacketsParser, SIGNAL(signalClientInfoRequestedPacketReceived(SOCKETID, const QString &, quint8)), this, SLOT(processClientInfoRequestedPacket(SOCKETID, const QString &, quint8)), Qt::QueuedConnection);


    //    connect(serverPacketsParser, SIGNAL(signalHeartbeatPacketReceived(const QString &, const QString&)), this, SLOT(processHeartbeatPacket(const QString &, const QString&)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalClientOnlineStatusChanged(SOCKETID, const QString&, bool, const QString&, quint16)), this, SLOT(processClientOnlineStatusChangedPacket(SOCKETID, const QString&, bool, const QString&, quint16)), Qt::QueuedConnection);
    connect(serverPacketsParser, SIGNAL(signalAdminLogin(SOCKETID, const QString &, const QString &, const QString &, const QString &)), this, SLOT(processAdminLoginPacket(SOCKETID, const QString &, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
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

void ServerService::saveClientLog(const QString &assetNO, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime){
    //qWarning()<<"ServerService::saveClientLog(...) IP:"<<clientAddress<<" log:"<<log;

    QString statement = QString("call sp_Logs_Save('%1', '%2', %3, '%4', '%5', '%6');")
            .arg(assetNO)
            .arg(clientAddress)
            .arg(logType)
            .arg(log)
            .arg("NULL")
            .arg(clientTime)
            ;

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when saving client log to database. Asset NO.: %1.").arg(assetNO);
        logMessage(error, QtServiceBase::Error);
    }

}

void ServerService::sendServerOnlinePacket(){
    qDebug()<<"----ServerService::sendServerOnlinePacket()";

    serverPacketsParser->sendServerDeclarePacket(QHostAddress::Broadcast, quint16(IP_MULTICAST_GROUP_PORT));
    serverPacketsParser->sendServerDeclarePacket(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT));

    //serverPacketsParser->sendServerOnlinePacket();
    
    //updateOrSaveAllClientsInfoToDatabase();

}

//bool ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info){
//    //    qWarning()<<"ServerService::updateOrSaveClientInfoToDatabase(ClientInfo *info)";

//    if(!info){
//        return false;
//    }

//    QString assetNO = info->getAssetNO();

//    QString osInfoStatement = "";
//    QString hardwareInfoStatement = "";
//    QString updateInstalledSoftwaresInfoStatement = "";

//    //    if(isRecordExistInDB(info->getComputerName())){
//    if(!info->getOSInfoSavedTODatabase()){
//        osInfoStatement = info->getUpdateOSInfoStatement();
//    }
//    if(!info->getHardwareInfoSavedTODatabase()){
//        hardwareInfoStatement = info->getUpdateHardwareInfoStatement();
//        //qWarning()<<"detailedStatement:"<<detailedStatement;
//    }
//    if(!info->isInstalledSoftwaresInfoSavedTODatabase()){
//        updateInstalledSoftwaresInfoStatement = info->getUpdateInstalledSoftwaresInfoStatement();
//    }


//    if(!query){
//        if(!openDatabase()){
//            return false;
//        }
//    }else{
//        query->clear();
//    }

//    if(!osInfoStatement.trimmed().isEmpty()){
//        if(!query->exec(osInfoStatement)){
//            QSqlError error = query->lastError();
//            QString msg = QString("Can not write client summary info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//            logMessage(msg, QtServiceBase::Error);
//            qCritical()<<msg;
//            qCritical()<<"summaryStatement:";
//            qCritical()<<osInfoStatement;
//            qCritical()<<"";
//            //MySQL数据库重启，重新连接
//            if(error.number() == 2006){
//                query->clear();
//                openDatabase(true);
//            }else{
//                getRecordsInDatabase();
//            }

//            return false;
//        }
//        //        qWarning()<<" Summary Statement For "<<computerName<<": "<<summaryStatement;

//        info->setOSInfoSavedTODatabase(true);
//        info->setUpdateOSInfoStatement("");
//        qWarning()<<"Client OS info from "<<assetNO<<" has been saved!";

//        query->clear();
//    }

//    if(!hardwareInfoStatement.trimmed().isEmpty()){
//        if(!query->exec(hardwareInfoStatement)){
//            QSqlError error = query->lastError();
//            QString msg = QString("Can not write client detailed info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//            logMessage(msg, QtServiceBase::Error);
//            qCritical()<<msg;

//            //MySQL数据库重启，重新连接
//            if(error.number() == 2006){
//                query->clear();
//                openDatabase(true);
//            }else{
//                getRecordsInDatabase();
//            }

//            return false;
//        }
//        //        qWarning()<<" Detailed Statement For "<<computerName<<": "<<detailedStatement;

//        info->setHardwareInfoSavedTODatabase(true);
//        info->setUpdateHardwareInfoStatement("");
//        qWarning()<<"Client hardware info from "<<assetNO<<" has been saved!";

//        query->clear();

//    }

//    if(!updateInstalledSoftwaresInfoStatement.trimmed().isEmpty()){
//        query->exec(updateInstalledSoftwaresInfoStatement);
//        QSqlError error = query->lastError();
//        if(error.type() != QSqlError::NoError){
//            //QSqlError error = query->lastError();
//            QString msg = QString("Can not write client installed softwares info to database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
//            logMessage(msg, QtServiceBase::Error);
//            qCritical()<<msg;

//            //MySQL数据库重启，重新连接
//            if(error.number() == 2006){
//                query->clear();
//                openDatabase(true);
//            }else{
//                getRecordsInDatabase();
//            }

//            return false;
//        }
//        //        qWarning()<<" Installed Softwares Statement For "<<computerName<<": "<<updateInstalledSoftwaresInfoStatement;

//        info->setInstalledSoftwaresInfoSavedTODatabase(true);
//        info->setUpdateInstalledSoftwaresInfoStatement("");
//        qWarning()<<"Client installed softwares info from "<<assetNO<<" has been saved!";

//        query->clear();

//    }



//    if(!isRecordExistInDB(assetNO)){
//        recordsInDatabase.append(assetNO.toLower());
//    }

//    return true;

//}

//void ServerService::updateOrSaveAllClientsInfoToDatabase(){

//    getRecordsInDatabase();

//    foreach(ClientInfo *info, clientInfoHash.values()){
//        updateOrSaveClientInfoToDatabase(info);
//    }

//}

void ServerService::clientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType){
    qDebug()<<"--ServerService::clientInfoPacketReceived(...) "<<" assetNO:"<<assetNO;

    if(assetNO.trimmed().isEmpty()){
        qCritical()<<"Invalid Asset NO. !";
        return;
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(assetNO)){
        info = clientInfoHash.value(assetNO);
    }else{
        info = new ClientInfo(assetNO, this);
        clientInfoHash.insert(assetNO, info);
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

    default:
        break;
    }


}

void ServerService::processOSInfo(ClientInfo *info, const QByteArray &osData){
    if(!info){
        return;
    }

    QString assetNO = info->getAssetNO();

    //    QString computerName = info->getComputerName();
    //    QString osInfo = info->getOSVersion();
    //    QString installationDate = info->getInstallationDate();
    //    QString osKey = info->getOsKey();
    //    QString workgroupName = info->getWorkgroup();
    //    bool isJoinedToDomain = info->isJoinedToDomain();
    //    QString usersInfo = info->getUsers();
    //    QString admins = info->getAdministrators();
    //    QString ipInfo = info->getIP();
    //    QString clientVersion = info->getClientVersion();

    QString newipInfo = info->getIP();
    info->setJsonData(osData);
    info->setIP(newipInfo);

    QString newComputerName = info->getComputerName();
    QString newOSInfo = info->getOSVersion();
    QString newinstallationDate = info->getInstallationDate();
    QString newOSKey = info->getOsKey();
    QString newworkgroupName = info->getWorkgroup();
    bool newJoinedToDomain = info->isJoinedToDomain();
    QString newUsers = info->getUsers();
    QString newadmins = info->getAdministrators();
    //QString newipInfo = info->getIP();
    QString newclientVersion = info->getClientVersion();
    quint8 newUsbSDStatus = quint8(info->getUsbSDStatus());

    if(m_isUsingMySQL){
        newUsers = newUsers.replace("\\", "\\\\");
        newadmins = newadmins.replace("\\", "\\\\");
    }

    QString statement;
    //    if(isRecordExistInDB(assetNO)){
    //        qDebug()<<"Client Info Exists!";

    //        statement = "UPDATE OS SET LastOnlineTime = NULL ";

    //        if(computerName != newComputerName){
    //            statement += QString(", ComputerName = '%1' ").arg(newComputerName);
    //        }

    //        if(osInfo != newOSInfo){
    //            statement += QString(", OSVersion = '%1' ").arg(newOSInfo);
    //        }

    //        if(installationDate != newinstallationDate){
    //            statement += QString(", InstallationDate = '%1' ").arg(newinstallationDate);
    //        }

    //        if(osKey != newOSKey){
    //            statement += QString(", OSKey = '%1' ").arg(newOSKey);
    //        }

    //        if(workgroupName != newworkgroupName){
    //            statement += QString(", Workgroup = '%1' ").arg(newworkgroupName);
    //        }

    //        if(isJoinedToDomain != newJoinedToDomain){
    //            statement += QString(", JoinedToDomain = %1 ").arg(newJoinedToDomain?1:0);
    //        }

    //        if(usersInfo != newUsers){
    //            QString tempUsersInfo = newUsers;
    //            tempUsersInfo.replace("\\", "\\\\");
    //            statement += QString(", Users = '%1' ").arg(tempUsersInfo);
    //        }

    //        if(admins != newadmins){
    //            QString tempAdminsInfo = newadmins;
    //            tempAdminsInfo.replace("\\", "\\\\");
    //            statement += QString(", Administrators = '%1' ").arg(tempAdminsInfo);
    //        }

    //        if(ipInfo != newipInfo){
    //            statement += QString(", IP = '%1' ").arg(newipInfo);
    //        }

    //        if(clientVersion != newclientVersion){
    //            statement += QString(", ClientVersion = '%1' ").arg(newclientVersion);
    //        }

    //        statement += QString("WHERE AssetNO = '%1'").arg(assetNO);
    //        //qWarning()<<"Update Client Info For Computer "<<computerName << " "<<networkInfo;
    //        //qWarning()<<statement;

    //    }else{
    //        qDebug()<<"Client OS Info Not Exists!";

    //        statement = QString("call sp_OS_Update('%1', '%2', '%3', '%4', '%5', '%6', %7, '%8', '%9', '%10', '%11' ); ")
    //                .arg(assetNO)
    //                .arg(newComputerName)
    //                .arg(newOSInfo)
    //                .arg(newinstallationDate)
    //                .arg(newOSKey)
    //                .arg(newworkgroupName)
    //                .arg(QVariant(newJoinedToDomain).toUInt())
    //                .arg(newUsers)
    //                .arg(newadmins)
    //                .arg(newipInfo)
    //                .arg(newclientVersion)
    //                ;
    //    }


    statement = QString("call sp_OS_Update('%1', '%2', '%3', '%4', '%5', '%6', %7, '%8', '%9', '%10', '%11', %12 ); ")
            .arg(assetNO)
            .arg(newComputerName)
            .arg(newOSInfo)
            .arg(newinstallationDate)
            .arg(newOSKey)
            .arg(newworkgroupName)
            .arg(QVariant(newJoinedToDomain).toUInt())
            .arg(newUsers)
            .arg(newadmins)
            .arg(newipInfo)
            .arg(newclientVersion)
            .arg(newUsbSDStatus)
            ;

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when saving OS info to database. Asset NO.: %1.").arg(assetNO);
        logMessage(error, QtServiceBase::Error);
    }


    //    info->setOSInfoSavedTODatabase(false);
    //    info->setUpdateOSInfoStatement(statement);

    //    //if(onlineAdminsCount > 0){
    //        if(updateOrSaveClientInfoToDatabase(info)){
    //            info->setLastHeartbeatTime(QDateTime::currentDateTime());
    //        }
    //    //}

    qWarning();


}

void ServerService::processHardwareInfo(ClientInfo *info, const QByteArray &hardwareData){
    qDebug()<<"--ServerService::processHardwareInfo(...)";

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
    statement = "UPDATE Hardware SET UpdateTime = NULL ";

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


    QString assetNO = info->getAssetNO();
    statement += QString("WHERE AssetNO = '%1'").arg(assetNO);

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when saving hardware info to database. Asset NO.: %1.").arg(assetNO);
        logMessage(error, QtServiceBase::Error);
    }



    //    info->setHardwareInfoSavedTODatabase(false);
    //    info->setUpdateHardwareInfoStatement(statement);

    //    //if(onlineAdminsCount > 0){
    //        if(updateOrSaveClientInfoToDatabase(info)){
    //            info->setLastHeartbeatTime(QDateTime::currentDateTime());
    //        }
    //    //}

    if(changes.isEmpty()){return;}

    QString alarmStatement ;
    quint8 alarmType = quint8(MS::ALARM_HARDWARECHANGE);
    foreach (QString change, changes) {
        alarmStatement += QString("INSERT INTO Alarm(AssetNO, AlarmType, Message) VALUES('%1', %2, '%3' ); ")
                .arg(assetNO)
                .arg(alarmType)
                .arg(change)
                ;
    }

    if(!execQuery(alarmStatement)){
        QString error = QString("ERROR! An error occurred when saving hardware alarm info to database. Asset NO.: %1.").arg(assetNO);
        logMessage(error, QtServiceBase::Error);
    }

    //    info->setUpdateAlarmsInfoStatement(alarmStatement);


    qCritical()<<"!!!!!!!!!!!!!Hardware Changed!!!!!!!!!!!!!";
    qCritical()<<"Asset NO.:"<<assetNO;
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

    int softwareCount = array.size();

    QString assetNO = info->getAssetNO();
    QString statement = QString("START TRANSACTION; delete from InstalledSoftware where AssetNO = '%1'; ").arg(assetNO);

    for(int i=0;i<softwareCount;i++){
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 4){continue;}

        statement += QString(" insert into InstalledSoftware values(NULL, '%1', '%2', '%3', '%4', '%5'); ")
                .arg(assetNO)
                .arg(infoArray.at(0).toString())
                .arg(infoArray.at(1).toString())
                .arg(infoArray.at(2).toString())
                .arg(infoArray.at(3).toString())
                ;
    }
    statement += "COMMIT;";


    //    info->setInstalledSoftwaresInfoSavedTODatabase(false);
    //    info->setUpdateInstalledSoftwaresInfoStatement(statement);

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when saving installed software info to database. Asset NO.: %1.").arg(assetNO);
        logMessage(error, QtServiceBase::Error);
    }


}

void ServerService::processClientInfoRequestedPacket(SOCKETID socketID, const QString &assetNO, quint8 infoType){

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
        getOSInfo(socketID, assetNO);
        break;

    case quint8(MS::SYSINFO_HARDWARE):
        getHardwareInfo(socketID, assetNO);
        break;

    case quint8(MS::SYSINFO_SOFTWARE):
        getSoftwareInfo(socketID, assetNO);
        break;



        //    case quint8(MS::SYSINFO_USERS):
        //        systemInfo->getUsersInfo(socketID);
        //        break;


    default:
        qCritical()<<"ERROR! Unknown info type:"<<infoType;
        break;
    }





}

void ServerService::processModifyAssetNOPacket(SOCKETID socketID, const QString &newAssetNO, const QString &oldAssetNO, const QString &adminName){

    if(newAssetNO.isEmpty() || oldAssetNO.isEmpty()){
        QString message = "Invalid asset NO.";
        serverPacketsParser->sendServerResponseModifyAssetNOPacket(socketID, newAssetNO, oldAssetNO, false, message);
        return;
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(oldAssetNO)){
        info = clientInfoHash.value(oldAssetNO);
    }else{
        QString message = QString("Asset NO. '%1' not found!").arg(oldAssetNO);
        serverPacketsParser->sendServerResponseModifyAssetNOPacket(socketID, newAssetNO, oldAssetNO, false, message);
        return;
    }


    QString statement = QString("call sp_OS_AssetNO_Update('%1', '%2'); ")
            .arg(newAssetNO)
            .arg(oldAssetNO)
            ;

    QString errStr = "";
    if(!execQuery(statement, &errStr)){
        QString message = QString("An error occurred when updating asset NO. to database. Old asset NO.:%1, new Asset No.:%2. %3").arg(oldAssetNO).arg(newAssetNO).arg(errStr);
        logMessage(errStr, QtServiceBase::Error);

        serverPacketsParser->sendServerResponseModifyAssetNOPacket(socketID, newAssetNO, oldAssetNO, false, message);
        return;
    }

    serverPacketsParser->sendServerResponseModifyAssetNOPacket(socketID, newAssetNO, oldAssetNO, true, "");

}


void ServerService::processRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRules, const QByteArray &globalRules, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO){

    QString adminName = adminSocketsHash.value(socketID);
    Q_ASSERT(adminName.isEmpty());

    QString rules;
    if(assetNO.isEmpty()){
        rules = QString(globalRules);
    }else{
        rules = QString(localRules);
    }

    QString statement = QString("call sp_ProcessMonitorSettings_Update('%1', %2, '%3', %4, %5, %6, %7, '%8');")
            .arg(assetNO)
            .arg(enableProcMon)
            .arg(rules)
            .arg(enablePassthrough)
            .arg(enableLogAllowedProcess)
            .arg(enableLogBlockedProcess)
            .arg(useGlobalRules)
            .arg(adminName)
            ;

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when saving process monitor info to database. Admin: %1.").arg(adminName);
        logMessage(error, QtServiceBase::Error);
    }




}

void ServerService::processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientAssetNO, bool online, const QString &ip, quint16 port){
    qDebug()<<"ServerService::processClientOnlineStatusChangedPacket(...)"<<" socketID:"<<socketID<<" clientAssetNO:"<<clientAssetNO<<" online:"<<online;


    if(online){

        if(clientSocketsHash.values().contains(clientAssetNO)){
            SOCKETID preSocketID = clientSocketsHash.key(clientAssetNO);
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
        clientSocketsHash.insert(socketID, clientAssetNO);

    }else{
        clientSocketsHash.remove(socketID);
        m_rtp->closeSocket(socketID);
    }

    ClientInfo *info = 0;
    if(clientInfoHash.contains(clientAssetNO)){
        info = clientInfoHash.value(clientAssetNO);
    }else{
        info =  new ClientInfo(clientAssetNO, this);
        clientInfoHash.insert(clientAssetNO, info);

        //        if(online){
        //            serverPacketsParser->sendServerRequestClientSummaryInfoPacket(socketID, "", clientName, "");
        //        }
        //qWarning()<<QString("Unknown Client '%1' %2 ! %3").arg(clientName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        //qWarning()<<QString("Unknown Client '%1' From %2:%3 %4 ! %5").arg(clientName).arg(ip).arg(port).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    }

    info->setIP(ip);
    info->setOnline(online);
    if(online){
        info->setClientUDTListeningAddress(ip);
        info->setClientUDTListeningPort(port);
    }

    qWarning();
    qWarning()<<QString("Client '%1' From %2:%3 %4 via %5! Time:%6 Socket: %7").arg(clientAssetNO).arg(ip).arg(port).arg(online?"Online":"Offline").arg(m_rtp->socketProtocolString(socketID)).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(socketID);
    qWarning()<<QString("Total Online Clients:%1").arg(clientSocketsHash.size());


}

void ServerService::processAdminLoginPacket(SOCKETID socketID, const QString &adminName, const QString &password, const QString &adminIP, const QString &adminComputerName){
    qDebug()<<"--ServerService::processAdminLoginPacket(...) "<<" adminName:"<<adminName<<" password:"<<password;
    bool verified = false, readonly = true;
    QString message = "";

    QString statement = QString("call sp_Admin_Login('%1', '%2', '%3', '%4' );")
            .arg(adminName)
            .arg(password)
            .arg(adminIP)
            .arg(adminComputerName)
            ;

    if(!execQuery(statement)){
        QString error = QString("ERROR! An error occurred when querying admin info. Admin: %1.").arg(adminName);
        logMessage(error, QtServiceBase::Error);
        verified = false;
        message = error;
    }else{
        if(!query->first()){
            verified = false;
            message = "Invalid user name or password.";
        }else{
            verified = true;
            readonly = query->value(0).toUInt();
        }
    }

    //TODO

    serverPacketsParser->sendAdminLoginResultPacket(socketID, verified, message, readonly);
}

void ServerService::processAdminOnlineStatusChangedPacket(SOCKETID socketID, const QString &adminComputerName, const QString &adminName, bool online){

    QString ip = "";
    quint16 port = 0;

    if(!m_rtp->getAddressInfoFromSocket(socketID, &ip, &port)){
        qCritical()<<m_rtp->lastErrorString();

        return;
    }

    qWarning()<<QString(" Admin %1@%2 %3! %4").arg(adminName).arg(adminComputerName).arg(online?"Online":"Offline").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));

    if(online){
        adminSocketsHash.insert(socketID, adminName);
        //updateOrSaveAllClientsInfoToDatabase();
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
    qDebug()<<"ServerService::peerDisconnected(...) "<<" socketID:"<<socketID<<" Time:"<<QDateTime::currentDateTime().toString("mm:ss:zzz")<<" ThreadID:"<<QThread::currentThreadId();

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
        Settings settings(SERVICE_NAME, "./");
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

    if(db.driverName().toUpper() == "QMYSQL"){
        m_isUsingMySQL = true;
    }else{
        m_isUsingMySQL = false;
    }


    return true;



}

bool ServerService::execQuery(const QString &statement, QString *errorString ){

    if(!query){
        if(!openDatabase()){
            return false;
        }
    }
    query->clear();

    if(!query->exec(statement)){
        QSqlError error = query->lastError();
        QString msg = QString("Can not execute the SQL statement! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        logMessage(msg, QtServiceBase::Error);
        if(errorString){
            *errorString = msg;
        }

        qCritical()<<msg;
        qCritical()<<"statement:";
        qCritical()<<statement;
        qCritical()<<"";
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query->clear();
            openDatabase(true);
        }else{
            getAllClientsInfoFromDB();
        }
        query->clear();

        return false;
    }


    return true;

}

void ServerService::getAllClientsInfoFromDB(){

    QString statement = QString("call sp_OS_Query('', '', '', '', '', '', -1, -1 ); ");
    if(!execQuery(statement)){
        return;
    }
    while(query->next()){
        QString assetNO = query->value("AssetNO").toString();
        ClientInfo *info = 0;
        if(clientInfoHash.contains(assetNO)){
            info = clientInfoHash.value(assetNO);
        }else{
            info = new ClientInfo(assetNO, this);
            clientInfoHash.insert(assetNO, info);
        }

        //info->setAssetNO(assetNO);
        info->setComputerName(query->value("ComputerName").toString());
        info->setOSVersion(query->value("OSVersion").toString());
        info->setInstallationDate(query->value("InstallationDate").toString());
        info->setOsKey(query->value("OSKey").toString());
        info->setWorkgroup(query->value("Workgroup").toString());
        info->setIsJoinedToDomain(query->value("JoinedToDomain").toBool());
        info->setUsers(query->value("Users").toString());
        info->setAdministrators(query->value("Administrators").toString());
        info->setIP(query->value("IP").toString());
        info->setClientVersion(query->value("ClientVersion").toString());
        info->setProcessMonitorEnabled(query->value("ProcessMonitorEnabled").toBool());
        info->setUsbSDStatus(query->value("USB").toUInt());
        info->setLastOnlineTime(query->value("LastOnlineTime").toDateTime());

    }
    query->clear();

    statement = QString("call sp_Hardware_Query('%'); ");
    if(!execQuery(statement)){
        return;
    }
    while(query->next()){
        QString assetNO = query->value("AssetNO").toString();
        ClientInfo *info = 0;
        if(clientInfoHash.contains(assetNO)){
            info = clientInfoHash.value(assetNO);
        }else{
            info = new ClientInfo(assetNO, this);
            clientInfoHash.insert(assetNO, info);
            qCritical()<<QString("ERROR! Client '%1' OS info not found!").arg(assetNO);
        }

        info->setCpu(query->value("CPU").toString());
        info->setMotherboardName(query->value("Motherboard").toString());
        info->setMemory(query->value("Memory").toString());
        info->setStorage(query->value("Storage").toString());
        info->setMonitor(query->value("Monitor").toString());
        info->setVideo(query->value("Video").toString());
        info->setAudio(query->value("Audio").toString());
        info->setNetwork(query->value("NIC").toString());
        //info->setu(query->value("UpdateTime").toString());
        //info->setClientVersion(query->value("Remark").toString());
    }
    query->clear();

}

void ServerService::getOSInfo(SOCKETID socketID, const QString &assetNO){

    QJsonArray infoArray;
    foreach (ClientInfo *info, clientInfoHash.values()) {
        infoArray.append(QString(info->getOSJsonData()));
    }

    QJsonObject object;
    object["OS"] = infoArray;

    QJsonDocument doc(object);
    serverPacketsParser->sendClientInfoPacket(socketID, assetNO, doc.toJson(QJsonDocument::Compact), MS::SYSINFO_OS);

}

void ServerService::getHardwareInfo(SOCKETID socketID, const QString &assetNO){
    QJsonArray infoArray;
    foreach (ClientInfo *info, clientInfoHash.values()) {
        infoArray.append(QString(info->getHardwareJsonData()));
    }

    QJsonDocument doc(infoArray);
    serverPacketsParser->sendClientInfoPacket(socketID, assetNO, doc.toJson(QJsonDocument::Compact), MS::SYSINFO_OS);
}

void ServerService::getSoftwareInfo(SOCKETID socketID, const QString &assetNO){

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
        serverPacketsParser->sendServerOnlineStatusChangedPacket(false);
    }

    //updateOrSaveAllClientsInfoToDatabase();

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
        //updateOrSaveAllClientsInfoToDatabase();
        break;
    case 3:
        serverPacketsParser->sendRequestClientInfoPacket("255.255.255.255", IP_MULTICAST_GROUP_PORT, "", false);
        break;
    default:
        qWarning()<<QString("Unknown Command Code '%1'!").arg(code);
        break;

    }


}


void ServerService::processArguments(int argc, char **argv){

    QCoreApplication core(argc, argv);
    qDebug()<<"-------availableDrivers:"<<DatabaseUtility::availableDrivers();


    QStringList arguments;
    for(int i = 0; i < argc; i++){
        arguments.append(QString(argv[i]));
    }

    HEHUI::Settings settings(SERVICE_NAME, "./");
    if(settings.getDBServerHost().isEmpty()
            || settings.getDBName().isEmpty()
            || settings.getDBServerUserName().isEmpty()
            ){
        qCritical()<<QString("No database settings found!");
        arguments.append("-setup");
    }

    if(arguments.contains("-setup", Qt::CaseInsensitive)){
        QStringList databaseTypes;
        databaseTypes<<"Other"<<"MYSQL"<<"SQLITE"<<"POSTGRESQL"<<"FIREBIRD"<<"DB2"<<"ORACLE"<<"M$ SQLSERVER"<<"M$ ACCESS";

        //HEHUI::Settings settings(SERVICE_NAME, "./");
        wcout<<tr("Current Database Info:").toStdWString()<<endl;
        wcout<<tr("\tDatabase type: ").toStdWString()<<databaseTypes.at(settings.getDBType()).toStdWString()<<endl;
        wcout<<tr("\tDriver: ").toStdWString()<<settings.getDBDriver().toStdWString()<<endl;
        wcout<<tr("\tDatabase server address: ").toStdWString()<<settings.getDBServerHost().toStdWString()<<endl;
        wcout<<tr("\tDatabase server port: ").toStdWString()<<settings.getDBServerPort()<<endl;
        wcout<<tr("\tUser name: ").toStdWString()<<settings.getDBServerUserName().toStdWString()<<endl;
        //wcout<<tr("\tPassword: ").toStdWString()<<settings.getDBServerUserPassword().toStdWString()<<endl;
        wcout<<tr("\tDatabase name: ").toStdWString()<<settings.getDBName().toStdWString()<<endl<<endl;

        wcerr<<tr("Database settings is invalid! Please reconfigure it!").toStdWString()<<endl;


        string input = "";
        bool ok = false;

        wcout<<tr("Database type:").toStdWString()<<endl;
        for(int i = 0; i<databaseTypes.size(); i++){
            QString type = databaseTypes.at(i);
            cout<<"\t"<<i<<":"<<qPrintable(type)<<endl;
        }
        //cout<<" 0:Other  1:MYSQL  2:SQLITE  3:POSTGRESQL  4:FIREBIRD  5:DB2  6:ORACLE  7:M$SQLSERVER  8:M$ACCESS"<<endl;
        int type = -1;
        while(1){
            wcout<<tr("Please select database type number: ").toStdWString();
            cin>>input;
            type = QString::fromStdString(input).toInt(&ok);
            if(ok && type >= 0 && type < databaseTypes.size()){
                break;
            }else{
                wcerr<<tr("Invalid type number!").toStdWString()<<endl;
            }
        }
        cout<<endl;
        input = "";

        wcout<<tr("Available Database Drivers:").toStdWString()<<endl;
        QStringList drivers = DatabaseUtility::availableDrivers();
        for(int i = 0; i<drivers.size(); i++){
            QString driver = drivers.at(i);
            cout<<"\t"<<i<<":"<<qPrintable(driver)<<endl;
        }
        QString driver = "";
        while(1){
            wcout<<tr("Please select database driver number: ").toStdWString();
            cin>>input;
            int driverNO = QString::fromStdString(input).toInt(&ok);
            if(ok && driverNO >= 0 && driverNO < drivers.size()){
                driver = drivers.at(driverNO);
                break;
            }else{
                wcerr<<tr("Invalid driver number!").toStdWString()<<endl;
            }
        }
        cout<<endl;
        input = "";

        QString host = "";
        while(1){
            wcout<<tr("Please input database server host name or IP address: ").toStdWString();
            cin>>input;
            host = QString::fromStdString(input);
            if(QHostAddress(host).isNull()){
                wcerr<<tr("Invalid database server host name or IP address!").toStdWString()<<endl;
            }else{
                break;
            }
        }
        cout<<endl;
        input = "";

        int port = 0;
        while(1){
            wcout<<tr("Please input database server port: ").toStdWString();
            cin>>input;
            port = QString::fromStdString(input).toInt(&ok);
            if(ok && port > 0 && port < 65535){
                break;
            }else{
                wcerr<<tr("Invalid database server port!").toStdWString()<<endl;
            }

        }
        cout<<endl;
        input = "";

        QString databaseName = "";
        while(1){
            wcout<<tr("Please input database name: ").toStdWString();
            cin>>input;
            databaseName = QString::fromStdString(input).trimmed();
            if(databaseName.isEmpty()){
                wcerr<<tr("Invalid database name!").toStdWString()<<endl;
            }else{
                break;
            }

        }
        cout<<endl;
        input = "";

        QString userName = "";
        while(1){
            wcout<<tr("Please input user name: ").toStdWString();
            cin>>input;
            userName = QString::fromStdString(input).trimmed();
            if(userName.isEmpty()){
                wcerr<<tr("Invalid user name!").toStdWString()<<endl;
            }else{
                break;
            }

        }
        cout<<endl;
        input = "";

        QString userPassword = "";
        while(1){
            wcout<<tr("Please input password: ").toStdWString();
            cin>>input;
            userPassword = QString::fromStdString(input);

            string userPassword2 = "";
            wcout<<tr("Please type your password again to confirm it: ").toStdWString();
            cin>>userPassword2;
            if(userPassword == QString::fromStdString(userPassword2)){
                break;
            }else{
                wcout<<tr("The two passwords you entered did not match!").toStdWString()<<endl;
            }

        }
        cout<<endl;
        input = "";


        wcout<<tr("New Database Info:").toStdWString()<<endl;
        wcout<<tr("\tDatabase type: ").toStdWString()<<databaseTypes.at(type).toStdWString()<<endl;
        wcout<<tr("\tDatabase driver: ").toStdWString()<<driver.toStdWString()<<endl;
        wcout<<tr("\tDatabase server address: ").toStdWString()<<host.toStdWString()<<endl;
        wcout<<tr("\tDatabase server port: ").toStdWString()<<port<<endl;
        wcout<<tr("\tDatabase name: ").toStdWString()<<databaseName.toStdWString()<<endl;
        wcout<<tr("\tUser name: ").toStdWString()<<userName.toStdWString()<<endl;
        wcout<<tr("\tPassword: ").toStdWString()<<userPassword.toStdWString()<<endl<<endl;


        wcout<<tr("Testing database connection...").toStdWString()<<endl;
        DatabaseUtility databaseUtility;
        QSqlError err = databaseUtility.openDatabase(SERVERSERVICE_DB_CONNECTION_NAME,
                                                     driver,
                                                     host,
                                                     port,
                                                     userName,
                                                     userPassword,
                                                     databaseName,
                                                     HEHUI::DatabaseType(type));
        if (err.type() != QSqlError::NoError) {
            QString errorString = tr("Error! Database connection failed! An error occurred when opening the database: %1").arg(err.text());
            wcerr<<errorString.toStdWString()<<endl;
            qCritical() << errorString;

            exit( err.type() );
        }else{
            QString str = tr("Database connection succeeded!");
            wcout<<str.toStdWString()<<endl;
            qWarning()<<str;
        }
        //databaseUtility.closeDBConnection(SERVERSERVICE_DB_CONNECTION_NAME);
        qWarning();


        QString confirmString = "";
        wcout<<tr("Do you wnat to save the changes?[Y:Yes  N:No]").toStdWString();
        cin>>input;
        confirmString = QString::fromStdString(input).toLower();
        input = "";
        if(confirmString == "y" || confirmString == "yes"){
            settings.setDBType(type);
            settings.setDBDriver(driver);
            settings.setDBServerHost(host);
            settings.setDBServerPort(port);
            settings.setDBName(databaseName);
            settings.setDBServerUserName(userName);
            settings.setDBServerUserPassword(userPassword);
            wcout<<tr("Database Info Saved!").toStdWString()<<endl<<endl;
        }else{
            wcout<<tr("Operation canceled! Nothing changes!").toStdWString()<<endl<<endl;
        }


    }


}



























} //namespace HEHUI

