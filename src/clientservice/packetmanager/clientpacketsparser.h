/*
 ****************************************************************************
 * clientpacketsparser.h
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

#ifndef CLIENTPACKETSPARSER_H_
#define CLIENTPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

#include "../clientresourcesmanager.h"

#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI {


class ClientPacketsParser : public QObject{
    Q_OBJECT
public:
    ClientPacketsParser(const QString &assetNO, ClientResourcesManager *manager, QObject *parent = 0);
    virtual ~ClientPacketsParser();




public slots:
    void parseIncomingPacketData(Packet *packet);

    void changeLocalUserOnlineStatus(SOCKETID userSocketID, bool online, const QString &userName);
    QString userNameOfSocket(SOCKETID socketID);
    SOCKETID socketIDOfUser(const QString &userName);
    QList<SOCKETID> localUserSockets();


    void requestScreenshot(SOCKETID adminSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort);
    void setSocketConnectedToServer(SOCKETID serverSocketID);
    void setSocketConnectedToAdmin(SOCKETID socketID, const QString &adminName);
    void setAssetNO(const QString &assetNO);


    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = IP_MULTICAST_GROUP_PORT){
        qDebug()<<"----sendClientLookForServerPacket(...) targetAddress:"<<targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }
        quint16 port = targetPort;
        if(!port){
            port = IP_MULTICAST_GROUP_PORT;
        }
        
        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << m_udpServer->localPort() << QString(APP_VERSION);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, address, port );

    }

    bool sendClientOnlineStatusChangedPacket(SOCKETID socketID, bool online){
        qDebug()<<"----sendClientOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientOnlineStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << quint8(online);
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &userName){
        qDebug()<<"----sendRequestAnnouncementsPacket(...)";
        return sendRequestAnnouncementsPacket(serverSocketID, "", "", "1", m_assetNO, userName, "-1", "1970-01-01", "2099-01-01");
    }

    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &id, const QString &keyword, const QString &validity, const QString &assetNO, const QString &userName, const QString &target, const QString &startTime, const QString &endTime){
        qDebug()<<"----sendRequestAnnouncementsPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);

        packet->setPacketType(quint8(MS::RequestAnnouncement));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << id << keyword << validity << assetNO << userName << target << startTime << endTime;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendClientResponseRemoteConsoleStatusPacket(int adminSocketID, bool running, const QString &extraMessage, quint8 messageType = quint8(MS::MSG_Information)){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::ClientResponseRemoteConsoleStatus));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << (running?quint8(1):quint8(0)) << extraMessage << messageType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(adminSocketID, &ba);

    }

    bool sendRemoteConsoleCMDResultFromClientPacket(int adminSocketID, const QString &result){
        qDebug()<<"----sendRemoteConsoleCMDResultFromClientPacket(...)";


        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::RemoteConsoleCMDResultFromClient));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << result;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendClientRequestSoftwareVersionPacket(SOCKETID socketID, const QString &softwareName){
        qDebug()<<"----sendClientRequestSoftwareVersionPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientRequestSoftwareVersion));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << softwareName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientLogPacket(SOCKETID socketID, quint8 logType, const QString &log){
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientLog));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << logType << log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientInfoPacket(SOCKETID socketID, const QByteArray &data, quint8 infoType){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << data << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientInfoPacket(const QString &targetAddress, quint16 targetPort, const QByteArray &data, quint8 infoType){
        //qWarning()<<"----sendClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientInfo));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << data << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);
    }

    bool sendClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &computerName, bool result, const QString &message){
        //qWarning()<<"----sendClientResponseAdminConnectionResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientResponseAdminConnectionResult));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << computerName << quint8(result) << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientMessagePacket(int adminSocketID, const QString &message, quint8 clientMessageType = quint8(MS::MSG_Information)){

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);
        
        packet->setPacketType(quint8(MS::ClientMessage));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << message <<clientMessageType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendClientResponseUSBInfoPacket(SOCKETID socketID, const QString &info){
        //qWarning()<<"----sendClientResponseSetupUSBInfoPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseUSBInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << info;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendLocalUserOnlineStatusChangedPacket(SOCKETID adminSocketID, const QString &userName, bool online){
        qDebug()<<"--sendLocalUserOnlineStatusChangedPacket(...)"<<" userSocketID:"<<adminSocketID<<" adminName:"<<userName<<" online:"<<online;

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::LocalUserOnlineStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << userName << (online?quint8(1):quint8(0)) ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }



    bool sendLocalServiceServerDeclarePacket(int userSocketID){

        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::LocalServiceServerDeclare));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(userSocketID, &ba);
    }
    
    void requestRemoteAssistance(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &userName){

//        foreach (SOCKETID socketID, m_localUserSocketsHash.keys()) {
//            sendRequestRemoteAssistancePacket(socketID, adminAddress, adminPort, adminName);
//        }
        SOCKETID sID = m_localUserSocketsHash.key(userName);
        sendRequestRemoteAssistancePacket(sID, adminAddress, adminPort, adminName);
        
    }

    bool sendRequestRemoteAssistancePacket( SOCKETID userSocketID, const QString &adminAddress, quint16 adminPort, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << adminAddress << adminPort << adminName ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(userSocketID, &ba);
    }


    bool sendInformUpdatePasswordPacket(const QString &userName, const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword){
        //qWarning()<<"sendInformUpdatePasswordPacket(...)"<<" userName:"<<userName<<" Port:"<<localUsersHash.value(userName) << " newPassword:"<<newPassword;
        
        SOCKETID socketID = INVALID_SOCK_ID;
        if(!m_localUserSocketsHash.values().contains(userName)){
            return false;
        }
        socketID = m_localUserSocketsHash.key(userName);


        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << adminAddress << adminPort << adminName << oldPassword << newPassword ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }
    
//    void sendAnnouncement(const QString &adminName, quint32 announcementID, const QString &announcement){
//        qDebug()<<"--sendAnnouncement(...)"<<" m_localUserSocketsHash.size():"<<m_localUserSocketsHash.size();

//        foreach (SOCKETID socketID, m_localUserSocketsHash.keys()) {
//            sendServerAnnouncementPacket(socketID, adminName, announcementID, announcement);
//        }

//    }
    
//    bool sendServerAnnouncementPacket(SOCKETID userSocketID, const QString &adminName, quint32 announcementID, const QString &serverAnnouncement, quint8 confirmationRequired, int validityPeriod){
//        qDebug()<<"--sendServerAnnouncementPacket(...)"<<" userSocketID:"<<userSocketID<<" adminName:"<<adminName<<" serverAnnouncement:"<<serverAnnouncement;
        
//        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
//        packet->setPacketType(quint8(MS::Announcement));
//        packet->setTransmissionProtocol(TP_UDT);
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_8);

//        out << m_assetNO << announcementID << adminName << serverAnnouncement << confirmationRequired << validityPeriod;
//        packet->setPacketData(ba);

//        ba.clear();
//        out.device()->seek(0);
//        QVariant v;
//        v.setValue(*packet);
//        out << v;

//        PacketHandlerBase::recylePacket(packet);

//        return m_rtp->sendReliableData(userSocketID, &ba);
//    }

    bool sendSystemInfoPacket(SOCKETID socketID, const QString &extraInfo, const QByteArray &data, quint8 infoType){
        //qDebug()<<"----sendSystemInfoPacket(...)"<<" socketID:"<<socketID<<" infoType:"<<infoType;

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::SystemInfoFromServer));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << extraInfo << data << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendClientResponseModifyAssetNOPacket(SOCKETID adminSocketID, const QString &oldAssetNO, bool modified, const QString &message){
        qWarning()<<"----sendClientResponseModifyAssetNOPacket(...) oldAssetNO"<<oldAssetNO<<" "<<modified;

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::AssetNOModified));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << oldAssetNO << quint8(modified) << message;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendModifyAssetNOPacket(SOCKETID serverSocketID, const QString &newAssetNO, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(MS::ModifyAssetNO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << newAssetNO << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendClientResponseTemperaturesPacket(SOCKETID socketID, const QString &cpuTemperature, const QString &harddiskTemperature){
        qWarning()<<"----sendClientResponseTemperaturesPacket(...):"<<cpuTemperature<<" "<<harddiskTemperature;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseTemperatures));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << cpuTemperature << harddiskTemperature;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAdminRequestScreenshotPacket(SOCKETID userSocketID, const QString &adminAddress, quint16 adminPort){
        //qDebug()<<"----sendAdminRequestScreenshotPacket(...):";

        Packet *packet = PacketHandlerBase::getPacket(userSocketID);

        packet->setPacketType(quint8(MS::RequestScreenshot));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << m_adminName << adminAddress << adminPort ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(userSocketID, &ba);
    }

    bool sendUserReplyMessagePacket(SOCKETID socketID, const QString &announcementID, const QString &sender, const QString &receiver, const QString &replyMessage){
        qWarning()<<"----sendUserReplyMessagePacket(...):";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ReplyMessage));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << announcementID << sender << receiver  << replyMessage;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

//    bool sendClientResponseScreenshotPacket(SOCKETID socketID, const QString &userName, const QByteArray &screenshot){
//        qWarning()<<"----sendClientResponseScreenshotPacket(...):";

//        Packet *packet = PacketHandlerBase::getPacket(socketID);

//        packet->setPacketType(quint8(MS::ResponseScreenshot));
//        packet->setTransmissionProtocol(TP_UDT);
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_8);
//        out << m_localComputerName << userName << screenshot;
//        packet->setPacketData(ba);

//        ba.clear();
//        out.device()->seek(0);
//        QVariant v;
//        v.setValue(*packet);
//        out << v;

//        PacketHandlerBase::recylePacket(packet);

//        return m_rtp->sendReliableData(socketID, &ba);
//    }

    bool sendClientResponseServiceConfigChangedPacket(SOCKETID socketID, const QString &serviceName, quint64 processID, quint64 startupType){
        qWarning()<<"----sendClientResponseServiceConfigChangedPacket(...):";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ServiceConfigChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << serviceName << processID << startupType;
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
    bool responseFileSystemInfo(SOCKETID socketID, QString parentDirPath, const QByteArray &fileSystemInfoData){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseFileSystemInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << parentDirPath << fileSystemInfoData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = ""){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestUploadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << fileMD5Sum << fileName << size << remoteFileSaveDir;
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
        out << m_assetNO << remoteBaseDir << remoteFileName << localFileSaveDir;
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
        out << m_assetNO << fileName << accepted << fileMD5Sum << size << remoteFileSaveDir;
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
        out << m_assetNO << fileName << accepted << message;
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
        out << m_assetNO << fileMD5Sum << accepted << message;
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
        qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::RequestFileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << fileMD5 << startPieceIndex << endPieceIndex;
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
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::FileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_assetNO << fileMD5 << pieceIndex << *data << *sha1 ;
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
        out << m_assetNO << fileMD5 << status ;
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
        out << m_assetNO << fileMD5 << errorCode << errorString ;
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
    //void  signalHeartbeatPacketReceived(const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void signalServerOnlineStatusChangedPacketReceived(bool online, const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientInfoRequestedPacketReceived(SOCKETID socketID, const QString &assetNO, quint8 infoType);
    void signalAdminRequestRemoteConsolePacketReceived(const QString &assetNO, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &assetNO, const QString &command, const QString &adminAddress, quint16 adminPort);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    void signalSystemInfoFromServerReceived(const QString &extraInfo, const QByteArray &clientInfo, quint8 infoType);

    //void signalServerAnnouncementPacketReceived(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, const QString &userName, bool mustRead);

    void signalUpdateClientSoftwarePacketReceived();

    void signalSetupUSBSDPacketReceived(quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminName);
    void signalShowAdminPacketReceived(bool show);
    void signalModifyAdminGroupUserPacketReceived(const QString &assetNO, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void signalModifyAssetNOPacketReceived(const QString &newAssetNO, const QString &adminName);
    void signalAssetNOModifiedPacketReceived(const QString &newAssetNO, const QString &oldAssetNO, bool modified, const QString &message);

    void signalRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &domainAdminName, const QString &domainAdminPassword);
    void signalJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool join, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword);

    void signalAdminRequestConnectionToClientPacketReceived(SOCKETID socketID, const QString &adminComputerName, const QString &users);
    void signalAdminSearchClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    //void signalAdminRequestRemoteAssistancePacketReceived(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestUpdateMSUserPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalAdminRequestInformUserNewPasswordPacketReceived(const QString &workgroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);

//    void signalLocalUserOnlineStatusChanged(SOCKETID userSocketID, const QString &userName, bool online);

    void signalAdminRequestTemperatures(SOCKETID adminSocketID, bool cpu, bool harddisk);
//    void signalAdminRequestScreenshot(SOCKETID socketID, const QString &userName, bool fullScreen);

    void signalAdminRequestShutdownPacketReceived(SOCKETID adminSocketID, const QString &message, quint32 waitTime, bool force, bool reboot);
    void signalAdminRequestLockWindowsPacketReceived(SOCKETID adminSocketID, const QString &userName, bool logoff);
    void signalAdminRequestCreateOrModifyWinUserPacketReceived(SOCKETID adminSocketID, const QByteArray &userData);
    void signalAdminRequestDeleteUserPacketReceived(SOCKETID adminSocketID, const QString &userName);


    void signalAdminRequestChangeServiceConfigPacketReceived(SOCKETID socketID, const QString &serviceName, bool startService, unsigned long startupType);
    void signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules);


///////////////////////////
    void signalFileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);

    void signalAdminRequestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalAdminRequestDownloadFile(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);
    void signalFileDataRequested(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

private:


private:
    QString m_assetNO;
    //QString m_localComputerName;

    QHostAddress serverAddress;
    quint16 serverRTPListeningPort;
    QString serverName;


    QHostAddress m_localUDTListeningAddress;
//    quint16 m_localUDTServerListeningPort;
    quint16 m_localTCPServerListeningPort;
    quint16 m_localENETListeningPort;

    ClientResourcesManager *m_resourcesManager;

    UDPServer *m_udpServer;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QMutex m_localUserSocketsHashMutex;
    QHash<SOCKETID /*Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

    SOCKETID m_socketConnectedToServer;
    SOCKETID m_socketConnectedToAdmin;
    QString m_adminName;

};

}

#endif /* CLIENTPACKETSPARSER_H_ */
