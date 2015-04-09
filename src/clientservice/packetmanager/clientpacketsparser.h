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
    ClientPacketsParser(ClientResourcesManager *manager, QObject *parent = 0);
    virtual ~ClientPacketsParser();




public slots:
    void parseIncomingPacketData(Packet *packet);

    void changeLocalUserOnlineStatus(SOCKETID userSocketID, bool online, const QString &userName);
    QString userNameOfSocket(SOCKETID socketID);
    SOCKETID socketIDOfUser(const QString &userName);
    QList<SOCKETID> localUserSockets();


    void requestScreenshot(SOCKETID adminSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort);
    void setSocketConnectedToAdmin(SOCKETID socketID, const QString &adminName);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...) targetAddress:"<<targetAddress;

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

        return m_udpServer->sendDatagram(ba, address, IP_MULTICAST_GROUP_PORT);

    }

    bool sendClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, bool online){
        qDebug()<<"----sendClientOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(online?quint8(MS::ClientOnline):quint8(MS::ClientOffline));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << clientName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);

    }

    bool sendClientResponseRemoteConsoleStatusPacket(int adminSocketID, bool running, const QString &extraMessage, quint8 messageType = quint8(MS::MSG_Information)){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(adminSocketID);

        packet->setPacketType(quint8(MS::ClientResponseRemoteConsoleStatus));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << (running?quint8(1):quint8(0)) << extraMessage << messageType;
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
        out << m_localComputerName << result;
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
        out << m_localComputerName << softwareName;
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
        out << m_localComputerName << logType << log << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
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
        out << m_localComputerName << data << infoType;
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
        out << m_localComputerName << data << infoType;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);
    }

    bool sendClientResponseAdminConnectionResultPacket(SOCKETID socketID, bool result, const QString &message){
        //qWarning()<<"----sendClientResponseAdminConnectionResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        
        packet->setPacketType(quint8(MS::ClientResponseAdminConnectionResult));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << result << message;
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
        out << m_localComputerName << message <<clientMessageType;
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
        out << m_localComputerName << info;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientResponseProgramesInfoPacket(SOCKETID socketID, const QString &info){
        //qWarning()<<"----ClientResponseProgramesInfo(...):"<<adminAddress.toString()<<" "<<adminPort;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ClientResponseProgramesInfo));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << info;
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
        out << m_localComputerName << userName << (online?quint8(1):quint8(0)) ;
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
        out << m_localComputerName  ;
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
        out << m_localComputerName << adminAddress << adminPort << adminName ;
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
        out << m_localComputerName << adminAddress << adminPort << adminName << oldPassword << newPassword ;
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
    
    bool sendServerAnnouncementPacket(SOCKETID userSocketID, const QString &adminName, quint32 announcementID, const QString &serverAnnouncement, quint8 confirmationRequired, int validityPeriod){
        qDebug()<<"--sendServerAnnouncementPacket(...)"<<" userSocketID:"<<userSocketID<<" adminName:"<<adminName<<" serverAnnouncement:"<<serverAnnouncement;
        
        Packet *packet = PacketHandlerBase::getPacket(userSocketID);
        
        packet->setPacketType(quint8(MS::Announcement));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << adminName << announcementID << serverAnnouncement << confirmationRequired << validityPeriod;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(userSocketID, &ba);
    }




    bool sendClientResponseTemperaturesPacket(SOCKETID socketID, const QString &cpuTemperature, const QString &harddiskTemperature){
        qWarning()<<"----sendClientResponseTemperaturesPacket(...):"<<cpuTemperature<<" "<<harddiskTemperature;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ResponseTemperatures));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << cpuTemperature << harddiskTemperature;
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
        out << m_localComputerName << m_adminName << adminAddress << adminPort ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        PacketHandlerBase::recylePacket(packet);

        return m_rtp->sendReliableData(userSocketID, &ba);
    }

    bool sendUserReplyMessagePacket(SOCKETID socketID, const QString &userName, quint32 originalMessageID, const QString &replyMessage){
        qWarning()<<"----sendUserReplyMessagePacket(...):";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ReplyMessage));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_localComputerName << userName << originalMessageID << replyMessage;
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
        out << m_localComputerName << serviceName << processID << startupType;
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
        out << m_localComputerName << parentDirPath << fileSystemInfoData;
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
        qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

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
    
private slots:


signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientInfoRequestedPacketReceived(SOCKETID socketID, const QString &computerName, quint8 infoType);
    void signalAdminRequestRemoteConsolePacketReceived(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //void signalServerAnnouncementPacketReceived(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, const QString &userName, bool mustRead);

    void signalUpdateClientSoftwarePacketReceived();

    void signalSetupUSBSDPacketReceived(quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminName);
    void signalSetupProgramesPacketReceived(bool enable, bool temporarilyAllowed, const QString &adminName);
    void signalShowAdminPacketReceived(bool show);
    void signalModifyAdminGroupUserPacketReceived(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void signalRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &domainAdminName, const QString &domainAdminPassword);
    void signalJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool join, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword);

    void signalAdminRequestConnectionToClientPacketReceived(SOCKETID socketID, const QString &computerName, const QString &users);
    void signalAdminSearchClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    void signalAdminRequestRemoteAssistancePacketReceived(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
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
    QHostAddress serverAddress;
    quint16 serverUDTListeningPort;
    QString serverName;


    QHostAddress m_localUDTListeningAddress;
//    quint16 m_localUDTServerListeningPort;
    quint16 m_localTCPServerListeningPort;
    quint16 m_localENETListeningPort;


    QString m_localComputerName;

    ClientResourcesManager *m_resourcesManager;

    UDPServer *m_udpServer;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QMutex m_localUserSocketsHashMutex;
    QHash<SOCKETID /*Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

    SOCKETID m_socketConnectedToAdmin;
    QString m_adminName;

};

}

#endif /* CLIENTPACKETSPARSER_H_ */
