/*
 ****************************************************************************
 * clientpacketsparser.cpp
 *
 * Created On: 2010-7-13
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
 * Last Modified On: 2010-7-13
 * Last Modified By: 贺辉
 ***************************************************************************
 */



#include "clientpacketsparser.h"




namespace HEHUI {


ClientPacketsParser::ClientPacketsParser(const QString &assetNO, ClientResourcesManager *manager, QObject *parent)
    :QObject(parent), m_assetNO(assetNO), m_resourcesManager(manager)
{

    if(m_assetNO.trimmed().isEmpty()){
        m_assetNO = QHostInfo::localHostName().toLower();
    }
    //m_localComputerName = QHostInfo::localHostName().toLower();


    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid UDPServer!");
    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    Q_ASSERT(m_udtProtocol);
//    m_udtProtocol->startWaitingForIOInOneThread(10);
//    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
//    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_enetProtocol = m_rtp->getENETProtocol();
    Q_ASSERT(m_enetProtocol);
    connect(m_enetProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    serverAddress = QHostAddress::Null;
    serverRTPListeningPort = 0;
    serverName = "";

//    m_localUDTServerListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_rtp->getTCPServerPort();
    m_localENETListeningPort = m_rtp->getENETProtocolPort();


    m_socketConnectedToAdmin = INVALID_SOCK_ID;
    
}

ClientPacketsParser::~ClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"ClientPacketsParser::~ClientPacketsParser() ";






}


void ClientPacketsParser::parseIncomingPacketData(Packet *packet){
    //    qDebug()<<"----ClientPacketsParser::parseIncomingPacketData(Packet *packet)";


    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    QString peerName = "";
    in >> peerName;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
    quint8 packetType = packet->getPacketType();
    SOCKETID socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);

//    qDebug()<<"--ClientPacketsParser::parseIncomingPacketData(...) "<<" peerName:"<<peerName<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" socketID:"<<socketID<<" packetType:"<<packetType;

    switch(packetType){

    case quint8(MS::ServerDeclare):
    {
        quint16 rtpPort = 0, tcpPort = 0;
        QString version = "";
        int serverInstanceID = 0;
        in >> rtpPort >> tcpPort >> version >> serverInstanceID;
        serverAddress = peerAddress;
        serverRTPListeningPort = rtpPort;
        serverName = peerName;

        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverRTPListeningPort, tcpPort, serverName, version, serverInstanceID);
        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRTPListeningPort:"<<serverRTPListeningPort << " serverTCPListeningPort:"<<tcpPort;
    }
    break;

    case quint8(MS::ServerOnlineStatusChanged):
    {
        quint8 online = 1;
        quint16 rtpPort = 0, tcpPort = 0;
        in >> online >> rtpPort >> tcpPort;
        serverAddress = online?peerAddress:QHostAddress::Null;
        serverRTPListeningPort = online?rtpPort:0;
        serverName = peerName;

        emit signalServerOnlineStatusChangedPacketReceived(online, serverAddress, serverRTPListeningPort, serverName);
        qDebug()<<"~~ServerOnlineStatusChanged"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverRTPListeningPort;

    }
    break;

    case quint8(MS::ClientInfoRequested):
    {
        QString assetNO = "";
        quint8 infoType = 0;
        in >> assetNO >> infoType;

        if(!assetNO.isEmpty()){
            if(assetNO != m_assetNO){
                qCritical()<<"ERROR! Computer Name Not Match!";
                return;
            }
        }

        emit signalClientInfoRequestedPacketReceived(socketID, assetNO, infoType);
        qDebug()<<"~~ClientInfoRequested";
    }
    break;

    case quint8(MS::AdminRequestRemoteConsole):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString assetNO = "", applicationPath = "", adminID = "";
        bool startProcess = true;
        in >> assetNO >> applicationPath >> adminID >> startProcess;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalAdminRequestRemoteConsolePacketReceived(assetNO, applicationPath, adminID, startProcess, peerAddress.toString(), peerPort);

