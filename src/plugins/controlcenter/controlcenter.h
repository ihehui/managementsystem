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

#include "../clientmanagement//clientinfomodel.h"

#include "../../sharedms/clientinfo.h"
#include "../../sharedms/rtp.h"



#include "systemmanagement/systemmanagementwidget.h"
#include "../remotedesktop/remotedesktopmonitor.h"


#include "HHSharedCore/huser.h"
#include "HHSharedCore/hmysharedlib_global.h"
#include "HHSharedGUI/hlogindlg.h"




namespace HEHUI {


class ControlCenter : public QMainWindow
{
    Q_OBJECT

public:
    ControlCenter(const QString appName, QWidget *parent = 0);
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
    void slotNewTab(ClientInfo *info = 0);
    void slotcloseTab();
    void updateWindowTitle();
    void updateTabText(SystemManagementWidget *wgt);

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
    void changProcessMonitorInfo(const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);

    void slotUpdateUserLogonPassword();
    void slotInformUserNewLogonPassword();

    void slotShowCustomContextMenu(const QPoint & pos);

    void startNetwork();


    void showReceivedMessage(const MessagePacket &packet);
    void updateOrSaveClientInfo(const ClientInfoPacket &packet);
    void processSystemInfoFromServer(const SystemInfoFromServerPacket &packet);

    void updateSystemInfoFromServer(const QString &extraInfo, const QByteArray &infoData, quint8 infoType);
    void updateClientInfoFromServer(const QString &assetNO, const QByteArray &infoData, quint8 infoType);

    void processAssetNOModifiedPacket(const ModifyAssetNOPacket &packet);

    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, bool online);

    void processScreenshotPacket(const ScreenshotPacket &packet);
    void closeRemoteDesktopMonitor();
    void closeUserSocket(quint32 userSocketID);


    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(SOCKETID socketID);

    void adminVerified();

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
    QString m_appName;
    QMenu *searchClientsMenu;

    QString localComputerName;

    QString databaseConnectionName;
    QSqlQuery *query;
    ClientInfoModel *clientInfoModel;
    ClientInfoSortFilterProxyModel *proxyModel;

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


    RemoteDesktopMonitor *m_remoteDesktopMonitor;
//    QHash<int/*Socket ID*/, QHostAddress/*IP*/> clientSocketsHash;

    //bool m_readonly;
    AdminUser *m_adminUser;
    
};

} //namespace HEHUI

#endif // CONTROLCENTER_H
