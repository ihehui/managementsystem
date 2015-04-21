/*
 ****************************************************************************
 * controlcenterpacketsparser.h
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
 * Last Modified On: 2015-4-10
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef CONTROLCENTERPACKETSPARSER_H_
#define CONTROLCENTERPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ControlCenterPacketsParser : public QObject{
    Q_OBJECT
public:
    ControlCenterPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ControlCenterPacketsParser();

public slots:

    void parseIncomingPacketData(Packet *packet);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendClientLookForServerPacket(...) "<<" targetAddress:"<<targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << m_udpServer->localPort() << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, address, targetPort);
    }

    bool sendAdminLoginPacket(SOCKETID socketID, const QString &adminName, const QString &password){
        qDebug()<<"----sendAdminLoginPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::AdminLogin));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << m_localComputerName << adminName << password;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendAdminOnlineStatusChangedPacket(SOCKETID socketID, const QString &adminName, quint8 online){
        qDebug()<<"----sendAdminOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::AdminOnlineStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << m_localComputerName << adminName << online;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }




    bool sendRequestClientInfoPacket(SOCKETID socketID, const QString &assetNO, quint8 infoType){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ClientInfoRequested));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << infoType;

        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestUpdateSysAdminInfoPacket(SOCKETID serverSocketID, const QString &sysAdminID, const QByteArray &infoData, bool deleteAdmin = false){

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(MS::UpdateSysAdminInfo));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << sysAdminID << infoData << quint8(deleteAdmin);

        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(serverSocketID, &ba);
    }


    bool sendAdminRequestRemoteConsolePacket(SOCKETID socketID, const QString &assetNO, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << applicationPath << adminID << startProcess;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRemoteConsoleCMDFromAdminPacket(SOCKETID socketID, const QString &assetNO, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << command;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendSetupUSBSDPacket(SOCKETID socketID, quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestSetupUSBSD));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << usbSTORStatus << temporarilyAllowed << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendShowAdminPacket(SOCKETID socketID, bool show){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ShowAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << show;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendModifyAdminGroupUserPacket(SOCKETID socketID, const QString &assetNO, const QString &userName, bool addToAdminGroup, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ModifyAdminGroupUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << userName << addToAdminGroup  << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendModifyAssetNOPacket(SOCKETID socketID, const QString &newAssetNO, const QString &oldAssetNO, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ModifyAssetNO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << newAssetNO << oldAssetNO << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRenameComputerPacket(SOCKETID socketID, const QString &assetNO, const QString &newComputerName, const QString &adminName, const QString &domainAdminName, const QString &domainAdminPassword){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::RenameComputer));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << newComputerName << adminName << domainAdminName << domainAdminPassword;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendJoinOrUnjoinDomainPacket(SOCKETID socketID, const QString &assetNO, const QString &adminName, bool join, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::JoinOrUnjoinDomain));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << adminName << join << domainOrWorkgroupName << domainAdminName << domainAdminPassword;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAdminRequestConnectionToClientPacket(SOCKETID socketID, const QString &adminComputerName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestConnectionToClient));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << adminComputerName << adminName ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }
    
    bool sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::AdminSearchClient));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, targetAddress, quint16(IP_MULTICAST_GROUP_PORT));
    }
    
    bool sendRemoteAssistancePacket(SOCKETID socketID, const QString &assetNO, const QString &adminName, const QString &userName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << adminName << userName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUpdateMSUserPasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);
    }
    
    bool sendUpdateMSUserPasswordPacket(SOCKETID socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendInformUpdatePasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);
    }
    
    bool sendInformUpdatePasswordPacket(SOCKETID socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAnnouncementPacket(const QString &peerAddress, quint16 peerPort, const QString &assetNO, const QString &userName, const QString &adminName, quint32 messageID, const QString &message, bool confirmationRequired = true, int validityPeriod = 60){
        qDebug()<<"--sendAnnouncementPacket(...) "<<" peerAddress:"<<peerAddress<<" computerName:"<<assetNO;
        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        packet->setPacketType(quint8(MS::Announcement));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << userName << adminName << messageID << message << (confirmationRequired?quint8(1):quint8(0)) << validityPeriod;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, targetAddress, peerPort);
    }

    bool sendAnnouncementPacket(SOCKETID socketID, const QString &assetNO, const QString &userName, const QString &adminName, quint32 messageID, const QString &message, bool confirmationRequired = true, int validityPeriod = 60){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::Announcement));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << assetNO << userName << adminName << messageID << message << (confirmationRequired?quint8(1):quint8(0)) << validityPeriod;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestTemperaturesPacket(SOCKETID socketID, bool cpu = true, bool harddisk = false){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestTemperatures));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << (cpu?quint8(1):quint8(0)) << (harddisk?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestScreenshotPacket(SOCKETID socketID, const QString &userName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestScreenshot));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << userName << m_localTCPServerListeningPort;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestShutdownPacket(SOCKETID socketID, QString message = "", quint32 waitTime = 0, bool force = true, bool reboot = true){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestShutdown));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << message << waitTime << (force?quint8(1):quint8(0)) << (reboot?quint8(1):quint8(0)) ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestLockWindowsPacket(SOCKETID socketID, QString userName, bool logoff = true){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestLockWindows));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << userName << (logoff?quint8(1):quint8(0)) ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestCreateOrModifyWinUserPacket(SOCKETID socketID, const QByteArray &userData){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestCreateOrModifyWinUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << userData ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestDeleteUserPacket(SOCKETID socketID, const QString &userName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestDeleteUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << userName ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestChangeServiceConfigPacket(SOCKETID socketID, const QString &serviceName, bool startService, quint64 startupType){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestChangeServiceConfig));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << serviceName << (startService?quint8(1):quint8(0)) << startupType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRules, const QByteArray &globalRules, quint8 enableProcMon, quint8 enablePassthrough, quint8 enableLogAllowedProcess, quint8 enableLogBlockedProcess, quint8 useGlobalRules, const QString &assetNO ){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestChangeProcessMonitorInfo));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localID << localRules << globalRules << enableProcMon << enablePassthrough << enableLogAllowedProcess << enableLogBlockedProcess << useGlobalRules << assetNO;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    //////////////////////////////
    bool requestFileSystemInfo(SOCKETID socketID, const QString &parentDirPath){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestFileSystemInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << parentDirPath;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = "./"){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestUploadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5Sum << fileName << size << remoteFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDownloadFile(SOCKETID socketID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestDownloadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << remoteBaseDir << remoteFileName << localFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool acceptFileDownloadRequest(SOCKETID socketID, const QString &fileName, bool accepted, const QByteArray &fileMD5Sum, quint64 size, const QString &remoteFileSaveDir){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileName << accepted << fileMD5Sum << size << remoteFileSaveDir;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }
    bool denyFileDownloadRequest(SOCKETID socketID, const QString &fileName, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileName << accepted << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileUploadRequest(SOCKETID socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileUploadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5Sum << accepted << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestFileData(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestFileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5 << startPieceIndex << endPieceIndex;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendFileData(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *sha1){
        qDebug()<<"--sendFileData(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5 << pieceIndex << *data << *sha1 ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileTXStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5 << status ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileTXError));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << fileMD5 << errorCode << errorString ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    QString lastErrorMessage(){return m_rtp->lastErrorString();}



signals:
    //void  signalHeartbeatPacketReceived(const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlineStatusChangedPacketReceived(bool online, const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientOnlineStatusChanged(SOCKETID socketID, const QString &assetNO, bool online);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    void signalClientResponseRemoteConsoleStatusPacketReceived(const QString &assetNO, bool accept, const QString &extraMessage, quint8 messageType);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &assetNO, const QString &result);



    void signalClientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType);
    void signalSystemInfoFromServerReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType);

    void signalClientResponseUSBInfoPacketReceived(SOCKETID socketID, const QString &assetNO, const QString &usbInfo);



    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //    void  signalServerAnnouncementPacketReceived(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true);

    void signalServerResponseAdminLoginResultPacketReceived(SOCKETID socketID, const QString &serverName, bool result, const QString &message, bool readonly);

    void signalClientResponseAdminConnectionResultPacketReceived(SOCKETID socketID, const QString &assetNO, const QString &computerName, bool result, const QString &message, const QString &clientIP);
    void signalClientMessagePacketReceived(const QString &assetNO, const QString &message, quint8 clientMessageType);

    void signalAssetNOModifiedPacketReceived(const QString &newAssetNO, const QString &oldAssetNO, bool modified, const QString &message);

    void signalUserResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);
    void signalNewPasswordRetrevedByUserPacketReceived(const QString &userName, const QString &computerName);
    

    void signalUserOnlineStatusChanged(const QString &assetNO, const QString &userName, bool online);

    void signalTemperaturesPacketReceived(const QString &assetNO, const QString &cpuTemperature, const QString &harddiskTemperature);

    void signalUserReplyMessagePacketReceived(const QString &assetNO, const QString &userName, quint32 originalMessageID, const QString &replyMessage);

    void signalDesktopInfoPacketReceived(quint32 userSocketID, const QString &userID, int desktopWidth, int desktopHeight,int  blockWidth, int blockHeight);
    void signalScreenshotPacketReceived(const QString &userID, QList<QPoint> locations, QList<QByteArray> images);

    void signalServiceConfigChangedPacketReceived(const QString &assetNO, const QString &serviceName, quint64 processID, quint64 startupType);



    ///////////////////////////
    void signalFileSystemInfoReceived(SOCKETID socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData);

    void signalAdminRequestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalAdminRequestDownloadFile(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);

    void signalFileDownloadRequestAccepted(SOCKETID socketID, const QString &remoteFileName, const QByteArray &fileMD5Sum, quint64 size, const QString &localFileSaveDir);
    void signalFileDownloadRequestDenied(SOCKETID socketID, const QString &remoteFileName, const QString &message);
    void signalFileUploadRequestResponsed(SOCKETID socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message);

    void signalFileDataRequested(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);


public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
    //    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    //    void heartbeat();
    //    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:

    ResourcesManagerInstance *m_resourcesManager;
    UDPServer *m_udpServer;
    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QHostAddress serverAddress;
    quint16 serverRTPListeningPort;


    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;


    QString m_localComputerName;
    QString m_localID;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;



//        quint16 localUDTListeningPort;
    quint16 m_localTCPServerListeningPort;
    quint16 m_localENETListeningPort;


};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