        qDebug()<<"~~AdminRequestRemoteConsole";
    }
    break;
    //    case quint8(MS::ClientResponseRemoteConsole):
    //        break;
    case quint8(MS::RemoteConsoleCMDFromAdmin):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString assetNO = "", command = "";
        in >> assetNO >> command;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalRemoteConsoleCMDFromServerPacketReceived(assetNO, command, peerAddress.toString(), peerPort);
        qDebug()<<"~~RemoteConsoleCMDFromAdmin";
    }
    break;
    //    case quint8(MS::RemoteConsoleCMDResultFromClient):
    //        break;
    //    case quint8(MS::ClientResponseClientInfo):
    //        break;
    //    case quint8(MS::ClientRequestSoftwareVersion):
    //        break;
    case quint8(MS::ServerResponseSoftwareVersion):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString softwareName, version;
        in >> softwareName >> version;
        emit signalServerResponseSoftwareVersionPacketReceived(softwareName, softwareName);
        qDebug()<<"~~ServerResponseSoftwareVersion";
    }
    break;
    case quint8(MS::Announcement):
    {
        QString assetNO = "", userName = "", adminName = "", announcement = "";
        quint32 announcementID = 0;
        quint8 confirmationRequired = 1;
        int validityPeriod = 60;
        in >> assetNO >> userName >> adminName >> announcementID >> announcement >> confirmationRequired >> validityPeriod;

        if(assetNO != m_assetNO){
            return;
        }

        if(userName.trimmed().isEmpty()){
            foreach (SOCKETID sID, localUserSockets()) {
                sendServerAnnouncementPacket(sID, adminName, announcementID, announcement, confirmationRequired, validityPeriod);
            }
        }else{
            SOCKETID sID = socketIDOfUser(userName);
            if(INVALID_SOCK_ID == sID){
                qCritical()<<QString("ERROR! No online user named '%1'.").arg(userName);
                return;
            }
            sendServerAnnouncementPacket(sID, adminName, announcementID, announcement, confirmationRequired, validityPeriod);
        }


        //emit signalServerAnnouncementPacketReceived(groupName, computerName, announcementID, announcement, adminName, userName, (mustRead == quint8(0))?false:true);

        qDebug()<<"~~Announcement"<<"computerName:"<<assetNO<<" announcement:"<<announcement<<" userName:"<<userName<<" mustRead:"<<confirmationRequired;
        //qDebug()<<"~~Announcement";

    }
    break;
    case quint8(MS::Update):
        emit signalUpdateClientSoftwarePacketReceived();
        qDebug()<<"~~Update";
        break;
    case quint8(MS::AdminRequestSetupUSBSD):
    {
        quint8 usbSTORStatus = quint8(MS::USBSTOR_Unknown);
        bool temporarilyAllowed = true;
        QString adminName = "";

        in >> usbSTORStatus >> temporarilyAllowed >> adminName;

        emit signalSetupUSBSDPacketReceived(usbSTORStatus, temporarilyAllowed, adminName);
        qDebug()<<"~~AdminRequestSetupUSBSD";
    }
    break;

    case quint8(MS::ShowAdmin):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        bool show = false;
        in >> show;

        emit signalShowAdminPacketReceived(show);
        qDebug()<<"~~ShowAdmin";
    }
    break;
    case quint8(MS::ModifyAdminGroupUser):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString assetNO = "", userName = "", adminName = "";
        bool addToAdminGroup = false;
        in >> assetNO >> userName >> addToAdminGroup >> adminName;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalModifyAdminGroupUserPacketReceived(assetNO, userName, addToAdminGroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~ModifyAdminGroupUser";
    }
    break;

    case quint8(MS::ModifyAssetNO):
    {
        QString oldAssetNO = "", newAssetNO = "", adminName = "";
        in >> newAssetNO >> oldAssetNO >> adminName;

        if(oldAssetNO != m_assetNO){
            return;
        }

        emit signalModifyAssetNOPacketReceived(newAssetNO, adminName);
        qDebug()<<"~~ModifyAssetNO";
    }
    break;

    case quint8(MS::AssetNOModified):
    {
        //From Server
        QString newAssetNO = "", oldAssetNO = "", message = "";
        quint8 modified = false;
        in >> newAssetNO >> oldAssetNO >> modified >> message;

        if(oldAssetNO != m_assetNO){
            return;
        }

        emit signalAssetNOModifiedPacketReceived(newAssetNO, oldAssetNO, modified, message);
        qDebug()<<"~~AssetNOModified";
    }
    break;

    case quint8(MS::RenameComputer):
    {
        QString assetNO = "", newComputerName = "", adminName = "", domainAdminName = "", domainAdminPassword = "";
        in >> assetNO >> newComputerName >> adminName >> domainAdminName >> domainAdminPassword;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalRenameComputerPacketReceived(newComputerName, adminName, domainAdminName, domainAdminPassword);
        qDebug()<<"~~RenameComputer";
    }
    break;

    case quint8(MS::JoinOrUnjoinDomain):
    {
        QString assetNO = "", adminName = "", domainOrWorkgroupName = "", domainAdminName = "", domainAdminPassword = "";
        bool join = false;
        in >> assetNO >> adminName >> join >> domainOrWorkgroupName >> domainAdminName >> domainAdminPassword;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalJoinOrUnjoinDomainPacketReceived(adminName, join, domainOrWorkgroupName, domainAdminName, domainAdminPassword);
        qDebug()<<"~~JoinOrUnjoinDomain";
    }
    break;

    case quint8(MS::AdminRequestConnectionToClient):
    {

        QString adminComputerName = "", adminName = "" ;
        in >> adminComputerName >> adminName ;

//        if(computerName.toLower() != m_localComputerName){
//            //m_udtProtocol->closeSocket(socketID);
//            m_rtp->closeSocket(socketID);
//            return;
//        }

        emit signalAdminRequestConnectionToClientPacketReceived(socketID, adminComputerName, adminName);
        qDebug()<<"~~AdminRequestConnectionToClient";
    }
    break;
    case quint8(MS::AdminSearchClient):
    {

        QString computerName = "", userName = "", workgroup = "", macAddress = "", ipAddress = "", osVersion = "", adminName = "" ;
        in >> computerName >> userName >> workgroup >> macAddress >> ipAddress >> osVersion >> adminName;

        emit signalAdminSearchClientPacketReceived(peerAddress.toString(), peerPort, computerName, userName, workgroup, macAddress, ipAddress, osVersion, adminName);

        qDebug()<<"~~AdminSearchClient "<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort;
    }
    break;


    case quint8(MS::AdminRequestRemoteAssistance):
    {

        QString assetNO = "", adminName = "", userName = "";
        in >> assetNO >> adminName >> userName;

        if(assetNO != m_assetNO){
            return;
        }

        SOCKETID sID = socketIDOfUser(userName);
        if(INVALID_SOCK_ID != sID){
            sendRequestRemoteAssistancePacket(sID, peerAddress.toString(), peerPort, adminName);
        }

        qDebug()<<"~~AdminRequestRemoteAssistance";
    }
    break;
    case quint8(MS::UpdateMSWUserPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;
        emit signalAdminRequestUpdateMSUserPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~UpdateMSWUserPassword";
    }
    break;

    case quint8(MS::InformUserNewPassword):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString workgroup = "", adminName = "";
        in >> workgroup >> adminName;

        emit signalAdminRequestInformUserNewPasswordPacketReceived(workgroup, adminName, peerAddress.toString(), peerPort);

        qDebug()<<"~~InformUserNewPassword";


    }
    break;


    case quint8(MS::LocalUserOnlineStatusChanged):
    {
        QString userName = "";
        quint8 online = 0;
        in >> userName >> online;
        
        changeLocalUserOnlineStatus(socketID, online, userName);
        
//        emit signalLocalUserOnlineStatusChanged(socketID, userName, online);

        qDebug()<<"~~LocalUserOnlineStatusChanged"<<"Name:"<<userName<<" Port:"<<peerPort <<" Online:"<<online;
    }
    break;

    case quint8(MS::RequestTemperatures):
    {
        quint8 cpu = 1, harddisk = 0;
        in >> cpu >> harddisk;

        emit signalAdminRequestTemperatures(socketID, cpu, harddisk);

        qDebug()<<"~~Temperatures";

    }
    break;
    case quint8(MS::RequestScreenshot):
    {
        QString userName = "";
        quint16 adminListeningPort = 0;
        in >> userName >> adminListeningPort;

        requestScreenshot(socketID, userName, peerAddress.toString(), adminListeningPort);

        //emit signalAdminRequestScreenshot(socketID, userName, fullScreen);

        qDebug()<<"~~RequestScreenshot";

    }
    break;

    case quint8(MS::ReplyMessage):
    {
        //From local user
        quint32 originalMessageID;
        QString replyMessage;
        in >> originalMessageID >> replyMessage ;

        sendUserReplyMessagePacket(m_socketConnectedToAdmin, userNameOfSocket(socketID), originalMessageID, replyMessage);
        qDebug()<<"~~ReplyMessage";
    }
    break;

