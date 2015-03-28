#ifndef HELPER_H
#define HELPER_H

#include <QObject>

//#include "systemsummaryinfo.h"
#include "remoteassistance/remoteassistance.h"
#include "bulletinboard/bulletinboardwidget.h"
#include "updatepassword/updatepasswordwidget.h"


#include "networkmanager/bulletinboardpacketsparser.h"
//#include "../../sharedms/networkmanager.h"
#include "networkmanager/resourcesmanagerinstance.h"



namespace HEHUI {


class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(QObject *parent = 0);
    ~Helper();
    

signals:

    
public slots:
    
private slots:
    void startNetwork();

    void adminRequestRemoteAssistancePacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName);
    void AdminInformUserNewPasswordPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &adminName, const QString &oldPassword, const QString &newPassword );
    void serverAnnouncementPacketReceived(const QString &adminName, quint32 announcementID, const QString &announcement);
    
    void newPasswordRetreved();

    void adminRequestScreenshotPacketReceived(SOCKETID adminSocketID);

    
//    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
//    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);

    void peerDisconnected(SOCKETID socketID);
    void connectToLocalServer();
    
private:

    //bool m_networkReady;

    ResourcesManagerInstance *resourcesManager;
    BulletinBoardPacketsParser *bulletinBoardPacketsParser;
    
    RemoteAssistance *remoteAssistance;
    BulletinBoardWidget *bulletinBoardWidget;
    UpdatePasswordWidget *updatePasswordWidget;
    
    quint16 localUDTListeningPort;
    RTP *m_rtp;
    SOCKETID m_socketConnectedToLocalServer;
//    UDTSOCKET m_socketConnectedToAdmin;

    QTimer *m_connectToLocalServerTimer;


};

} //namespace HEHUI 

#endif // HELPER_H