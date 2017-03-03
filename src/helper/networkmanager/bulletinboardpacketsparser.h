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

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
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

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUserScreenshotPacket(SOCKETID socketID, QList<QPoint> locations, QList<QByteArray> images){
        qWarning()<<"----sendUserScreenshotPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_DATA;
        packet.ScreenshotData.locations = locations;
        packet.ScreenshotData.images = images;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUserDesktopInfoPacket(SOCKETID socketID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){
        qWarning()<<"----sendUserDesktopInfoPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_DESKTOP_INFO;
        packet.DesktopInfo.desktopWidth = desktopWidth;
        packet.DesktopInfo.desktopHeight = desktopHeight;
        packet.DesktopInfo.blockWidth = blockWidth;
        packet.DesktopInfo.blockHeight = blockHeight;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }



    /////////////////////////////////////////////////////



signals:
    void signalLocalServiceServerDeclarePacketReceived(const QString &localComputerName);

    void signalSystemInfoFromServerReceived(const SystemInfoFromServerPacket &packet);
    void signalAdminRequestScreenshotPacketReceived(const ScreenshotPacket &packet);


private:

    QString m_userName;
    QString m_localComputerName;
    QString m_localID;

    ResourcesManagerInstance *m_resourcesManager;
//    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;

    RTP *m_rtp;
//    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

};

}

#endif /* BULLETINBOARDPACKETSPARSER_H_ */