//    case quint8(MS::ResponseScreenshot):
//    {
//        //From local user
//        QByteArray screenshot;
//        in >> screenshot ;

//        sendClientResponseScreenshotPacket(m_socketConnectedToAdmin, userNameOfSocket(socketID), screenshot);
//        qDebug()<<"~~ResponseScreenshot";
//    }
//    break;

    case quint8(MS::RequestShutdown):
    {
        QString message = "";
        quint32 waitTime = 0;
        quint8 force = 1, reboot = 0 ;

        in >> message >> waitTime >> force >> reboot ;

        emit signalAdminRequestShutdownPacketReceived(socketID, message, waitTime, force, reboot );
        qDebug()<<"~~RequestShutdown";
    }
    break;

    case quint8(MS::RequestLockWindows):
    {
        QString userName = "";
        quint8 logoff = 0 ;

        in >> userName >> logoff ;

        emit signalAdminRequestLockWindowsPacketReceived(socketID, userName, logoff );
        qDebug()<<"~~RequestLockWindows";
    }
    break;

    case quint8(MS::RequestCreateOrModifyWinUser):
    {
        QByteArray userData;
        in >> userData ;

        emit signalAdminRequestCreateOrModifyWinUserPacketReceived(socketID, userData );
        qDebug()<<"~~RequestCreateOrModifyWinUser";
    }
    break;

    case quint8(MS::RequestDeleteUser):
    {
        QString userName = "";
        in >> userName ;

        emit signalAdminRequestDeleteUserPacketReceived(socketID, userName );
        qDebug()<<"~~RequestDeleteUser";
    }
    break;

    case quint8(MS::RequestChangeServiceConfig):
    {
        QString serviceName = "";
        quint8  startService = 0;
        quint64 startType = 0xFFFFFFFF;

        in >> serviceName >> startService >> startType ;

        emit signalAdminRequestChangeServiceConfigPacketReceived(socketID, serviceName, startService, startType);
        qDebug()<<"~~RequestChangeServiceConfig";
    }
    break;

    case quint8(MS::RequestChangeProcessMonitorInfo):
    {
        QByteArray localRules,  globalRules;
        quint8 enableProcMon = 0;
        quint8 enablePassthrough = 1;
        quint8 enableLogAllowedProcess = 0;
        quint8 enableLogBlockedProcess = 1;
        quint8 useGlobalRules = 1;
        QString assetNO = "";

        in >> localRules >> globalRules >> enableProcMon >> enablePassthrough >> enableLogAllowedProcess >> enableLogBlockedProcess >>useGlobalRules >>assetNO;

        if(assetNO != m_assetNO){
            return;
        }

        emit signalRequestChangeProcessMonitorInfoPacketReceived(socketID, localRules, globalRules, enableProcMon,enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess, useGlobalRules);
        qDebug()<<"~~RequestChangeProcessMonitorInfo";
    }
    break;

