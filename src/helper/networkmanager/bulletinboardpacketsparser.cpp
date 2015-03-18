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
//#include "HHSharedWindowsManagement/hwindowsmanagement.h"
//#define msleep(x) Sleep(x)
//#endif

//#ifdef Q_CC_GNU
//#include <unistd.h>
//#define msleep(x) usleep(x*1000)
//#endif


#include "HHSharedGUI/himageresourcebase.h"

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/WinUtilities"
#endif



namespace HEHUI {


BulletinBoardPacketsParser::BulletinBoardPacketsParser(ResourcesManagerInstance *resourcesManager, QObject *parent)
    :QObject(parent), m_resourcesManager(resourcesManager)
{


    m_rtp = m_resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    Q_ASSERT(m_udtProtocol);
//    m_udtProtocol->startWaitingForIOInOneThread(1000);
//    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
//   connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)));

   m_tcpServer = m_rtp->getTCPServer();
   Q_ASSERT(m_tcpServer);
   connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

   m_enetProtocol = m_rtp->getENETProtocol();
   Q_ASSERT(m_enetProtocol);
   connect(m_enetProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);


   WinUtilities::getLogonInfoOfCurrentUser(&m_userName, &m_domain);
   m_domain = m_domain.toLower();
    //m_userName = Utilities::currentUserNameOfOS();
    m_localComputerName = QHostInfo::localHostName().toLower();
    //m_localID = m_localComputerName + "/BulletinBoard";
    m_localID = m_userName + "@" + m_localComputerName;
    if(!m_domain.isEmpty() && (m_localComputerName != m_domain)){
        m_userName = m_domain + "\\" + m_userName;
        m_localID = m_domain + "\\" + m_localID;
    }
    qDebug()<<"----------m_localComputerName:"<<m_localComputerName <<"  m_domain"<<m_domain;

    //    emit signalAnnouncementPacketReceived("ADMIN", "TEST!");
    
}

BulletinBoardPacketsParser::~BulletinBoardPacketsParser() {
    // TODO Auto-generated destructor stub
    qDebug()<<"~BulletinBoardPacketsParser()";



    //    if(processWaitingForReplyPacketsTimer){
    //        processWaitingForReplyPacketsTimer->stop();
    //    }
    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;

    disconnect();


}

void BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet){
    qDebug()<<"----BulletinBoardPacketsParser::parseIncomingPacketData(Packet *packet)";
    

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_6);

    QString peerID = "";
    in >> peerID;

    quint8 packetType = packet->getPacketType();
    SOCKETID socketID = packet->getSocketID();
    PacketHandlerBase::recylePacket(packet);

    switch(packetType){

    case quint8(MS::LocalServiceServerDeclare):
    {        
        //QString localComputerName = "";
        //in >> localComputerName;

        emit signalLocalServiceServerDeclarePacketReceived(peerID);
        
    }
    break;

    case quint8(MS::AdminRequestRemoteAssistance):
    {
        
        QString adminAddress = "", adminName = "";
        quint16 adminPort = 0;

        in >> adminAddress >> adminPort >> adminName;
        emit signalAdminRequestRemoteAssistancePacketReceived(adminAddress, adminPort, adminName);

        qDebug()<<"~~AdminRequestRemoteAssistance";
        
    }
    break;

    case quint8(MS::InformUserNewPassword):
    {        
        QString adminAddress = "", adminName = "",  oldPassword = "",  newPassword = "";
        quint16 adminPort = 0;
        in >> adminAddress >> adminPort >> adminName >> oldPassword >> newPassword;
        emit signalAdminInformUserNewPasswordPacketReceived(adminAddress, adminPort, adminName, oldPassword, newPassword);
        
        qDebug()<<"~~InformUserNewPassword";

    }
    break;
    
    case quint8(MS::ServerAnnouncement):
    {        
        QString adminName = "", serverAnnouncement = "";
        quint32 announcementID = 0;
        in >> adminName >> announcementID >> serverAnnouncement;
        emit signalAnnouncementPacketReceived(adminName, announcementID, serverAnnouncement);
        //qDebug()<<"~~ServerAnnouncement"<<" Admin:"<<adminName<<" Msg:"<<serverAnnouncement<<" SN:"<<packet->getPacketSerialNumber();

    }
    break;

    case quint8(MS::RequestScreenshot):
    {
//        quint8 fullScreen = 1;
//        in >> fullScreen;

        emit signalAdminRequestScreenshotPacketReceived(socketID);

//        QImage image = ImageResourceBase::screenshot();
//        if(image.isNull()){
//            return;
//        }

//        QByteArray byteArray;
//        QBuffer buffer(&byteArray);
//        buffer.open(QIODevice::WriteOnly);
//        image.save(&buffer, "jpg");
//        buffer.close();

//        sendUserResponseScreenshotPacket(socketID, byteArray);

        qDebug()<<"~~RequestScreenshot";

    }
    break;


    default:
        qWarning()<<"BulletinBoardPacketsParser! Unknown Packet Type:"<<packetType;
                //<<" Serial Number:"<<packetSerialNumber
                //<<" From:"<<packet->getPeerHostAddress().toString()
               //<<":"<<packet->getPeerHostPort()
              //<<" Local Port:"<<localRUDPListeningPort;

        break;

    }

    qApp->processEvents();

}
























} //namespace HEHUI
