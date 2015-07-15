/*
 ****************************************************************************
 * serverpacketsparser.cpp
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
 * Last Modified On: 2010-9-3
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include <QDebug>

#include "serverpacketsparser.h"


//#ifdef Q_CC_MSVC
//#include <windows.h>
//#include "HHSharedSystemUtilities/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif



namespace HEHUI {


ServerPacketsParser::ServerPacketsParser(ResourcesManagerInstance *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ServerPacketsParser::ServerPacketsParser(...)", "Invalid UDPServer!");
    connect(m_udpServer, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);

    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    Q_ASSERT(m_udtProtocol);
//    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);


    m_enetProtocol = m_rtp->getENETProtocol();
    Q_ASSERT(m_enetProtocol);
    connect(m_enetProtocol, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);


//    localUDTListeningAddress = m_udtProtocol->getUDTListeningAddress();
//    localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
    localRTPListeningPort = m_resourcesManager->getRTPPort();

    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();

    m_serverName = QHostInfo::localHostName().toLower();
    Packet::setLocalID(m_serverName);


    localIPMCListeningPort = m_udpServer->localPort();

}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub





}



void ServerPacketsParser::parseIncomingPacketData(const PacketBase &packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";

    //QByteArray packetBody = packet.getPacketBody();
    quint8 packetType = packet.getPacketType();
    QString peerID = packet.getPeerID();
    QHostAddress peerAddress = packet.getPeerHostAddress();
    quint16 peerPort = packet.getPeerHostPort();
    SOCKETID socketID = packet.getSocketID();
    
//    QByteArray packetData = packet->getPacketData();
//    QDataStream in(&packetData, QIODevice::ReadOnly);
//    in.setVersion(QDataStream::Qt_4_8);

//    QString peerID = "";
//    in >> peerID;

//    QHostAddress peerAddress = packet.getPeerHostAddress();
//    quint16 peerPort = packet.getPeerHostPort();
//    quint8 packetType = packet.getPacketType();
//    SOCKETID socketID = packet.getSocketID();
//    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){

    case quint8(MS::CMD_ServerDiscovery):
    {
        ServerDiscoveryPacket p(packet);
        if(p.responseFromServer){return;}
        quint16 udpPort= p.udpPort;
        if(!udpPort){
            udpPort = peerPort;
        }
        sendServerDeclarePacket(peerAddress, udpPort);
        qWarning()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort <<" Version:"<<p.version << " peerID:" << peerID;
    }
    break;

    case quint8(MS::CMD_AdminLogin):
    {
        qDebug()<<"~~CMD_AdminLogin";

        AdminLoginPacket p(packet);
        emit signalAdminLogin(socketID, p.LoginInfo.adminID, p.LoginInfo.password, p.LoginInfo.computerName, peerAddress.toString());
    }
    break;

    case quint8(MS::CMD_SystemAlarms):
    {
        qDebug()<<"~~CMD_SystemAlarms";

        SystemAlarmsPacket p(packet);
        switch (p.InfoType) {
        case SystemAlarmsPacket::SYSTEMALARMS_QUERY:
        {
            emit signalSystemAlarmsRequested(socketID, p.QueryInfo.assetNO, p.QueryInfo.type, p.QueryInfo.acknowledged, p.QueryInfo.startTime, p.QueryInfo.endTime);
        }
            break;

        case SystemAlarmsPacket::SYSTEMALARMS_ACK:
        {
            emit signalAcknowledgeSystemAlarmsPacketReceived(socketID, p.ACKInfo.alarms, p.ACKInfo.deleteAlarms);
        }
            break;

        default:
            break;
        }

    }
    break;

    case quint8(MS::CMD_Announcement):
    {
        qDebug()<<"~~CMD_Announcement";

        AnnouncementPacket p(packet);
        switch (p.InfoType) {
        case AnnouncementPacket::ANNOUNCEMENT_QUERY :
        {
            emit signalAnnouncementsRequested(socketID, p.QueryInfo.announcementID, p.QueryInfo.keyword, p.QueryInfo.validity, p.QueryInfo.assetNO, p.QueryInfo.userName, p.QueryInfo.target, p.QueryInfo.startTime, p.QueryInfo.endTime);
        }
            break;

        case AnnouncementPacket::ANNOUNCEMENT_CREATE :
        {
            emit signalCreateAnnouncementPacketReceived(socketID, p.JobID, p.CreateInfo.localTempID, p.CreateInfo.adminID, p.CreateInfo.type, p.CreateInfo.content, p.CreateInfo.confirmationRequired, p.CreateInfo.validityPeriod, p.CreateInfo.targetType, p.CreateInfo.targets);
        }
            break;

        case AnnouncementPacket::ANNOUNCEMENT_UPDATE :
        {
            emit signalUpdateAnnouncementRequested(socketID, p.JobID, p.UpdateInfo.adminName, p.UpdateInfo.announcementID, p.UpdateInfo.targetType, p.UpdateInfo.active, p.UpdateInfo.addedTargets, p.UpdateInfo.deletedTargets);
        }
            break;

        case AnnouncementPacket::ANNOUNCEMENT_REPLY :
        {
            emit signalReplyMessagePacketReceived(socketID, peerID, p.ReplyInfo.announcementID, p.ReplyInfo.sender, p.ReplyInfo.receiver, p.ReplyInfo.receiversAssetNO, p.ReplyInfo.replyMessage);
        }
            break;
        case AnnouncementPacket::ANNOUNCEMENT_QUERY_TARGETS :
        {
            emit signalAnnouncementTargetsRequested(socketID, p.QueryTargetsInfo.announcementID);
        }
            break;

        default:
            break;
        }

    }
    break;

    case quint8(MS::CMD_ClientInfo):
    {
        qDebug()<<"~~CMD_ClientInfo";

        ClientInfoPacket p(packet);
        if(p.IsRequest){
            emit signalClientInfoRequestedPacketReceived(socketID, p.assetNO, p.infoType);
        }else{
            emit signalClientInfoPacketReceived(peerID, p.data, p.infoType);
        }
    }
    break;

    case quint8(MS::CMD_ClientLog):
    {
        qDebug()<<"~~CMD_ClientLog";

        ClientLogPacket p(packet);
        emit signalClientLogReceived(peerID, peerAddress.toString(), p.logType, p.log);
    }
    break;

    case quint8(MS::CMD_ModifyAssetNO):
    {
        qDebug()<<"~~CMD_ModifyAssetNO";

        ModifyAssetNOPacket p(packet);
        emit signalModifyAssetNOPacketReceived(socketID, p.newAssetNO, peerID, p.adminID);
    }
    break;

    case quint8(MS::CMD_ProcessMonitorInfo):
    {
        qDebug()<<"~~CMD_ProcessMonitorInfo";

        ProcessMonitorInfoPacket p(packet);
        emit signalRequestChangeProcessMonitorInfoPacketReceived(socketID, p.localRules, p.globalRules, p.enableProcMon, p.enablePassthrough, p.enableLogAllowedProcess, p.enableLogBlockedProcess, p.useGlobalRules, p.assetNO);
    }
    break;

    case quint8(MS::CMD_SysAdminInfo):
    {
        qDebug()<<"~~CMD_SysAdminInfo";

        SysAdminInfoPacket p(packet);
        emit signalUpdateSysAdminInfoPacketReceived(socketID, p.adminID, p.data, p.deleteAdmin);
    }
    break;



    default:
//        qWarning()<<"Unknown Packet Type:"<<packetType
//                 //<<"    Serial Number: "<<packetSerialNumber
//                <<" From:"<<peerAddress.toString()
//               <<":"<<peerPort;
        break;

    }



}































} //namespace HEHUI
