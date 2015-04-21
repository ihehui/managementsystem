#ifndef SYSTEMMANAGEMENTWIDGET_H
#define SYSTEMMANAGEMENTWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>
//#include <QSqlQueryModel>
#include <QHostInfo>

#include "ui_systemmanagementwidget.h"

#include "networkmanager/controlcenterpacketsparser.h"
#include "../adminuser.h"

#include "../../sharedms/clientinfo.h"
#include "../../sharedms/rtp.h"



namespace HEHUI {


class SystemManagementWidget : public QWidget
{
    Q_OBJECT

public:
    SystemManagementWidget(RTP *rtp, ControlCenterPacketsParser *parser, ClientInfo *clientInfo = 0,  QWidget *parent = 0);
    ~SystemManagementWidget();


    QString peerAssetNO() const;

protected:
    void closeEvent(QCloseEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dropEvent(QDropEvent *event);

    
signals:
    void updateTitle(SystemManagementWidget *wgt);
    void signalSetProcessMonitorInfo(const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);


public slots:
    void setRTP(RTP *rtp);
//    void setUDTProtocol(UDTProtocol *udtProtocol);
//    void setTCPServer(TCPServer *tcpServer);
    void setControlCenterPacketsParser(ControlCenterPacketsParser *parser);



private slots:
    void on_toolButtonVerify_clicked();

    void on_toolButtonModifyAssetNO_clicked();
    void on_toolButtonShutdown_clicked();
    void on_toolButtonRenameComputer_clicked();
    void changeWorkgroup();

    void on_toolButtonSetupUSB_clicked();

    void on_pushButtonMMC_clicked();
    void on_pushButtonCMD_clicked();
    void on_pushButtonRegedit_clicked();
    void on_pushButtonOtherEXE_clicked();

    void on_toolButtonQuerySystemInfo_clicked();
    void on_toolButtonRequestHardwareInfo_clicked();
    void on_toolButtonSaveAs_clicked();

    void on_groupBoxTemperatures_clicked(bool checked);
    void requestUpdateTemperatures();

    void on_toolButtonRunRemoteApplication_clicked();
    void on_toolButtonSendCommand_clicked();

    void targetHostLookedUp(const QHostInfo &host);

    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &assetNO, bool online);
    void processClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &assetNO, const QString &computerName, bool result, const QString &message, const QString &clientIP);

    void requestConnectionToClientTimeout();

    void clientMessageReceived(const QString &assetNO, const QString &message, quint8 clientMessageType);

    void requestClientInfo(quint8 infoType);
    void clientInfoPacketReceived(const QString &assetNO, const QByteArray &data, quint8 infoType);
    void updateOSInfo();
    void updateHardwareInfo();


    void processAssetNOModifiedPacket(const QString &oldAssetNO, const QString &newAssetNO, bool modified, const QString &message);
    void modifyAssetNOTimeout();

    void changServiceConfig(const QString &serviceName, bool startService, quint64 startupType);
    void changProcessMonitorInfo(const QByteArray &rulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);



    void requestCreateOrModifyWinUser(const QByteArray &userData);
    void requestDeleteUser(const QString &userName);

    void requestSendMessageToUser(const QString &userName);
    void requestMonitorUserDesktop(const QString &userName);

    //void sendMessageToUser(quint32 messageID, const QString &message, bool confirmationRequired, int validityPeriod);
    void requestLockWindows(const QString &userName, bool logoff);

    void requestClientInfoTimeout();

    void clientResponseRemoteConsoleStatusPacketReceived(const QString &assetNO, bool running, const QString &extraMessage, quint8 messageType);
    void remoteConsoleCMDResultFromClientPacketReceived(const QString &assetNO, const QString &result);
    void requestRemoteConsoleTimeout();

    void userOnlineStatusChangedPacketReceived(const QString &assetNO, const QString &userName, bool online);

    void userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);

    void updateTemperatures(const QString &assetNO, const QString &cpuTemperature, const QString &harddiskTemperature);
    void replyMessageReceived(const QString &assetNO, const QString &userName, quint32 originalMessageID, const QString &replyMessage);

    void serviceConfigChangedPacketReceived(const QString &assetNO, const QString &serviceName, quint64 processID, quint64 startupType);

    void peerDisconnected(SOCKETID socketID);
    void peerDisconnected(bool normalClose);


private:
    bool verifyPrivilege();
    bool canModifySettings();

    bool temporarilyAllowed();

    void runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters = "", bool show = true);

    void resetSystemInfo();

private:
    Ui::SystemManagementWidgetClass ui;

    QMenu *m_joinWorkgroupMenu;


    RTP *m_rtp;
    SOCKETID m_peerSocket;
    bool m_aboutToCloseSocket;

    ControlCenterPacketsParser *controlCenterPacketsParser;
    AdminUser *m_adminUser;
    ClientInfo m_clientInfo;

    QString m_peerAssetNO;
    QString m_peerComputerName;
    QHostAddress m_peerIPAddress;

    bool localComputer;
    bool m_isJoinedToDomain;

    QString m_winDirPath;

    QMenu *administratorsManagementMenu;

    bool clientResponseAdminConnectionResultPacketReceived;
    bool remoteConsoleRunning;

    FileManagement *m_fileManagementWidget;

    QTimer *m_updateTemperaturesTimer;


    QStringList m_onlineUsers;
    QStringList adminProcesses;

};

} //namespace HEHUI

#endif // SYSTEMMANAGEMENTWIDGET_H
