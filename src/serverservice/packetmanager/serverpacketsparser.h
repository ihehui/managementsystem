/*
 ****************************************************************************
 * serverpacketsparser.h
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

#ifndef SERVERPACKETSPARSER_H_
#define SERVERPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QDateTime>

//#include "../../sharedms/global_shared.h"
#include "../../sharedms/mspackets.h"

#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/Cryptography"



namespace HEHUI
{


class ServerPacketsParser : public QObject
{
    Q_OBJECT
public:
    ServerPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ServerPacketsParser();


public slots:

    void parseIncomingPacketData(const PacketBase &packet);


    bool sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort)
    {
qDebug()<<"--sendServerDeclarePacket(...)  peerAddress:"<<peerAddress<<"  peerPort:"<<peerPort;
        static int serverInstanceID = 0;
        if(!serverInstanceID){
            qsrand(QDateTime::currentDateTime().toTime_t());
            serverInstanceID = qrand();
        }
        //qDebug()<<"Server Instance ID:"<<serverInstanceID;

        ServerDiscoveryPacket packet;
        packet.responseFromServer = 1;
        packet.version = QString(APP_VERSION);
        packet.rtpPort = localRTPListeningPort;
        packet.tcpPort = m_localTCPServerListeningPort;
        packet.serverInstanceID = serverInstanceID;

        return m_udpServer->sendDatagram(packet.toByteArray(), peerAddress, peerPort);
    }

    bool forwardData(SOCKETID peerSocketID, const QString &senderID, const QByteArray &data)
    {
        qDebug() << "--forwardData(...)";

        DataForwardPacket p;
        p.data = data;
        p.peer = senderID;

        QByteArray ba = p.toByteArray();
        if(ba.isEmpty()) {
            return false;
        }

        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendServerMessagePacket(SOCKETID adminSocketID, const QString &message, quint8 messageType = quint8(MS::MSG_Information))
    {

        MessagePacket packet;
        packet.msgType = messageType;
        packet.message = message;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendJobFinishedPacket(SOCKETID adminSocketID, quint32 jobID, quint8 result, const QVariant &extraData)
    {

        JobProgressPacket packet;
        packet.jobID = jobID;
        packet.result = result;
        packet.extraData = extraData;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendRequestClientInfoPacket(const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT), const QString &assetNO = "", quint8 infoType = 0)
    {

        ClientInfoPacket packet;
        packet.IsRequest = 1;
        packet.infoType = infoType;

        return m_udpServer->sendDatagram(packet.toByteArray(), QHostAddress(peerAddress), clientPort);
    }

    bool sendRequestClientInfoPacket(SOCKETID socketID, const QString &assetNO = "", quint8 infoType = 0)
    {

        ClientInfoPacket packet;
        packet.IsRequest = 1;
        packet.infoType = infoType;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendAdminLoginResultPacket(SOCKETID socketID, bool result, const QString &message, bool readonly)
    {
        qDebug() << "----sendAdminLoginResultPacket(...)";

        AdminLoginPacket packet;
        packet.InfoType = AdminLoginPacket::LOGIN_RESULT;
        packet.LoginResult.loggedIn = result;
        packet.LoginResult.message = message;
        packet.LoginResult.readonly = readonly;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendAdminRequestConnectionAuthPacket(SOCKETID adminSocketID, const QString &adminID, int adminToken, const QString &hostName)
    {
        //qWarning()<<"----sendAdminRequestConnectionAuthPacket(...):"<<adminID<<" verified:"<<verified;

        AdminConnectionToClientPacket packet;
        packet.InfoType = AdminConnectionToClientPacket::ADMINCONNECTION_SERVER_ASK_CLIENT_AUTH;
        packet.adminID = adminID;
        packet.adminToken = adminToken;
        packet.hostName = hostName;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendClientResponseAdminConnectionAuthPacket(SOCKETID adminSocketID, const QString &adminID, bool verified, int clientToken, quint8 errorCode, const QString &errorMessage)
    {
        //qWarning()<<"----sendClientResponseAdminConnectionAuthPacket(...):"<<adminID<<" verified:"<<verified;

        AdminConnectionToClientPacket packet;
        packet.InfoType = AdminConnectionToClientPacket::ADMINCONNECTION_RESPONSE_AUTH;
        packet.adminID = adminID;
        packet.ok = quint8(verified);
        packet.clientToken = clientToken;
        packet.errorCode = errorCode;
        packet.errorMessage = errorMessage;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendSystemInfoPacket(SOCKETID socketID, const QString &extraInfo, const QByteArray &data, quint8 infoType)
    {
        //qDebug()<<"----sendSystemInfoPacket(...)"<<" socketID:"<<socketID<<" infoType:"<<infoType;

        SystemInfoFromServerPacket packet;
        packet.infoType = infoType;
        packet.data = data;
        packet.extraInfo = extraInfo;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendServerResponseModifyAssetNOPacket(SOCKETID socketID, const QString &newAssetNO, const QString &oldAssetNO)
    {
        qWarning() << "----sendServerResponseModifyAssetNOPacket(...) newAssetNO:" << newAssetNO << " oldAssetNO:" << oldAssetNO;

        ModifyAssetNOPacket packet;
        packet.isRequest = 0;
        packet.oldAssetNO = oldAssetNO;
        packet.newAssetNO = newAssetNO;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }





private slots:


signals:

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);
    void signalClientDeclarePacketReceived(SOCKETID socketID, const QString &clientName, bool isAdmin);

    //void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    //void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    void signalDataForwardPacketReceived(const DataForwardPacket &packet);

    void signalClientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);

    void signalClientLogReceived(const ClientLogPacket &packet);

    void signalModifyAssetNOPacketReceived(const ModifyAssetNOPacket &packet);

    void signalProcessMonitorInfoPacketReceived(const ProcessMonitorInfoPacket &packet);

    void signalClientInfoPacketReceived(const ClientInfoPacket &packet);
    void signalUpdateSysAdminInfoPacketReceived(const SysAdminInfoPacket &packet);


    void signalAdminConnectionToClientPacketReceived(const AdminConnectionToClientPacket &packet);
    void signalAdminLogin(const AdminLoginPacket &packet);
    void signalSystemAlarmsPacketReceived(const SystemAlarmsPacket &packet);

    void signalAnnouncementsPacketReceived(const AnnouncementPacket &packet);

    void signalAdminOnlineStatusChanged(SOCKETID socketID, const QString &adminComputerName, const QString &adminName, bool online);

private:

private:
    ResourcesManagerInstance *m_resourcesManager;
    UDPServer *m_udpServer;

    RTP *m_rtp;
//    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    quint16 localRTPListeningPort;
    quint16 m_localTCPServerListeningPort;

    QString m_serverName;

    quint16 localIPMCListeningPort;





};

}

#endif /* SERVERPACKETSPARSER_H_ */
