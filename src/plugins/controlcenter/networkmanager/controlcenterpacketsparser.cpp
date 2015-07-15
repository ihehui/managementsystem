/*
 ****************************************************************************
 * controlcenterpacketsparser.cpp
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


#include <QHostInfo>
#include <QDebug>

#include "controlcenterpacketsparser.h"

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


ControlCenterPacketsParser::ControlCenterPacketsParser(ResourcesManagerInstance *manager, QObject *parent)
    :QObject(parent), m_resourcesManager(manager)
{

    Q_ASSERT(m_resourcesManager);

    m_udpServer = m_resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "ControlCenterPacketsParser::ControlCenterPacketsParser(...)", "Invalid UDPServer!");
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



    ipmcGroupAddress = QString(IP_MULTICAST_GROUP_ADDRESS);
    ipmcListeningPort = quint16(IP_MULTICAST_GROUP_PORT);


    //    localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
    m_localTCPServerListeningPort = m_tcpServer->getTCPServerListeningPort();
    m_localENETListeningPort = m_rtp->getENETProtocolPort();


    m_localComputerName = QHostInfo::localHostName().toLower();
    Packet::setLocalID(m_localComputerName);

}

ControlCenterPacketsParser::~ControlCenterPacketsParser() {
    // TODO Auto-generated destructor stub
    qDebug()<<"~ControlCenterPacketsParser()";

    disconnect();

}


void ControlCenterPacketsParser::parseIncomingPacketData(const PacketBase &packet){
    //qDebug()<<"----ControlCenterPacketsParser::parseIncomingPacketData(Packet *packet)";


    //QByteArray packetBody = packet.getPacketBody();
    quint8 packetType = packet.getPacketType();
    QString peerID = packet.getPeerID();

    QHostAddress peerAddress = packet.getPeerHostAddress();
    quint16 peerPort = packet.getPeerHostPort();
    SOCKETID socketID = packet.getSocketID();

    switch(packetType){

    case quint8(MS::CMD_ServerDiscovery):
    {
        qDebug()<<"~~CMD_ServerDiscovery";

        ServerDiscoveryPacket p(packet);

        serverAddress = peerAddress;
        serverRTPListeningPort = p.rtpPort;
        serverName = peerID;
        emit signalServerDeclarePacketReceived(serverAddress.toString(), serverRTPListeningPort, p.tcpPort, serverName, p.version, p.serverInstanceID);
    }
        break;

    case quint8(MS::CMD_JobProgress):
    {
        qDebug()<<"~~CMD_JobProgress--";

        JobProgressPacket p(packet);
        emit signalJobFinished(p.jobID, p.result, p.extraData);
    }
        break;

    case quint8(MS::CMD_Message):
    {
        qDebug()<<"~~CMD_Message--";

        MessagePacket p(packet);
        if(serverName == peerID){
            emit signalServerMessageReceived(p.message, p.msgType);

        }else{
            emit signalClientMessagePacketReceived(peerID, p.message, p.msgType);
        }
    }
        break;

    case quint8(MS::CMD_ClientInfo):
    {
        qDebug()<<"~~CMD_ClientInfo";

        ClientInfoPacket p(packet);
        emit signalClientInfoPacketReceived(p.assetNO, p.data, p.infoType);
    }
        break;

    case quint8(MS::CMD_SystemInfoFromServer):
    {
        //qDebug()<<"~~CMD_SystemInfoFromServer";

        SystemInfoFromServerPacket p(packet);
        emit signalSystemInfoFromServerReceived(p.extraInfo, p.data, p.infoType);
    }
        break;

    case quint8(MS::CMD_USBDev):
    {
        qDebug()<<"~~CMD_USBDev";

        USBDevPacket p(packet);
        emit signalClientResponseUSBInfoPacketReceived(socketID, peerID, p.usbSTORStatus);
    }
        break;

    case quint8(MS::CMD_RemoteConsole):
    {
        qDebug()<<"~~ClientResponseRemoteConsole";

        RemoteConsolePacket p(packet);
        switch (p.InfoType) {
        case RemoteConsolePacket::REMOTECONSOLE_STATE :
            emit signalClientResponseRemoteConsoleStatusPacketReceived(peerID, p.ConsoleState.isRunning, p.ConsoleState.message, p.ConsoleState.messageType);
            break;

        case RemoteConsolePacket::REMOTECONSOLE_OUTPUT :
            emit signalRemoteConsoleCMDResultFromClientPacketReceived(peerID, p.Output.output);
            break;

        default:
            break;
        }

    }
        break;

    case quint8(MS::CMD_AdminLogin):
    {
        qDebug()<<"~~CMD_AdminLogin";

        AdminLoginPacket p(packet);
        emit signalServerResponseAdminLoginResultPacketReceived(socketID, peerID, p.LoginResult.loggedIn, p.LoginResult.message, p.LoginResult.readonly);
    }
        break;

    case quint8(MS::CMD_AdminConnectionToClient):
    {
        AdminConnectionToClientPacket p(packet);
        emit signalClientResponseAdminConnectionResultPacketReceived(socketID, peerID, p.computerName, p.verified, p.errorCode, peerAddress.toString());
    }
        break;

    case quint8(MS::CMD_ModifyAssetNO):
    {
        ModifyAssetNOPacket p(packet);
        emit signalAssetNOModifiedPacketReceived(p.oldAssetNO, p.newAssetNO);
    }
        break;

    case quint8(MS::CMD_LocalUserOnlineStatusChanged):
    {
        LocalUserOnlineStatusChangedPacket p(packet);
        emit signalUserOnlineStatusChanged(peerID, p.userName, p.online);
    }
        break;

    case quint8(MS::CMD_Temperatures):
    {
        TemperaturesPacket p(packet);
        emit signalTemperaturesPacketReceived(peerID, p.TemperaturesResponse.cpuTemperature, p.TemperaturesResponse.harddiskTemperature);
    }
        break;

    case quint8(MS::CMD_Screenshot):
    {
        ScreenshotPacket p(packet);
        switch (p.InfoType) {
        case ScreenshotPacket::SCREENSHOT_DESKTOP_INFO :
            emit signalDesktopInfoPacketReceived(socketID, peerID, p.DesktopInfo.desktopWidth, p.DesktopInfo.desktopHeight, p.DesktopInfo.blockWidth, p.DesktopInfo.blockHeight);
            break;

        case ScreenshotPacket::SCREENSHOT_DATA :
            emit signalScreenshotPacketReceived(peerID, p.ScreenshotData.locations, p.ScreenshotData.images);
            break;

        default:
            break;
        }
    }
        break;

    case quint8(MS::CMD_ServiceConfig):
    {
        ServiceConfigPacket p(packet);
        emit signalServiceConfigChangedPacketReceived(peerID, p.serviceName, p.processID, p.startupType);
    }
        break;


        ////////////////////////////////////////////
    case quint8(MS::CMD_FileTransfer):
    {
        qDebug()<<"~~CMD_FileTransfer";

        FileTransferPacket p(packet);
        emit signalFileTransferPacketReceived(p);
    }
        break;



    default:
        qWarning()<<"ControlCenterPacketsParser! Unknown Packet Type: "<<packetType
                    //                 <<"    Serial Number: "<<packetSerialNumber
                 <<"    From: "<<peerAddress.toString()
                <<":"<<peerPort;
        break;

    }



}























} //namespace HEHUI
