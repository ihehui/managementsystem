#ifndef ADMINUSER_H
#define ADMINUSER_H

#include "HHSharedCore/UserBase"
#include "HHSharedGUI/LoginDlg"


#include "networkmanager/controlcenterpacketsparser.h"
#include "../../sharedms/rtp.h"


namespace HEHUI
{


class AdminUser : public UserBase
{
    Q_OBJECT

private:
    AdminUser();

public:
    static AdminUser *instance();
    ~AdminUser();

    void init(RTP *rtp, ControlCenterPacketsParser *controlCenterPacketsParser, QObject *parent);

    bool isReadonly();
    bool isAdminVerified();
    SOCKETID socketConnectedToServer();
    ControlCenterPacketsParser *packetsParser();

    QString serverAddress() const;
    quint16 serverPort();
    QString serverName() const;
    QString serverVersion() const;

    void setAboutToQuit(bool quit);

signals:
    void signalVerified();


public slots:
    void verifyUser();
    void modifyServerSettings();
    void serverSelected(const QString &serverAddress, quint16 serverPort, const QString &serverName, const QString &version);
    bool connectToServer(const QString &serverAddress, quint16 serverPort);
    bool login();
    void processLoginResult(const AdminLoginPacket &packet);

    void peerDisconnected(SOCKETID socketID);


private:
    static AdminUser *adminUserInstance;

    RTP *m_rtp;
    ControlCenterPacketsParser *m_controlCenterPacketsParser;

    SOCKETID m_socketConnectedToServer;

    QString m_serverAddress;
    quint16 m_serverPort;
    QString m_serverName;
    QString m_serverVersion;

    LoginDlg *m_loginDlg;

    bool m_readonly;
    bool m_aboutToQuit;



};

} //namespace HEHUI

#endif // ADMINUSER_H