////////////////////////////////////////////
    case quint8(MS::RequestFileSystemInfo):
    {
        QString parentDirPath = "";
        in >> parentDirPath;

        emit signalFileSystemInfoRequested(socketID, parentDirPath);

        qDebug()<<"~~RequestFileSystemInfo";
    }
    break;
    case quint8(MS::RequestUploadFile):
    {
        QByteArray fileMD5Sum;
        QString fileName = "";
        quint64 size = 0;
        QString localFileSaveDir = "./";
        in >> fileMD5Sum >> fileName >> size >> localFileSaveDir ;

        emit signalAdminRequestUploadFile(socketID, fileMD5Sum, fileName, size, localFileSaveDir);

        qDebug()<<"~~RequestUploadFile";
    }
    break;
    case quint8(MS::RequestDownloadFile):
    {
        QString localBaseDir, fileName, remoteFileSaveDir;
        in >> localBaseDir >> fileName >> remoteFileSaveDir;

        emit signalAdminRequestDownloadFile(socketID, localBaseDir, fileName, remoteFileSaveDir);

        qDebug()<<"~~RequestDownloadFile";
    }
    break;
    case quint8(MS::RequestFileData):
    {
        QByteArray fileMD5;
        int startPieceIndex = 0, endPieceIndex = 0;
        in >> fileMD5 >> startPieceIndex >> endPieceIndex;

        emit signalFileDataRequested(socketID, fileMD5, startPieceIndex, endPieceIndex);

        qDebug()<<"~~RequestFileData";
    }
    break;
    case quint8(MS::FileData):
    {
        QByteArray fileMD5, data, sha1;;
        int pieceIndex = 0;


        in >> fileMD5 >> pieceIndex >> data >>sha1;

//        if(data.size() != size || sha1 != QCryptographicHash::hash(data, QCryptographicHash::Sha1)){
//            qCritical()<<"ERROR! Data Verification Failed!";
//            requestFileData(socketID, offset, size);
//            return;
//        }

        emit signalFileDataReceived(socketID, fileMD5, pieceIndex, data, sha1);

        //qDebug()<<"~~FileData";
    }
    break;
    case quint8(MS::FileTXStatusChanged):
    {
        QByteArray fileMD5;
        quint8 status;
        in >> fileMD5 >> status;

        emit signalFileTXStatusChanged(socketID, fileMD5, status);

        qDebug()<<"~~FileTXStatusChanged";
    }
    break;
    case quint8(MS::FileTXError):
    {
        QByteArray fileMD5;
        quint8 errorCode;
        QString message;
        in >> fileMD5 >> errorCode >> message;

        emit signalFileTXError(socketID, fileMD5, errorCode, message);

        qDebug()<<"~~FileTXStatusChanged";
    }
    break;





    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType//<<" Serial Number:"<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()<<":"<<peerPort
