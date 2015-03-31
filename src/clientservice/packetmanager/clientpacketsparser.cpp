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


ClientPacketsParser::ClientPacketsParser(ClientResourcesManager *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

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
    serverUDTListeningPort = 0;
    serverName = "";

//    m_localUDTServerListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_rtp->getTCPServerPort();
    m_localENETListeningPort = m_rtp->getENETProtocolPort();

    m_localComputerName = QHostInfo::localHostName().toLower();

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

        QString address = "";
        quint16 udtPort = 0, tcpPort = 0;
        QString version = "";
        int serverInstanceID = 0;
        in >> address >> udtPort >> tcpPort >> version >> serverInstanceID;
        serverAddress = peerAddress;
        serverUDTListeningPort = udtPort;
        serverName = peerName;

        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverUDTListeningPort, tcpPort, serverName, version, serverInstanceID);
        qDebug()<<"~~ServerDeclare"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverUDTListeningPort << " serverTCPListeningPort:"<<tcpPort;
    }
    break;
    case quint8(MS::ServerOnline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = peerAddress;
        serverUDTListeningPort = port;
        serverName = peerName;


        emit signalServerOnlinePacketReceived(serverAddress, serverUDTListeningPort, serverName);
        qDebug()<<"~~ServerOnline"<<" serverAddress:"<<serverAddress.toString()<<" servername:"<<serverName <<" serverRUDPListeningPort:"<<serverUDTListeningPort;

    }
    break;
    case quint8(MS::ServerOffline):
    {
        QString address = "";
        quint16 port = 0;
        in >> address >> port;
        serverAddress = QHostAddress::Null;
        serverUDTListeningPort = 0;
        serverName = "";

        //emit signalServerOfflinePacketReceived(serverAddress, serverRUDPListeningPort, serverName);
        qDebug()<<"~~ServerOffline"<<" peerAddress:"<<peerAddress.toString()<<" servername:"<<peerName <<" peerPort:"<<peerPort;
    }
    break;
    case quint8(MS::ClientInfoRequested):
    {
        QString computerName = "";
        quint8 infoType = 0;
        in >> computerName >> infoType;

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                qCritical()<<"ERROR! Computer Name Not Match!";
                return;
            }
        }

        emit signalClientInfoRequestedPacketReceived(socketID, computerName, infoType);
        qDebug()<<"~~ClientInfoRequested";
    }
    break;
    case quint8(MS::AdminRequestRemoteConsole):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", applicationPath = "", adminID = "";
        bool startProcess = true;
        in >> computerName >> applicationPath >> adminID >> startProcess;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalAdminRequestRemoteConsolePacketReceived(computerName, applicationPath, adminID, startProcess, peerAddress.toString(), peerPort);

        qDebug()<<"~~AdminRequestRemoteConsole";
    }
    break;
    //    case quint8(MS::ClientResponseRemoteConsole):
    //        break;
    case quint8(MS::RemoteConsoleCMDFromAdmin):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString computerName = "", command = "";
        in >> computerName >> command;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalRemoteConsoleCMDFromServerPacketReceived(computerName, command, peerAddress.toString(), peerPort);
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
        QString computerName = "", userName = "", adminName = "", announcement = "";
        quint32 announcementID = 0;
        quint8 confirmationRequired = 1;
        int validityPeriod = 60;
        in >> computerName >> userName >> adminName >> announcementID >> announcement >> confirmationRequired >> validityPeriod;


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

        qDebug()<<"~~Announcement"<<"computerName:"<<computerName<<" announcement:"<<announcement<<" userName:"<<userName<<" mustRead:"<<confirmationRequired;
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
    case quint8(MS::AdminRequestSetupProgrames):
    {

//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerName);

        QString adminName = "";
        bool enable = false;
        bool temporarilyAllowed = true;
        in >> enable >> temporarilyAllowed >> adminName;

        emit signalSetupProgramesPacketReceived(enable, temporarilyAllowed, adminName);
        qDebug()<<"~~SetupProgrames";
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

        QString computerName = "", userName = "", adminName = "";
        bool addToAdminGroup = false;
        in >> computerName >> userName >> addToAdminGroup >> adminName;

        if(computerName.toLower() != m_localComputerName){
            return;
        }

        emit signalModifyAdminGroupUserPacketReceived(computerName, userName, addToAdminGroup, adminName, peerAddress.toString(), peerPort);
        qDebug()<<"~~ModifyAdminGroupUser";
    }
    break;
    case quint8(MS::RenameComputer):
    {
        QString oldComputerName = "", newComputerName = "", adminName = "", domainAdminName = "", domainAdminPassword = "";
        in >> oldComputerName >> newComputerName >> adminName >> domainAdminName >> domainAdminPassword;

        if(oldComputerName.toLower() != m_localComputerName){
            return;
        }

        emit signalRenameComputerPacketReceived(newComputerName, adminName, domainAdminName, domainAdminPassword);
        qDebug()<<"~~RenameComputer";
    }
    break;
    case quint8(MS::JoinOrUnjoinDomain):
    {
        QString computerName = "", adminName = "", domainOrWorkgroupName = "", domainAdminName = "", domainAdminPassword = "";
        bool join = false;
        in >> computerName >> adminName >> join >> domainOrWorkgroupName >> domainAdminName >> domainAdminPassword;

        if(computerName.toLower() != m_localComputerName){
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

        if(!computerName.isEmpty()){
            if(computerName.toLower() != m_localComputerName){
                return;
            }
        }

        emit signalAdminSearchClientPacketReceived(peerAddress.toString(), peerPort, computerName, userName, workgroup, macAddress, ipAddress, osVersion, adminName);

        qDebug()<<"~~AdminSearchClient "<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort;
    }
    break;


    case quint8(MS::AdminRequestRemoteAssistance):
    {

        QString computerName = "", adminName = "", userName = "";
        in >> computerName >> adminName >> userName;
        //emit signalAdminRequestRemoteAssistancePacketReceived(peerName, adminName, peerAddress.toString(), peerPort);

        if(computerName.toLower() != m_localComputerName){
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
        quint8 fullScreen = 1;
        in >> userName >> fullScreen;

        requestScreenshot(socketID, userName);

        //emit signalAdminRequestScreenshot(socketID, userName, fullScreen);s

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

    case quint8(MS::ResponseScreenshot):
    {
        //From local user
        QByteArray screenshot;
        in >> screenshot ;

        sendClientResponseScreenshotPacket(m_socketConnectedToAdmin, userNameOfSocket(socketID), screenshot);
        qDebug()<<"~~ResponseScreenshot";
    }
    break;

    case quint8(MS::RequestShutdown):
    {
        QString message = "";
        quint32 waitTime = 0;
        quint8 force = 1, reboot = 0 ;

        in >> message >> waitTime >> force >> reboot ;

        signalAdminRequestShutdownPacketReceived(socketID, message, waitTime, force, reboot );
        qDebug()<<"~~RequestShutdown";
    }
    break;

    case quint8(MS::RequestLockWindows):
    {
        QString userName = "";
        quint8 logoff = 0 ;

        in >> userName >> logoff ;

        signalAdminRequestLockWindowsPacketReceived(socketID, userName, logoff );
        qDebug()<<"~~RequestLockWindows";
    }
    break;

    case quint8(MS::RequestCreateOrModifyWinUser):
    {
        QByteArray userData;
        in >> userData ;

        signalAdminRequestCreateOrModifyWinUserPacketReceived(socketID, userData );
        qDebug()<<"~~RequestCreateOrModifyWinUser";
    }
    break;

    case quint8(MS::RequestDeleteUser):
    {
        QString userName = "";
        in >> userName ;

        signalAdminRequestDeleteUserPacketReceived(socketID, userName );
        qDebug()<<"~~RequestDeleteUser";
    }
    break;

    case quint8(MS::RequestChangeServiceConfig):
    {
        QString serviceName = "";
        quint8  startService = 0;
        quint64 startType = 0xFFFFFFFF;

        in >> serviceName >> startService >> startType ;

        signalAdminRequestChangeServiceConfigPacketReceived(socketID, serviceName, startService, startType);
        qDebug()<<"~~RequestChangeServiceConfig";
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

void ClientPacketsParser::requestScreenshot(SOCKETID adminSocketID, const QString &userName){

    SOCKETID userSocketID = socketIDOfUser(userName);
    if(INVALID_SOCK_ID == userSocketID){
        sendClientMessagePacket(adminSocketID, "User not connected!", MS::MSG_Critical);
        return;
    }

    sendAdminRequestScreenshotPacket(userSocketID);

}

void ClientPacketsParser::setSocketConnectedToAdmin(SOCKETID socketID){
    m_socketConnectedToAdmin = socketID;
}
























} //namespace HEHUI
