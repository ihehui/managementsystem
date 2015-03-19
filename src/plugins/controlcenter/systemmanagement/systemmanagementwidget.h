#ifndef SYSTEMMANAGEMENTWIDGET_H
#define SYSTEMMANAGEMENTWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QSqlQueryModel>

#include "ui_systemmanagementwidget.h"
#include "../../sharedms/rtp.h"

#include "networkmanager/controlcenterpacketsparser.h"

#include "../../sharedms/clientinfo.h"



namespace HEHUI {


class SystemManagementWidget : public QWidget
{
    Q_OBJECT

public:
    SystemManagementWidget(RTP *rtp, ControlCenterPacketsParser *parser, const QString &adminName, ClientInfo *clientInfo = 0,  QWidget *parent = 0);
    ~SystemManagementWidget();


    QHostAddress peerIPAddress() const{return m_peerIPAddress;}

    
    
signals:
    void requestRemoteAssistance();

public slots:
    void setRTP(RTP *rtp);
//    void setUDTProtocol(UDTProtocol *udtProtocol);
//    void setTCPServer(TCPServer *tcpServer);
    void setControlCenterPacketsParser(ControlCenterPacketsParser *parser);

protected:
    void closeEvent(QCloseEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dropEvent(QDropEvent *event);


private slots:
    void on_toolButtonVerify_clicked();
    void on_pushButtonUSBSD_clicked();
    void on_pushButtonPrograms_clicked();
    void on_pushButtonShowAdmin_clicked();
    void on_pushButtonRemoteAssistance_clicked();

    void on_actionAddAdmin_triggered();
    void on_actionDeleteAdmin_triggered();

    void getNewComputerName();
    void on_pushButtonRenameComputer_clicked();
    void on_pushButtonDomain_clicked();

    void on_pushButtonMMC_clicked();
    void on_pushButtonCMD_clicked();
    void on_pushButtonRegedit_clicked();
    void on_pushButtonOtherEXE_clicked();

    void on_toolButtonQuerySystemInfo_clicked();
    void on_toolButtonRequestSystemInfo_clicked();
    void on_toolButtonSaveAs_clicked();

    void on_groupBoxTemperatures_clicked(bool checked);
    void requestUpdateTemperatures();

    void on_toolButtonRunRemoteApplication_clicked();
    void on_toolButtonSendCommand_clicked();

    void on_pushButtonRefreshScreenshot_clicked();


    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &computerName, bool online);
    void processClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &computerName, bool result, const QString &message);

    void requestConnectionToClientTimeout();

    void clientMessageReceived(const QString &computerName, const QString &message, quint8 clientMessageType);

    void clientResponseClientSummaryInfoPacketReceived(SOCKETID socketID, const QByteArray &clientInfo);


    void clientDetailedInfoPacketReceived(const QString &computerName, const QByteArray &clientInfo);
    void updateSystemInfo(const QJsonObject &obj);
    void updateSoftwareInfo(const QJsonArray &array);

    void requestClientInfoTimeout();

    void clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage, quint8 messageType);
    void remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);
    void requestRemoteConsoleTimeout();

    void userOnlineStatusChangedPacketReceived(const QString &userName, const QString &computerName, bool online);

    void userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);

    void updateTemperatures(const QString &cpuTemperature, const QString &harddiskTemperature);
    void updateScreenshot(const QString &userName, const QByteArray &screenshot);

    void peerDisconnected(SOCKETID socketID);
    void peerDisconnected(bool normalClose);


private:
    bool verifyPrivilege();
    bool temporarilyAllowed();

    void runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters = "", bool show = true);

    void resetSystemInfo();

private:
    Ui::SystemManagementWidgetClass ui;

    QString m_adminName;
    ClientInfo m_clientInfo;

    QString m_peerComputerName;

    QHostAddress m_peerIPAddress;

    ControlCenterPacketsParser *controlCenterPacketsParser;

//    QString m_users;

//    QString m_peerMACAddress;
//    quint8 m_usbSTORStatus;
//    bool m_programesEnabled;


    bool localComputer;
    bool m_isJoinedToDomain;

    QString m_winDirPath;
    //QString m_joinInfo;

    static QMap<QString/*Short Name*/, QString/*Department*/>departments;

    QMenu *administratorsManagementMenu;

    QStringList m_onlineUsers;


    QStringList adminProcesses;

    bool clientResponseAdminConnectionResultPacketReceived;
    bool remoteConsoleRunning;


    QSqlQueryModel *queryModel;

    RTP *m_rtp;
    SOCKETID m_peerSocket;

    bool m_aboutToCloseSocket;

    //UDPServer *m_udpServer;
//    UDTProtocol *m_udtProtocol;

//    TCPServer *m_tcpServer;
//    bool m_usingTCP;

//    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;
//    UDTSOCKET m_peerFileTransmissionSocket;

//    FileManager *m_fileManager;
//    QList<int/*File TX Request ID*/> fileTXRequestList;
//    QList<QByteArray/*File MD5*/> filesList;

    FileManagement *m_fileManagementWidget;

    QTimer *m_updateTemperaturesTimer;



};

} //namespace HEHUI

#endif // SYSTEMMANAGEMENTWIDGET_H
