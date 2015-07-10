
#include <QThreadPool>
//#include <QtConcurrentRun>
#include <QMessageBox>
#include <QScreen>
#include <QBuffer>
#include <QHostInfo>

#include "helper.h"

#ifdef Q_OS_WIN32
#include "HHSharedSystemUtilities/WinUtilities"
#endif


namespace HEHUI {


Helper::Helper(QObject *parent) :
    QObject(parent)
{

    m_userName = "";
    m_logonDomain = "";
    unsigned long status = 0;
    bool ok = WinUtilities::getLogonInfoOfCurrentUser(&m_userName, &m_logonDomain, 0, &status);
    if(!ok){
       qCritical()<<QString("ERROR! %1").arg(WinUtilities::WinSysErrorMsg(status));
    }
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
    
    localUDTListeningPort = RTP_LISTENING_PORT + 20;
    m_rtp = 0;
    m_socketConnectedToLocalServer = INVALID_SOCK_ID;
    m_socketConnectedToAdmin = INVALID_SOCK_ID;


    m_connectToLocalServerTimer = new QTimer(this);
    connect(m_connectToLocalServerTimer, SIGNAL(timeout()), this, SLOT(connectToLocalServer()));
    m_connectToLocalServerTimer->start(60000);


    m_screenshotTimer = 0;
    screen = 0;

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

    if(m_screenshotTimer){
        m_screenshotTimer->stop();
        delete m_screenshotTimer;
        m_screenshotTimer = 0;
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

        connect(bulletinBoardPacketsParser, SIGNAL(signalSystemInfoFromServerReceived(const QString &, const QByteArray &,quint8)), this, SLOT(processSystemInfoFromServer(const QString &, const QByteArray &,quint8)));

        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), this, SLOT(adminRequestRemoteAssistancePacketReceived(const QString&, quint16, const QString&)), Qt::QueuedConnection);
        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), this, SLOT(AdminInformUserNewPasswordPacketReceived(const QString&, quint16, const QString&, const QString&, const QString&)), Qt::QueuedConnection);
//        connect(bulletinBoardPacketsParser, SIGNAL(signalAnnouncementPacketReceived(const QString &, const QString &, quint8, const QString &, bool, int)), this, SLOT(serverAnnouncementPacketReceived(const QString &, const QString &, quint8, const QString &, bool, int)), Qt::QueuedConnection);

        connect(bulletinBoardPacketsParser, SIGNAL(signalAdminRequestScreenshotPacketReceived(SOCKETID, const QString&, const QString&, quint16)), this, SLOT(adminRequestScreenshotPacketReceived(SOCKETID, const QString&, const QString&, quint16)), Qt::QueuedConnection);

    }

    connectToLocalServer();

}

void Helper::processSystemInfoFromServer(const QString &extraInfo, const QByteArray &infoData, quint8 infoType){

    switch (infoType) {
    case quint8(MS::SYSINFO_ANNOUNCEMENTS):
    {
        processAnnouncementsInfo(extraInfo, infoData);
    }
        break;

    default:
        break;
    }

}

void Helper::processAnnouncementsInfo(const QString &userName, const QByteArray &infoData){
    qDebug()<<"--Helper::processAnnouncementsInfo(...)";

    if(m_userName != userName){return;}
    if(infoData.isEmpty()){return;}

    setupBulletinBoardWidget();
    if(!bulletinBoardWidget->processAnnouncementsInfo(infoData)){
        return;
    }
    bulletinBoardWidget->showNormal();
    bulletinBoardWidget->raise();
}

