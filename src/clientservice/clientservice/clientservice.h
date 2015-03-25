
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H



#include <QObject>
#include <QSettings>
#include <QTimer>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/clientinfo.h"

#include "packetmanager/clientpacketsparser.h"
#include "../clientresourcesmanager.h"
#include "process.h"
#include "systeminfo.h"




#include "HHSharedService/hservice.h"
#include "HHSharedCore/hdatabaseutility.h"

//#include "HHSharedUDT/hudtprotocolforfiletransmission.h"

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/hwindowsmanagement.h"
    #include "HHSharedWindowsManagement/hhardwaremonitor.h"
    #include "HHSharedWindowsManagement/WinUtilities"

    #ifndef SITOY_USERS_DB_CONNECTION_NAME
    #define SITOY_USERS_DB_CONNECTION_NAME "200.200.200.2/mis/users"
    #endif
#endif



namespace HEHUI {


class ClientService : public Service
{
    Q_OBJECT
public:
    ClientService(int argc, char **argv, const QString &serviceName = "ClientService", const QString &description = "Client service for management system.");
    ~ClientService();

    bool setDeskWallpaper(const QString &wallpaperPath);




signals:


private slots:
    bool startMainService();

    void serverLookedUp(const QHostInfo &host);

    void serverFound(const QString &serverAddress, quint16 serverUDTListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void processClientInfoRequestedPacket(SOCKETID socketID, const QString &computerName, quint8 infoType);
    void systemInfoResultReady(const QByteArray &data, quint8 infoType, SOCKETID socketID);
    void systemInfoThreadFinished();

    void processSetupUSBSDPacket(quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminName);
    void processSetupProgramesPacket(bool enable, bool temporarilyAllowed, const QString &adminName);
    void processShowAdminPacket(bool show);
    void processModifyAdminGroupUserPacket(const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &domainAdminName, const QString &domainAdminPassword);
    void processJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool joinDomain, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword);

    void processAdminRequestConnectionToClientPacket(SOCKETID adminSocketID, const QString &adminComputerName, const QString &adminName);
    void processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
    
//    void processServerAnnouncementPacket(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, const QString &userName, bool mustRead);
//    void processAdminRequestRemoteAssistancePacket(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestUpdateMSUserPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);
    void processAdminRequestInformUserNewPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void processAdminRequestRemoteConsolePacket(const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort);
    void processRemoteConsoleCMDFromServerPacket(const QString &computerName, const QString &command, const QString &adminAddress, quint16 adminPort);

    void consoleProcessStateChanged(bool running, const QString &message);
    void consoleProcessOutputRead(const QString &output);

//    void processLocalUserOnlineStatusChanged(SOCKETID userSocketID, const QString &userName, bool online);

    void processAdminRequestTemperaturesPacket(SOCKETID socketID, bool cpu = true, bool harddisk = false);
//    void processAdminRequestScreenshotPacket(SOCKETID socketID, const QString &userName, bool fullScreen = true);

    void processAdminRequestRequestShutdownPacket(SOCKETID adminSocketID, bool reboot, bool force, quint32 waitTime, const QString &message);
    void processAdminRequestChangeServiceConfigPacket(SOCKETID socketID, const QString &serviceName, bool startService, unsigned long startupType);




    QStringList usersOnLocalComputer();
    void uploadClientSummaryInfo(SOCKETID socketID);
    void uploadClientSummaryInfo(const QString &adminAddress, quint16 adminPort);

//    void uploadClientDetailedInfoToServer();

    void update();
    bool updateAdministratorPassword(const QString &newPassword);
    void setWinAdminPassword(const QString &userPassword);
    QString getWinAdminPassword() const;

    bool checkUsersAccount();

    bool setupUSBStorageDevice(bool readable, bool writeable, bool temporary);
//    bool isUSBSDEnabled();
    MS::USBSTORStatus readUSBStorageDeviceSettings();

    void checkUSBSD();

    bool enableProgrames(bool temporary);
    void disableProgrames();
    bool isProgramesEnabled();
    void checkProgrames();


    QStringList administrators();
    void modifyAdminGroupUser(const QString &userName, bool addToAdminGroup);

    bool setupStartupWithSafeMode(bool startup);

    QString getServerLastUsed() const;
    void setServerLastUsed(const QString &serverAddress);

    void checkHasAnyServerBeenFound();

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(SOCKETID socketID);

///////////////////
    void fileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);
    //File TX
    void startFileManager();
    void processAdminRequestUploadFilePacket(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void processAdminRequestDownloadFilePacket(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);
    void processFileDataRequestPacket(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);


private:
    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);



private:

    ClientResourcesManager *resourcesManager;
    ClientPacketsParser *clientPacketsParser;

    UDPServer *m_udpServer;
    RTP *m_rtp;

//    UDTProtocol *m_udtProtocol;
    SOCKETID m_socketConnectedToServer;
    SOCKETID m_socketConnectedToAdmin;
    SOCKETID peerSocketThatRequiresDetailedInfo;


//    UDTProtocolForFileTransmission *m_udtProtocolForFileTransmission;

//    QHash<int /*UDT Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;

    QSettings *settings;


    QString m_localComputerName;
    bool m_isJoinedToDomain;
    QString m_joinInfo;


#if defined(Q_OS_WIN32)
    WindowsManagement *m_wm;

    HardwareMonitor *m_hardwareMonitor;


#endif


    Process *process;


    QString m_adminAddress;
    quint16 m_adminPort;
    
    QHash<QString/*User Name*/, QString/*Password*/> userPasswordsHash;


    QTimer *lookForServerTimer;

    QHostAddress m_serverAddress;
    quint16 m_serverUDTListeningPort;

    QString m_serverName;
    int m_serverInstanceID;

    FileManager *m_fileManager;
    QHash<int/*File TX Request ID*/, int/*Socket ID*/> fileTXRequestHash;
    QMultiHash<SOCKETID/*Socket ID*/, QByteArray/*File MD5*/> fileTXSocketHash;

    QStringList logs;

    ClientInfo *m_myInfo;
    SystemInfo *systemInfo;


};

} //namespace HEHUI

#endif // CLIENTSERVICE_H
