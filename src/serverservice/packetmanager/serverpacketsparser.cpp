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
    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    Q_ASSERT(m_udtProtocol);
//    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


    m_enetProtocol = m_rtp->getENETProtocol();
    Q_ASSERT(m_enetProtocol);
    connect(m_enetProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


//    localUDTListeningAddress = m_udtProtocol->getUDTListeningAddress();
//    localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
    localRTPListeningPort = m_resourcesManager->getRTPPort();

    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();

    m_serverName = QHostInfo::localHostName().toLower();

    localIPMCListeningPort = m_udpServer->localPort();

}

ServerPacketsParser::~ServerPacketsParser() {
    // TODO Auto-generated destructor stub





}



void ServerPacketsParser::parseIncomingPacketData(Packet *packet){

    //    qDebug()<<"----ServerPacketsParser::parseIncomingPacketData(Packet *packet)";


    
    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();
//    quint16 packetSerialNumber = packet->getPacketSerialNumber();
    quint8 packetType = packet->getPacketType();
    SOCKETID socketID = packet->getSocketID();
//    qDebug()<<"--ServerPacketsParser::parseIncomingPacketData(...) "<<" peerID:"<<peerID<<" peerAddress:"<<peerAddress<<" peerPort:"<<peerPort<<" packetSerialNumber:"<<packetSerialNumber<<" packetType:"<<packetType;

    switch(packetType){
//    case quint8(HEHUI::HeartbeatPacket):
//    {
//        emit signalHeartbeatPacketReceived(packet->getPeerHostAddress().toString(), peerID);
//        qDebug()<<"~~HeartbeatPacket--"<<" peerID:"<<peerID;
//    }
//    break;
//    case quint8(HEHUI::ConfirmationOfReceiptPacket):
//    {
//        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
//        in >> packetSerialNumber1 >> packetSerialNumber2;
//        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
//        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
//        qDebug()<<"~~ConfirmationOfReceiptPacket--"<<packetSerialNumber1<<" "<<packetSerialNumber2;
//    }
//    break;
    case quint8(MS::ClientLookForServer):
    {

        quint16 peerUDPListeningPort;
        QString version = "";
        in >> peerUDPListeningPort >> version;

        sendServerDeclarePacket(peerAddress, peerUDPListeningPort);
        //emit signalClientLookForServerPacketReceived(peerAddress, peerPort, peerName);
        qWarning()<<"~~ClientLookForServer--"<<" peerAddress:"<<peerAddress.toString()<<"   peerPort:"<<peerPort<<" peerUDPListeningPort:"<<peerUDPListeningPort <<" Version:"<<version;
    }
    break;
    case quint8(MS::ClientOnlineStatusChanged):
    {
        quint8 online;
        in >> online;
        emit signalClientOnlineStatusChanged(socketID, peerID, online, peerAddress.toString(), peerPort);

        qDebug()<<"~~ClientOnlineStatusChanged--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<online;
    }
    break;

    case quint8(MS::AdminLogin):
    {
        QString adminComputerName = "", adminName = "", password = "";
        in >> adminComputerName >> adminName >> password;
        emit signalAdminLogin(socketID, adminName, password, adminComputerName, peerAddress.toString());

        qDebug()<<"~~AdminLogin--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<adminComputerName <<" adminName:"<<adminName;
    }
    break;

    case quint8(MS::RequestSystemAlarms):
    {
        QString assetNO = "", type = "-1", acknowledged = "-1", startTime = "", endTime = "";
        in >> assetNO >> type >> acknowledged >> startTime >> endTime;
        emit signalSystemAlarmsRequested(socketID, assetNO, type, acknowledged, startTime, endTime);

        qDebug()<<"~~RequestSystemAlarms--" <<" assetNO:"<<assetNO;
    }
    break;

    case quint8(MS::AcknowledgeSystemAlarms):
    {
        QString adminID = "", alarms = "";
        quint8 deleteAlarms = false;
        in >> adminID >> alarms >> deleteAlarms;
        emit signalAcknowledgeSystemAlarmsPacketReceived(socketID, adminID, alarms, deleteAlarms);

        qDebug()<<"~~AcknowledgeSystemAlarms--" <<" adminID:"<<adminID;
    }
    break;

    case quint8(MS::RequestAnnouncement):
    {
        QString id = "0", keyword = "", validity = "-1",  assetNO = "", userName = "", target = "-1", startTime = "", endTime = "";
        in >> id >> keyword >> validity >> assetNO >> userName >> target >> startTime >> endTime;
        emit signalAnnouncementsRequested(socketID, id, keyword, validity, assetNO, userName, target, startTime, endTime);

        qDebug()<<"~~RequestAnnouncement--";
    }
    break;

    case quint8(MS::Announcement):
    {
        unsigned int localTempID = 0;
        QString adminName = "";
        quint8 type = quint8(MS::ANNOUNCEMENT_NORMAL);
        QString content = "";
        bool confirmationRequired = true;
        int validityPeriod = 60;
        quint8 targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
        QString targets = "";

        in >> localTempID >> adminName >> type >> content >> confirmationRequired >> validityPeriod >> targetType >> targets;
        emit signalAnnouncementPacketReceived(socketID, localTempID, adminName, type, content, confirmationRequired, validityPeriod, targetType, targets);

        qDebug()<<"~~Announcement--";
    }
    break;

    case quint8(MS::UpdateAnnouncement):
    {
        QString adminName = "";
        unsigned int announcementID = 0;
        quint8 targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
        quint8 active = 1;
        QString addedTargets = "", deletedTargets = "";

        in >> adminName >> announcementID >> targetType >> active >> addedTargets >> deletedTargets;
        emit signalUpdateAnnouncementRequested(socketID, adminName, announcementID, targetType, active, addedTargets, deletedTargets);

        qDebug()<<"~~UpdateAnnouncement--";
    }
    break;

    case quint8(MS::RequestAnnouncementTargets):
    {
        QString announcementID = "0";
        in >> announcementID ;
        emit signalAnnouncementTargetsRequested(socketID, announcementID);

        qDebug()<<"~~RequestAnnouncementTargets--";
    }
    break;

    case quint8(MS::ReplyMessage):
    {
        QString announcementID = "", sender = "", receiver = "", replyMessage = "";
        in >> announcementID >> sender >> receiver >> replyMessage ;

        emit signalReplyMessagePacketReceived(socketID, announcementID, sender, receiver, replyMessage);
        qDebug()<<"~~ReplyMessage--";
    }
    break;

    case quint8(MS::AdminOnlineStatusChanged):
    {
        QString peerComputerName = "", adminName = "";
        quint8 online = 0;
        in >> peerComputerName >> adminName >> online;
        emit signalAdminOnlineStatusChanged(socketID, peerComputerName, adminName, online);

        qDebug()<<"~~AdminOnlineStatusChanged--"<<" peerAddress:"<<peerAddress<<"   peerName:"<<peerComputerName <<" adminName:"<<adminName;
    }
    break;

    case quint8(MS::ClientInfo):
    {
        QByteArray systemInfo;
        quint8 infoType = 0;
        in >> systemInfo >> infoType;
        emit signalClientInfoPacketReceived(peerID, systemInfo, infoType);
        qDebug()<<"~~ClientInfo";

    }
    break;

    case quint8(MS::ClientRequestSoftwareVersion):
    {
        QString softwareName;
        in >> softwareName;
        emit signalClientRequestSoftwareVersionPacketReceived(softwareName);
        qDebug()<<"~~ClientRequestSoftwareVersion";
    }
    break;
    //    case quint8(MS::ServerResponseSoftwareVersion):
    //        break;
    //    case quint8(MS::ServerAnnouncement):
    //        break;
    case quint8(MS::ClientLog):
    {
//        sendConfirmationOfReceiptPacket(peerAddress, peerPort, packetSerialNumber, peerID);

        QString log = "", clientTime = "";
        quint8 logType = 0;
        in >> logType >> log >> clientTime;
        emit signalClientLogReceived(peerID, packet->getPeerHostAddress().toString(), logType, log, clientTime);
        qDebug()<<"~~ClientLog";
    }
    break;

    case quint8(MS::ModifyAssetNO):
    {
        QString newAssetNO = "", adminName = "";
        in >> newAssetNO >> adminName;

        emit signalModifyAssetNOPacketReceived(socketID, newAssetNO, peerID, adminName);
        qDebug()<<"~~ModifyAssetNO";
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

        emit signalRequestChangeProcessMonitorInfoPacketReceived(socketID, localRules, globalRules, enableProcMon,enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess, useGlobalRules, assetNO);
        qDebug()<<"~~RequestChangeProcessMonitorInfo";
    }
    break;

    case quint8(MS::ClientInfoRequested):
    {
        QString assetNO = "";
        quint8 infoType = 0;
        in >> assetNO >> infoType;

        emit signalClientInfoRequestedPacketReceived(socketID, assetNO, infoType);
        qDebug()<<"~~ClientInfoRequested";
    }
    break;

    case quint8(MS::UpdateSysAdminInfo):
    {
        QString sysAdminID = "";
        QByteArray infoData;
        quint8 deleteAdmin = 0;

        in >> sysAdminID >> infoData >> deleteAdmin;

        emit signalUpdateSysAdminInfoPacketReceived(socketID, sysAdminID, infoData, deleteAdmin);
        qDebug()<<"~~UpdateSysAdminInfo";
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
