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




namespace HEHUI
{


ClientPacketsParser::ClientPacketsParser(const QString &assetNO, ClientResourcesManager *manager, QObject *parent)
    : QObject(parent), m_assetNO(assetNO), m_resourcesManager(manager)
{

    if(m_assetNO.trimmed().isEmpty()) {
        m_assetNO = QHostInfo::localHostName().toLower();
    }
    //m_localComputerName = QHostInfo::localHostName().toLower();
    Packet::setLocalID(m_assetNO);

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ClientPacketsParser::ClientPacketsParser(...)", "Invalid UDPServer!");
    connect(m_udpServer, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    Q_ASSERT(m_udtProtocol);
//    m_udtProtocol->startWaitingForIOInOneThread(10);
//    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
//    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);

    m_enetProtocol = m_rtp->getENETProtocol();
    Q_ASSERT(m_enetProtocol);
    connect(m_enetProtocol, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);


    serverAddress = QHostAddress::Null;
    serverRTPListeningPort = 0;
    serverName = "";

//    m_localUDTServerListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_rtp->getTCPServerPort();
    m_localENETListeningPort = m_rtp->getENETProtocolPort();


    m_socketConnectedToServer = INVALID_SOCK_ID;
    m_socketConnectedToAdmin = INVALID_SOCK_ID;

}

ClientPacketsParser::~ClientPacketsParser()
{
    // TODO Auto-generated destructor stub

    qDebug() << "ClientPacketsParser::~ClientPacketsParser() ";






}


void ClientPacketsParser::parseIncomingPacketData(const PacketBase &packet)
{
    //    qDebug()<<"----ClientPacketsParser::parseIncomingPacketData(Packet *packet)";


    //QByteArray packetBody = packet.getPacketBody();
    quint8 packetType = packet.getPacketType();
    QString peerID = packet.getPeerID();

    QHostAddress peerAddress = packet.getPeerHostAddress();
    quint16 peerPort = packet.getPeerHostPort();
    SOCKETID socketID = packet.getSocketID();

    switch(packetType) {

    case quint8(MS::CMD_ServerDiscovery): {
        ServerDiscoveryPacket p(packet);

        serverAddress = peerAddress;
        serverRTPListeningPort = p.rtpPort;
        serverName = peerID;

        emit signalServerDeclarePacketReceived(p);
        qDebug() << "~~CMD_ServerDiscovery" << " serverAddress:" << serverAddress.toString() << " servername:" << serverName << " serverRTPListeningPort:" << serverRTPListeningPort << " serverTCPListeningPort:" << p.tcpPort;
    }
    break;

    case quint8(MS::CMD_ClientInfo): {
        qDebug() << "~~CMD_ClientInfo";

        ClientInfoPacket p(packet);
        emit signalClientInfoRequestedPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_RemoteConsole): {
        qDebug() << "~~CMD_RemoteConsole";

        RemoteConsolePacket p(packet);
        emit signalRemoteConsolePacketReceived(p);
    }
    break;

    case quint8(MS::CMD_SystemInfoFromServer): {
        qDebug() << "SystemInfoFromServer";

        SystemInfoFromServerPacket p(packet);

        if(p.infoType == quint8(MS::SYSINFO_ANNOUNCEMENTS) || p.infoType == quint8(MS::SYSINFO_ANNOUNCEMENTREPLIES)) {
            if(p.extraInfo.isEmpty()) {
                foreach (SOCKETID sID, localUserSockets()) {
                    sendSystemInfoPacket(sID, p.extraInfo, p.data, p.infoType);
                }
            } else {
                SOCKETID sID = socketIDOfUser(p.extraInfo);
                if(INVALID_SOCK_ID == sID) {
                    qCritical() << QString("ERROR! No online user named '%1'.").arg(p.extraInfo);
                    return;
                }
                sendSystemInfoPacket(sID, p.extraInfo, p.data, p.infoType);
            }

            return;
        }

        emit signalSystemInfoFromServerReceived(p);
    }
    break;

    case quint8(MS::CMD_USBDev): {
        qDebug() << "~~CMD_USBDev";
        USBDevPacket p(packet);
        emit signalSetupUSBSDPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_ModifyAssetNO): {
        qDebug() << "~~CMD_ModifyAssetNO";

        ModifyAssetNOPacket p(packet);
        emit signalModifyAssetNOPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_RenameComputer): {
        qDebug() << "~~CMD_RenameComputer";

        RenameComputerPacket p(packet);
        if(p.assetNO != m_assetNO) {
            return;
        }

        emit signalRenameComputerPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_JoinOrUnjoinDomain): {
        qDebug() << "~~CMD_JoinOrUnjoinDomain";

        JoinOrUnjoinDomainPacket p(packet);
        if(p.assetNO != m_assetNO) {
            return;
        }

        emit signalJoinOrUnjoinDomainPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_AdminConnectionToClient): {
        qDebug() << "~~CMD_AdminConnectionToClient";

        AdminConnectionToClientPacket p(packet);
        emit signalAdminRequestConnectionToClientPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_AdminSearchClient): {
        qDebug() << "~~CMD_AdminSearchClient ";
        AdminSearchClientPacket p(packet);

        emit signalAdminSearchClientPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_LocalUserOnlineStatusChanged): {
        qDebug() << "~~CMD_LocalUserOnlineStatusChanged";

        LocalUserOnlineStatusChangedPacket p(packet);
        changeLocalUserOnlineStatus(socketID, p.online, p.userName);
    }
    break;

    case quint8(MS::CMD_Temperatures): {
        qDebug() << "~~CMD_Temperatures";

        TemperaturesPacket p(packet);
        emit signalAdminRequestTemperatures(p);
    }
    break;

    case quint8(MS::CMD_Screenshot): {
        qDebug() << "~~CMD_Screenshot";
        ScreenshotPacket p(packet);
        requestScreenshot(socketID, p.ScreenshotRequest.userName, peerAddress.toString(), p.ScreenshotRequest.adminListeningPort);
    }
    break;

    case quint8(MS::CMD_Announcement): {
        AnnouncementPacket p(packet);
        if(p.InfoType == AnnouncementPacket::ANNOUNCEMENT_REPLY) {
            sendUserReplyMessagePacket(m_socketConnectedToServer, p.ReplyInfo.announcementID, userNameOfSocket(socketID), p.ReplyInfo.receiver, "", p.ReplyInfo.replyMessage);
        }

    }
    break;

    case quint8(MS::CMD_Shutdown): {
        qDebug() << "~~CMD_Shutdown";
        ShutdownPacket p(packet);
        emit signalAdminRequestShutdownPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_LockWindows): {
        qDebug() << "~~CMD_LockWindows";
        LockWindowsPacket p(packet);
        emit signalAdminRequestLockWindowsPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_WinUser): {
        qDebug() << "~~CMD_WinUser";
        WinUserPacket p(packet);
        emit signalWinUserPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_ServiceConfig): {
        qDebug() << "~~CMD_ServiceConfig";
        ServiceConfigPacket p(packet);
        emit signalAdminRequestChangeServiceConfigPacketReceived(p);
    }
    break;

    case quint8(MS::CMD_ProcessMonitorInfo): {
        qDebug() << "~~CMD_ProcessMonitorInfo";
        ProcessMonitorInfoPacket p(packet);
        if(p.assetNO != m_assetNO) {
            return;
        }

        emit signalRequestChangeProcessMonitorInfoPacketReceived(p);
    }
    break;


////////////////////////////////////////////
    case quint8(MS::CMD_FileTransfer): {
        //qDebug()<<"~~CMD_FileTransfer";

        FileTransferPacket p(packet);
        emit signalFileTransferPacketReceived(p);
    }
    break;







    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType//<<" Serial Number:"<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()<<":"<<peerPort
//                <<" (ClientPacketsParser)";

        break;

    }



}

void ClientPacketsParser::changeLocalUserOnlineStatus(SOCKETID userSocketID, bool online, const QString &userName)
{
    qDebug() << "--ClientPacketsParser::changeLocalUserOnlineStatus(...)  userSocketID:" << userSocketID << " online:" << online << " userName" << userName;

    QMutexLocker locker(&m_localUserSocketsHashMutex);

    QString name = userName;
    if(!online && name.isEmpty()) {
        name = m_localUserSocketsHash.value(userSocketID);
    }

    if(INVALID_SOCK_ID != m_socketConnectedToAdmin && (!name.isEmpty()) ) {
        sendLocalUserOnlineStatusChangedPacket(m_socketConnectedToAdmin, name, online);
    }

    if(online) {
        m_localUserSocketsHash.insert(userSocketID, name);
        if(m_socketConnectedToServer != INVALID_SOCK_ID) {
            sendRequestAnnouncementsPacket(m_socketConnectedToServer, name);
        }
    } else {
        m_localUserSocketsHash.remove(userSocketID);
    }

}

QString ClientPacketsParser::userNameOfSocket(SOCKETID socketID)
{
    QMutexLocker locker(&m_localUserSocketsHashMutex);

    if(!m_localUserSocketsHash.contains(socketID)) {
        return "";
    }
    return m_localUserSocketsHash.value(socketID);
}

SOCKETID ClientPacketsParser::socketIDOfUser(const QString &userName)
{
    QMutexLocker locker(&m_localUserSocketsHashMutex);

    if(!m_localUserSocketsHash.values().contains(userName)) {
        return INVALID_SOCK_ID;
    }
    return m_localUserSocketsHash.key(userName);
}

QList<SOCKETID> ClientPacketsParser::localUserSockets()
{
    QMutexLocker locker(&m_localUserSocketsHashMutex);
    return m_localUserSocketsHash.keys();
}

void ClientPacketsParser::requestScreenshot(SOCKETID adminSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort)
{

    SOCKETID userSocketID = socketIDOfUser(userName);
    if(INVALID_SOCK_ID == userSocketID) {
        sendClientMessagePacket(adminSocketID, "User not connected!", MS::MSG_Critical);
        return;
    }

    sendAdminRequestScreenshotPacket(userSocketID, userName, adminPort);

}

void ClientPacketsParser::setSocketConnectedToServer(SOCKETID serverSocketID)
{
    m_socketConnectedToServer = serverSocketID;
}

void ClientPacketsParser::setSocketConnectedToAdmin(SOCKETID socketID, const QString &adminName)
{
    m_socketConnectedToAdmin = socketID;
    m_adminName = adminName;
}

void ClientPacketsParser::setAssetNO(const QString &assetNO)
{
    m_assetNO = assetNO;
}
























} //namespace HEHUI
