#ifndef ADMINUSER_H
#define ADMINUSER_H

#include "HHSharedCore/huserbase.h"
#include "HHSharedGUI/hlogindlg.h"


#include "networkmanager/controlcenterpacketsparser.h"
#include "../../sharedms/rtp.h"


namespace HEHUI {


class AdminUser : public UserBase
{
    Q_OBJECT

private:
     AdminUser();

public:
    static AdminUser * instance();
    ~AdminUser();

    void init(RTP *rtp, ControlCenterPacketsParser *controlCenterPacketsParser, QObject *parent);

    bool isReadonly();
    bool isAdminVerified();
    SOCKETID socketConnectedToServer();


signals:
    void signalVerified();


public slots:
    void verifyUser();
    void modifyServerSettings();
    void serverSelected(const QString &serverAddress, quint16 serverPort);
    bool connectToServer(const QString &serverAddress, quint16 serverPort);
    bool login();
    void processLoginResult(SOCKETID socketID, const QString &serverName, bool result, const QString &message, bool readonly);

    void peerDisconnected(SOCKETID socketID);


private:
    static AdminUser *adminUserInstance;

    RTP *m_rtp;
    ControlCenterPacketsParser *m_controlCenterPacketsParser;

    SOCKETID m_socketConnectedToServer;
    QString m_serverAddress;
    quint16 m_serverPort;

    LoginDlg *m_loginDlg;

    bool m_readonly;



};

} //namespace HEHUI

#endif // ADMINUSER_H
