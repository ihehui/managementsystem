#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QProcess>

#include "ui_controlcenter.h"
#include "adminuser.h"
#include "networkmanager/controlcenterpacketsparser.h"
#include "networkmanager/resourcesmanagerinstance.h"

#include "../clientinfomodel/clientinfomodel.h"

#include "../../sharedms/clientinfo.h"
#include "../../sharedms/rtp.h"



#include "systemmanagement/systemmanagementwidget.h"
#include "../remotedesktop/remotedesktopmonitor.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedGUI/hprogressdlg.h"
#include "HHSharedGUI/hlogindlg.h"




namespace HEHUI {


class ControlCenter : public QMainWindow
{
    Q_OBJECT

public:
    ControlCenter(QWidget *parent = 0);
    ~ControlCenter();

    static bool isRunning(){return running;}

    void retranslateUi();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void languageChange();
    void closeEvent(QCloseEvent *e);

public slots:


private slots:
    void slotInitTabWidget();
    void slotTabPageChanged();
    void slotNewTab();
    void slotcloseTab();

    void slotRemoteManagement(const QModelIndex &index = QModelIndex());

    void slotQueryDatabase();
    void slotSearchNetwork();
    void filter();
    //    void slotUpdateButtonClicked();

    void slotShowClientInfo(const QModelIndex &index);
    void slotExportQueryResult();
    void slotPrintQueryResult();

    void slotRemoteDesktop();
    void slotVNC();
    void updateTitle(SystemManagementWidget *wgt);
    void changProcessMonitorInfo(const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &computerName);

    void slotUpdateUserLogonPassword();
    void slotInformUserNewLogonPassword();
    void slotSendAnnouncement();
    void slotSendAnnouncement(quint32 messageID, const QString &message, bool confirmationRequired,  int validityPeriod);

    void slotShowCustomContextMenu(const QPoint & pos);

    void startNetwork();

    void serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void updateOrSaveClientInfo(const QString &assetNO, const QByteArray &clientInfoData, quint8 infoType);
    void processSystemInfoFromServer(const QString &assetNO, const QByteArray &infoData, quint8 infoType);

    void processAssetNOModifiedPacket(const QString &newAssetNO, const QString &oldAssetNO, bool modified, const QString &message);

    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, bool online);

    void processDesktopInfo(quint32 userSocketID, const QString &userID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight);
    void processScreenshot(const QString &userID, QList<QPoint> locations, QList<QByteArray> images);
    void closeRemoteDesktopMonitor();
    void closeUserSocket(quint32 userSocketID);


    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(SOCKETID socketID);

    void adminVerified();

//    bool isAdminVerified();
//    void verifyUser();
//    void modifyServerSettings();
//    void serverSelected(const QString &serverAddress, quint16 serverPort);
//    bool connectToServer(const QString &serverAddress, quint16 serverPort);
//    bool login();
//    void processLoginResult(SOCKETID socketID, const QString &serverName, bool result, const QString &message, bool readonly);


private:
    bool openDatabase(bool reopen = false);
    bool openDatabase(QSqlDatabase *database, bool reopen = false, QString *errorString = 0);
    bool execQuery(const QString &statement );

    QString assetNO() const;
    QString computerName() const;
    QString osVersion() const;
    QString workgroup() const;
    QString userName() const;
    QString ipAddress() const;
    int procMonEnabled() const;
    int usbsdStatus();

    void updateActions();

private:
    Ui::ControlCenterClass ui;
    QMenu *searchClientsMenu;

    QString localComputerName;
    SystemManagementWidget *localSystemManagementWidget;

    QString databaseConnectionName;
    QSqlQuery *query;
    ClientInfoModel *clientInfoModel;
    ClientInfoSortFilterProxyModel *proxyModel;

    //QWidget *m_progressWidget;
    //QLayout* hlayout;
    //QLabel *label;
    //QProgressBar *progressBar;

    static bool running;

    QProcess *vncProcess;

    bool m_networkReady;

    ResourcesManagerInstance *resourcesManager;
    ControlCenterPacketsParser *controlCenterPacketsParser;
    UDPServer *m_udpServer;
    quint16 m_localUDPListeningPort;

    RTP *m_rtp;
    quint16 m_localRTPListeningPort;
    SOCKETID m_socketConnectedToServer;
    //QString m_serverAddress;
    //quint16 m_serverPort;
    //LoginDlg *m_loginDlg;

    RemoteDesktopMonitor *m_remoteDesktopMonitor;
//    QHash<int/*Socket ID*/, QHostAddress/*IP*/> clientSocketsHash;

    //bool m_readonly;
    AdminUser *m_adminUser;
    
};

} //namespace HEHUI

#endif // CONTROLCENTER_H
