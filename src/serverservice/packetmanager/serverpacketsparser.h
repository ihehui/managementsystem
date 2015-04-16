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

#include "../../sharedms/global_shared.h"
#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ServerPacketsParser : public QObject{
    Q_OBJECT
public:
    ServerPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ServerPacketsParser();


public slots:

    void parseIncomingPacketData(Packet *packet);


    bool sendServerDeclarePacket(const QHostAddress peerAddress, quint16 peerPort){

        qsrand(QDateTime::currentDateTime().toTime_t());
        static int serverInstanceID = qrand();
        //qDebug()<<"Server Instance ID:"<<serverInstanceID;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerDeclare));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << localRTPListeningPort << m_localTCPServerListeningPort << QString(APP_VERSION) << serverInstanceID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, peerAddress, peerPort);

    }

    bool sendServerOnlineStatusChangedPacket(bool online, const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendServerOnlinePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerOnlineStatusChanged));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << quint8(online) << localRTPListeningPort << m_localTCPServerListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);
    }

    bool sendRequestClientInfoPacket(const QString &peerAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 clientPort = quint16(IP_MULTICAST_GROUP_PORT), const QString &assetNO = "", quint8 infoType = 0){

        QHostAddress targetAddress = QHostAddress(peerAddress);
        Packet *packet = PacketHandlerBase::getPacket();
        packet->setTransmissionProtocol(TP_UDP);

        packet->setPacketType(quint8(MS::ClientInfoRequested));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << assetNO << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, targetAddress, clientPort);
    }

    bool sendRequestClientInfoPacket(SOCKETID socketID, const QString &assetNO = "", quint8 infoType = 0){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setTransmissionProtocol(TP_UDT);

        packet->setPacketType(quint8(MS::ClientInfoRequested));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << assetNO << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendServerResponseSoftwareVersionPacket(SOCKETID socketID, const QString &softwareName, const QString &version){
        qDebug()<<"----sendServerResponseSoftwareVersionPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ServerResponseSoftwareVersion));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << softwareName << version;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUpdateClientSoftwarePacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendUpdateClientSoftwarePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::Update));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);
    }

    bool sendAdminLoginResultPacket(SOCKETID socketID, bool result, const QString &message, bool readonly){
        qDebug()<<"----sendAdminLoginResultPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ServerResponseAdminLoginResult));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_serverName << quint8(result) << message << quint8(readonly);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


private slots:


signals:
    //void  signalHeartbeatPacketReceived(const QString &clientAddress, const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber, quint16 packetSerialNumbe2);

    void signalClientLookForServerPacketReceived(const QHostAddress &clientAddress, quint16 clientTCPListeningPort, const QString &clientName);
    void signalClientDeclarePacketReceived(SOCKETID socketID, const QString &clientName, bool isAdmin);

    //void signalClientOnlinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);
    //void signalClientOfflinePacketReceived(const QHostAddress &clientAddress, quint16 clientPort, const QString &clientName);

    void signalClientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);

    void signalClientLogReceived(const QString &assetNO, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);


    void signalClientOnlineStatusChanged(SOCKETID socketID, const QString &assetNO, bool online, const QString &ip, quint16 port);

    void signalAdminLogin(SOCKETID socketID, const QString &adminName, const QString &password, const QString &adminIP, const QString &adminComputerName);
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