//                <<" (ClientPacketsParser)";

        break;

    }



}

void ClientPacketsParser::changeLocalUserOnlineStatus(SOCKETID userSocketID, bool online, const QString &userName){
    qDebug()<<"--ClientPacketsParser::changeLocalUserOnlineStatus(...)  userSocketID:"<<userSocketID<<" online:"<<online<<" userName"<<userName;

    QMutexLocker locker(&m_localUserSocketsHashMutex);

    QString name = userName;
    if(!online && name.isEmpty()){
        name = m_localUserSocketsHash.value(userSocketID);
    }

    if(INVALID_SOCK_ID != m_socketConnectedToAdmin && (!name.isEmpty()) ){
        sendLocalUserOnlineStatusChangedPacket(m_socketConnectedToAdmin, name, online);
    }

    if(online){
        m_localUserSocketsHash.insert(userSocketID, name);
    }else{
        m_localUserSocketsHash.remove(userSocketID);
    }

}

QString ClientPacketsParser::userNameOfSocket(SOCKETID socketID){
    QMutexLocker locker(&m_localUserSocketsHashMutex);

    if(!m_localUserSocketsHash.contains(socketID)){
        return "";
    }
    return m_localUserSocketsHash.value(socketID);
}

SOCKETID ClientPacketsParser::socketIDOfUser(const QString &userName){
    QMutexLocker locker(&m_localUserSocketsHashMutex);

    if(!m_localUserSocketsHash.values().contains(userName)){
        return INVALID_SOCK_ID;
    }
    return m_localUserSocketsHash.key(userName);
}

QList<SOCKETID> ClientPacketsParser::localUserSockets(){
    QMutexLocker locker(&m_localUserSocketsHashMutex);
    return m_localUserSocketsHash.keys();
}

void ClientPacketsParser::requestScreenshot(SOCKETID adminSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort){

    SOCKETID userSocketID = socketIDOfUser(userName);
    if(INVALID_SOCK_ID == userSocketID){
        sendClientMessagePacket(adminSocketID, "User not connected!", MS::MSG_Critical);
        return;
    }

    sendAdminRequestScreenshotPacket(userSocketID, adminAddress, adminPort);

}

void ClientPacketsParser::setSocketConnectedToAdmin(SOCKETID socketID, const QString &adminName){
    m_socketConnectedToAdmin = socketID;
    m_adminName = adminName;
}

void ClientPacketsParser::setAssetNO(const QString &assetNO){
    m_assetNO = assetNO;
}
























} //namespace HEHUI