void Helper::processAnnouncementRepliesInfo(const QString &userName, const QByteArray &infoData){
    if(m_userName != userName){return;}
    if(infoData.isEmpty()){return;}

    setupBulletinBoardWidget();
    bulletinBoardWidget->processAnnouncementReplies(infoData);
    bulletinBoardWidget->showNormal();
    bulletinBoardWidget->raise();
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

void Helper::serverAnnouncementPacketReceived(const QString &id, const QString &adminName, quint8 type, const QString &content, bool confirmationRequired, int validityPeriod){
    qWarning()<<"Helper::serverAnnouncementPacketReceived(...)";
    
//    if(!bulletinBoardWidget){
//        bulletinBoardWidget = new BulletinBoardWidget(m_userName);
//        connect(bulletinBoardWidget, SIGNAL(sendReplyMessage(quint32, const QString &)), this, SLOT(sendReplyMessage(quint32, const QString &)));
//    }

//    bulletinBoardWidget->processAnnouncementsInfo(infoData);
//    bulletinBoardWidget->showNormal();
//    bulletinBoardWidget->raise();
}

void Helper::sendReplyMessage(unsigned int originalMessageID, const QString &replyMessage){
    bulletinBoardPacketsParser->sendUserReplyMessagePacket(m_socketConnectedToLocalServer, originalMessageID, "", replyMessage);
}

void Helper::newPasswordRetreved(){

}

void Helper::adminRequestScreenshotPacketReceived(SOCKETID socketID, const QString &adminName, const QString &adminAddress, quint16 adminPort){


    QString errorMessage;
    if(m_socketConnectedToAdmin != INVALID_SOCK_ID){
        m_rtp->closeSocket(m_socketConnectedToAdmin);
    }

    m_socketConnectedToAdmin = m_rtp->connectToHost(QHostAddress(adminAddress), adminPort, 5000, &errorMessage, RTP::TCP);
    if(m_socketConnectedToAdmin == INVALID_SOCK_ID){
        qCritical()<<QString("Can not connect to admin host '%1:%2'.<br>%3").arg(adminAddress).arg(adminPort).arg(errorMessage);
        return;
    }


    screen = QApplication::primaryScreen();
    if (!screen){
        qCritical()<<"ERROR! No primary screen.";
        return;
    }

    QPixmap pixmap = screen->grabWindow(0);
    QImage screenImage = pixmap.toImage();
    if(screenImage.isNull()){
        qCritical()<<"ERROR! Invalid image.";
        return;
    }

    if(!m_screenshotTimer){
        m_screenshotTimer = new QTimer(this);
        connect(m_screenshotTimer, SIGNAL(timeout()), this, SLOT(screenshot()));
    }

    int imageWidth = screenImage.width();
    int imageHeight = screenImage.height();
    int columnCount = 2;
    int rowCount = 2;


    m_blockWidth = imageWidth/columnCount;
    m_blockHeight = imageHeight/rowCount;
    m_blockSize = QSize(m_blockWidth, m_blockHeight);

    if(!bulletinBoardPacketsParser->sendUserDesktopInfoPacket(m_socketConnectedToAdmin, imageWidth, imageHeight, m_blockWidth, m_blockHeight)){
        qCritical()<<"ERROR! Can not send data to peer.";
        return;
    }

    for(int i=0;i<columnCount;i++){
        for(int j=0;j<rowCount;j++){
            QPoint point(i*m_blockWidth, j*m_blockHeight);
            m_locations.append(point);
            //m_images.append(QImage());
            m_imagesHash.append(QByteArray());
            //            QImage image = screenImage.copy(QRect(point, m_blockSize));
            //            m_images.append(image);

            //            QByteArray byteArray;
            //            QBuffer buffer(&byteArray);
            //            buffer.open(QIODevice::WriteOnly);
            //            image.save(&buffer, "jpg");
            //            buffer.close();
            //            m_imagesHash.append(QCryptographicHash::hash(byteArray, QCryptographicHash::Md5));

        }
    }

    m_screenshotTimer->start(1000);

}

void Helper::screenshot(){

    if (!screen){
        qCritical()<<"ERROR! No primary screen.";
        m_screenshotTimer->stop();

        return;
    }

    QPixmap pixmap = screen->grabWindow(0);
    QImage screenImage = pixmap.toImage();
    if(screenImage.isNull()){
        qCritical()<<"ERROR! Invalid image.";
        return;
    }


    QList<QPoint> locations;
    QList<QByteArray> images;

    for(int i=0;i<m_locations.size();i++){
        QPoint point = m_locations.at(i);
        QImage image = screenImage.copy(QRect(point, m_blockSize));

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "jpg");
        buffer.close();
        QByteArray hash = QCryptographicHash::hash(byteArray, QCryptographicHash::Md5);

        if(m_imagesHash.at(i) != hash){
            locations.append(point);
            images.append(byteArray);
            //m_images[i] = image;
            m_imagesHash[i] = hash;
        }

    }

    bulletinBoardPacketsParser->sendUserScreenshotPacket(m_socketConnectedToAdmin, locations, images);


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

    if(socketID == m_socketConnectedToAdmin){
        m_socketConnectedToAdmin = INVALID_SOCK_ID;
        if(m_screenshotTimer){
            m_screenshotTimer->stop();
        }

        screen = 0;
        m_blockWidth = 0;
        m_blockHeight = 0;
        m_blockSize = QSize(0, 0);
        m_locations.clear();
        //m_images.clear();
        m_imagesHash.clear();


    }else{
        m_socketConnectedToLocalServer = INVALID_SOCK_ID;
        if(m_connectToLocalServerTimer){
            m_connectToLocalServerTimer->start();
        }
    }


}

void Helper::connectToLocalServer(){
    qDebug()<<"--BulletinBoardObject::connectToLocalServer()";

    m_rtp->closeSocket(m_socketConnectedToLocalServer);
    QString errorMessage;
    m_socketConnectedToLocalServer = m_rtp->connectToHost(QHostAddress::LocalHost, RTP_LISTENING_PORT, 10000, &errorMessage);
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

void Helper::setupBulletinBoardWidget(){
    if(bulletinBoardWidget){return;}

    bulletinBoardWidget = new BulletinBoardWidget(m_userName);
    connect(bulletinBoardWidget, SIGNAL(sendReplyMessage(const QString &, const QString &)), this, SLOT(sendReplyMessage(const QString &, const QString &)));
}






} //namespace HEHUI
