
#include <QThreadPool>
#include <QtConcurrentRun>
#include <QMessageBox>
#include <QScreen>
#include <QBuffer>
#include <QHostInfo>

#include "helper.h"

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/WinUtilities"
#endif


namespace HEHUI {


Helper::Helper(QObject *parent) :
    QObject(parent)
{
    


    WinUtilities::getLogonInfoOfCurrentUser(&m_userName, &m_logonDomain);
    m_logonDomain = m_logonDomain.toLower();
     m_localComputerName = QHostInfo::localHostName().toLower();
     if(!m_logonDomain.isEmpty() && (m_localComputerName != m_logonDomain)){
         m_userName = m_logonDomain + "\\" + m_userName;
     }





    //m_networkReady = false;

    resourcesManager = ResourcesManagerInstance::instance();
    bulletinBoardPacketsParser = 0;
    
    remoteAssistance = 0;
    bulletinBoardWidget = 0;
    updatePasswordWidget = 0;
    
    localUDTListeningPort = UDT_LISTENING_PORT + 20;
    m_rtp = 0;
    m_socketConnectedToLocalServer = INVALID_SOCK_ID;
//    m_socketConnectedToAdmin = INVALID_SOCK_ID;


    m_connectToLocalServerTimer = new QTimer(this);
    connect(m_connectToLocalServerTimer, SIGNAL(timeout()), this, SLOT(connectToLocalServer()));
    m_connectToLocalServerTimer->start(60000);

    QTimer::singleShot(5000, this, SLOT(startNetwork()));
//    startNetwork();



    
}

Helper::~Helper(){

    disconnect();

    if(m_connectToLocalServerTimer){
        m_connectToLocalServerTimer->stop();
        delete m_connectToLocalServerTimer;
        m_connectToLocalServerTimer = 0;
    }

    if(m_rtp){
        m_rtp->closeSocket(m_socketConnectedToLocalServer);
        m_rtp->stopServers();
    }

    if(remoteAssistance){
        remoteAssistance->close();
        delete remoteAssistance;
    }
    remoteAssistance = 0;
    
    if(bulletinBoardWidget){
        bulletinBoardWidget->close();
        delete bulletinBoardWidget;
    }
    remoteAssistance = 0;
    
    if(updatePasswordWidget){
        updatePasswordWidget->close();
        delete updatePasswordWidget;
    }
    remoteAssistance = 0;
    
    
    delete bulletinBoardPacketsParser;
    bulletinBoardPacketsParser = 0;

    //m_udtProtocol->closeUDTProtocol();

//    resourcesManager->cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

    PacketHandlerBase::clean();

    
}


void Helper::startNetwork(){
    qDebug()<<"--BulletinBoardObject::startNetwork()";

    if(!m_rtp){
        QString errorMessage = "";
        m_rtp = resourcesManager->startRTP(QHostAddress::Any, localUDTListeningPort, true, &errorMessage);
        connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)));
        if(!errorMessage.isEmpty()){
            qCritical()<<errorMessage;
        }

        //        m_udtProtocol = m_rtp->getUDTProtocol();
////        if(!m_udtProtocol){
////            QString error = tr("Can not start UDT listening on port %1! %2").arg(localUDTListeningPort).arg(errorMessage);
////            qCritical()<< error;
////            return;
////        }
//        localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
////        connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
//        m_udtProtocol->startWaitingForIOInOneThread(1000);
//        //m_udtProtocol->startWaitingForIOInSeparateThread(1, 500);

    }

    if(!bulletinBoardPacketsParser){
        bulletinBoardPacketsParser = new BulletinBoardPacketsParser(resourcesManager, m_userName, m_localComputerName, this);

        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), this, SLOT(adminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), this, SLOT(AdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAnnouncementPacketReceived(const QString&, quint32, const QString&, bool, int)), this, SLOT(serverAnnouncementPacketReceived(const QString&, quint32, const QString&, bool, int)), Qt::QueuedConnection);

        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestScreenshotPacketReceived(SOCKETID)), this, SLOT(adminRequestScreenshotPacketReceived(SOCKETID)), Qt::QueuedConnection);

    }

    connectToLocalServer();

}

