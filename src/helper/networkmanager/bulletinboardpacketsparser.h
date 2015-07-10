/*
 ****************************************************************************
 * bulletinboardpacketsparser.cpp.h
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

#ifndef BULLETINBOARDPACKETSPARSER_H_
#define BULLETINBOARDPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/packets.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"
//#include "HHSharedNetwork/hpacketparserbase.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedNetwork/hpackethandlerbase.h"


namespace HEHUI {


class BulletinBoardPacketsParser : public QObject{
    Q_OBJECT

public:
    BulletinBoardPacketsParser(ResourcesManagerInstance *resourcesManager, const QString &userName, const QString &computerName, QObject *parent = 0);
    virtual ~BulletinBoardPacketsParser();


public slots:
    void parseIncomingPacketData(const PacketBase &packet);


    bool sendLocalUserOnlineStatusChangedPacket(SOCKETID socketID, bool online){
        qDebug()<<"--sendLocalUserOnlineStatusChangedPacket(...)";

        LocalUserOnlineStatusChangedPacket packet;
        packet.userName = m_userName;
        packet.online = online;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }
    
    bool sendUserReplyMessagePacket(SOCKETID socketID, unsigned int announcementID, const QString &receiver, const QString &replyMessage){
        qWarning()<<"----sendUserReplyMessagePacket(...):";

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_REPLY;
        packet.ReplyInfo.announcementID = announcementID;
        packet.ReplyInfo.sender = m_userName;
        packet.ReplyInfo.receiver = receiver;
        packet.ReplyInfo.receiversAssetNO = "";
        packet.ReplyInfo.replyMessage = replyMessage;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendUserScreenshotPacket(SOCKETID socketID, QList<QPoint> locations, QList<QByteArray> images){
        qWarning()<<"----sendUserScreenshotPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_DATA;
        packet.ScreenshotData.locations = locations;
        packet.ScreenshotData.images = images;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendUserDesktopInfoPacket(SOCKETID socketID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){
        qWarning()<<"----sendUserDesktopInfoPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_DESKTOP_INFO;
        packet.DesktopInfo.desktopWidth = desktopWidth;
        packet.DesktopInfo.desktopHeight = desktopHeight;
        packet.DesktopInfo.blockWidth = blockWidth;
        packet.DesktopInfo.blockHeight = blockHeight;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }




    /////////////////////////////////////////////////////



signals:
    //void  signalHeartbeatPacketReceived(const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalLocalServiceServerDeclarePacketReceived(const QString &localComputerName);

    void signalSystemInfoFromServerReceived(const QString &extraInfo, const QByteArray &clientInfo, quint8 infoType);


    void signalAdminRequestScreenshotPacketReceived(SOCKETID adminSocketID, const QString &adminName, const QString &adminAddress, quint16 adminPort);


private:

    QString m_userName;
    QString m_localComputerName;
    QString m_localID;

    ResourcesManagerInstance *m_resourcesManager;
    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;



    RTP *m_rtp;
//    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;



};

}

#endif /* BULLETINBOARDPACKETSPARSER_H_ */
