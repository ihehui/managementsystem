/*
 ****************************************************************************
 * bulletinboardpacketsparser.cpp
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
#include <QBuffer>

#include "bulletinboardpacketsparser.h"

//#ifdef Q_CC_MSVC
//#include <windows.h>
//#include "HHSharedSystemUtilities/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif


#include "HHSharedGUI/himageresourcebase.h"

#ifdef Q_OS_WIN32
    #include "HHSharedSystemUtilities/WinUtilities"
#endif



namespace HEHUI
{


BulletinBoardPacketsParser::BulletinBoardPacketsParser(ResourcesManagerInstance *resourcesManager, const QString &userName, const QString &computerName, QObject *parent)
    : QObject(parent), m_resourcesManager(resourcesManager), m_userName(userName), m_localComputerName(computerName)

{


    m_localID = m_userName + "@" + computerName;
    qDebug() << "----------computerName:" << computerName;
    Packet::setLocalID(m_localID);


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

    //    m_udtProtocol = m_rtp->getUDTProtocol();
    //    Q_ASSERT(m_udtProtocol);
    //    m_udtProtocol->startWaitingForIOInOneThread(1000);
    //    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
    //   connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)));

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);

    m_enetProtocol = m_rtp->getENETProtocol();
    Q_ASSERT(m_enetProtocol);
    connect(m_enetProtocol, SIGNAL(packetReceived(const PacketBase &)), this, SLOT(parseIncomingPacketData(const PacketBase &)), Qt::QueuedConnection);



    //    emit signalAnnouncementPacketReceived("ADMIN", "TEST!");

}

BulletinBoardPacketsParser::~BulletinBoardPacketsParser()
{
    qDebug() << "~BulletinBoardPacketsParser()";

    disconnect();
}

void BulletinBoardPacketsParser::parseIncomingPacketData(const PacketBase &packet)
{
    qDebug() << "----BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet)";


    quint8 packetType = packet.getPacketType();
    QString senderID = packet.getSenderID();

    QHostAddress peerAddress = packet.getPeerHostAddress();
    quint16 peerPort = packet.getPeerHostPort();
    SOCKETID socketID = packet.getSocketID();

    switch(packetType) {

    case quint8(MS::CMD_SystemInfoFromServer): {
        SystemInfoFromServerPacket p(packet);
        emit signalSystemInfoFromServerReceived(p);
    }
    break;

    case quint8(MS::CMD_Screenshot): {
        qDebug() << "~~CMD_Screenshot";
        ScreenshotPacket p(packet);
        if(m_userName != p.ScreenshotRequest.userName) {
            return;
        }
        emit signalAdminRequestScreenshotPacketReceived(p);
    }
    break;


    default:
        qWarning() << "BulletinBoardPacketsParser! Unknown Packet Type:" << packetType;
        //<<" Serial Number:"<<packetSerialNumber
        //<<" From:"<<packet->getPeerHostAddress().toString()
        //<<":"<<packet->getPeerHostPort()
        //<<" Local Port:"<<localRUDPListeningPort;

        break;

    }

    qApp->processEvents();

}
























} //namespace HEHUI