void Helper::adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName ){

    if(!remoteAssistance){
        remoteAssistance = new RemoteAssistance(adminAddress, adminPort, adminName);
        connect(remoteAssistance, SIGNAL(signalUserResponseRemoteAssistance(const QString &, quint16, bool)), bulletinBoardPacketsParser, SLOT(sendUserResponseRemoteAssistancePacket(const QString &, quint16, bool )));
    }else {
        remoteAssistance->requestRemoteAssistance(adminAddress, adminPort, adminName);
    }
    
    remoteAssistance->show();
    remoteAssistance->raise();
    
    
    
    
}

void Helper::AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword ){

    qWarning()<<"BulletinBoardPlugin::AdminInformUserNewPasswordPacketReceived(...)";
    
    if(!updatePasswordWidget){
        updatePasswordWidget = new UpdatePasswordWidget(adminAddress, adminPort, adminName, oldPassword, newPassword);
        connect(updatePasswordWidget, SIGNAL(newPasswordRetreved()), this, SLOT(sendNewPasswordRetrevedByUserPacket()));
    }else {
        updatePasswordWidget->informNewPassword(adminAddress, adminPort, adminName, oldPassword, newPassword);
    }
    
    //updatePasswordWidget->show();
    
}

void Helper::serverAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement, bool confirmationRequired, int validityPeriod){
    qWarning()<<"Helper::serverAnnouncementPacketReceived(...)";
    
    if(!bulletinBoardWidget){
        bulletinBoardWidget = new BulletinBoardWidget(m_userName);
        connect(bulletinBoardWidget, SIGNAL(sendReplyMessage(quint32, const QString &)), this, SLOT(sendReplyMessage(quint32, const QString &)));
    }

    bulletinBoardWidget->showServerAnnouncement(adminName, announcementID, announcement);
    bulletinBoardWidget->showNormal();
    bulletinBoardWidget->raise();
}

void Helper::sendReplyMessage(quint32 originalMessageID, const QString &replyMessage){
    bulletinBoardPacketsParser->sendUserReplyMessagePacket(m_socketConnectedToLocalServer, originalMessageID, replyMessage);
}

void Helper::newPasswordRetreved(){

    bulletinBoardPacketsParser->sendNewPasswordRetrevedByUserPacket(m_socketConnectedToLocalServer);
}

void Helper::adminRequestScreenshotPacketReceived(SOCKETID socketID){

    QScreen *screen = QApplication::primaryScreen();
    if (!screen){
        qCritical()<<"ERROR! No primary screen.";
        return;
    }

    QPixmap pixmap = screen->grabWindow(0);
    QImage image = pixmap.toImage();
    if(image.isNull()){
        qCritical()<<"ERROR! Invalid image.";
        return;
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "jpg");
    buffer.close();

    bulletinBoardPacketsParser->sendUserResponseScreenshotPacket(socketID, byteArray);

}

//void BulletinBoardObject::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
//    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));
//}


//void BulletinBoardObject::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
//    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

//    if(!normalClose){
//        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
//    }

//}

void Helper::peerDisconnected(SOCKETID socketID){
    qDebug()<<"--BulletinBoardObject::peerDisconnected(...) "<<"socketID:"<<socketID;

    //m_rtp->closeSocket(socketID);
    m_socketConnectedToLocalServer = INVALID_SOCK_ID;


    if(m_connectToLocalServerTimer){
        m_connectToLocalServerTimer->start();
    }

}

void Helper::connectToLocalServer(){
    qDebug()<<"--BulletinBoardObject::connectToLocalServer()";

    m_rtp->closeSocket(m_socketConnectedToLocalServer);
    QString errorMessage;
    m_socketConnectedToLocalServer = m_rtp->connectToHost(QHostAddress::LocalHost, UDT_LISTENING_PORT, 10000, &errorMessage);
    if(m_socketConnectedToLocalServer == INVALID_SOCK_ID){
        qCritical()<<tr("ERROR! Can not connect to local server! %3").arg(errorMessage);
        m_connectToLocalServerTimer->start();
        return;
    }else{
        qDebug()<<"m_socketConnectedToLocalServer:"<<m_socketConnectedToLocalServer;

        bool ok = bulletinBoardPacketsParser->sendLocalUserOnlineStatusChangedPacket(m_socketConnectedToLocalServer, true);
        if(!ok){
            qCritical()<<m_rtp->lastErrorString();
        }else{
            m_connectToLocalServerTimer->stop();
        }

    }

}








} //namespace HEHUI
