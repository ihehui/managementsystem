
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H



#include <QObject>
#include <QSettings>
#include <QTimer>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/clientinfo.h"
#include "../../sharedms/settings.h"


#include "packetmanager/clientpacketsparser.h"
#include "../clientresourcesmanager.h"
#include "process.h"
#include "systeminfo.h"
#include "../processmonitor/processmonitor.h"



#include "HHSharedService/hservice.h"
#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedCore/hcryptography.h"


//#include "HHSharedUDT/hudtprotocolforfiletransmission.h"

#ifdef Q_OS_WIN32
    #include "HHSharedSystemUtilities//hhardwaremonitor.h"
    #include "HHSharedSystemUtilities/WinUtilities"
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

    void serverFound(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void processClientInfoRequestedPacket(SOCKETID socketID, quint8 infoType);
    void systemInfoResultReady(const QByteArray &data, quint8 infoType, SOCKETID socketID);
    void systemInfoThreadFinished();

    void processSetupUSBSDPacket(quint8 usbSTORStatus);
    void processShowAdminPacket(bool show);

    void processModifyAssetNOPacket(const QString &newAssetNO);
    void processAssetNOModifiedPacket(const QString &newAssetNO);
    void modifyAssetNOTimeout();

    void processRenameComputerPacketReceived(const QString &newComputerName, const QString &domainAdminName, const QString &domainAdminPassword);
    void processJoinOrUnjoinDomainPacketReceived(bool joinDomain, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword);

    void processAdminRequestConnectionToClientPacket(SOCKETID adminSocketID, const QString &adminComputerName, const QString &adminID);
    void processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    

    void processAdminRequestRemoteConsolePacket(const QString &applicationPath, bool startProcess);
    void processRemoteConsoleCMDFromServerPacket(const QString &command);

    void consoleProcessStateChanged(bool running, const QString &message);
    void consoleProcessOutputRead(const QString &output);

//    void processLocalUserOnlineStatusChanged(SOCKETID userSocketID, const QString &userName, bool online);

    void processAdminRequestTemperaturesPacket(SOCKETID socketID, bool cpu = true, bool harddisk = false);
//    void processAdminRequestScreenshotPacket(SOCKETID socketID, const QString &userName, bool fullScreen = true);

    void processAdminRequestShutdownPacket(SOCKETID adminSocketID, const QString &message, quint32 waitTime, bool force, bool reboot);
    void processAdminRequestLockWindowsPacket(SOCKETID adminSocketID, const QString &userName, bool logoff);
    void processAdminRequestCreateOrModifyWinUserPacket(SOCKETID adminSocketID, const QByteArray &userData);
    void processAdminRequestDeleteUserPacket(SOCKETID adminSocketID, const QString &userName);

    void processAdminRequestChangeServiceConfigPacket(SOCKETID socketID, const QString &serviceName, bool startService, unsigned long startupType);
    void processRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules);
    void initProcessMonitorInfo();


    QStringList usersOnLocalComputer();
    void uploadClientOSInfo(SOCKETID socketID);
    void uploadClientOSInfo(const QString &adminAddress, quint16 adminPort);

//    void uploadClientDetailedInfoToServer();

    void processSystemInfoFromServer(const QString &extraInfo, const QByteArray &infoData, quint8 infoType);

    void update();
    bool updateAdministratorPassword(const QString &newPassword);
    void setWinAdminPassword(const QString &userPassword);
    QString getWinAdminPassword() const;


    bool setupUSBStorageDevice(bool readable, bool writeable, bool temporary);
//    bool isUSBSDEnabled();
    MS::USBSTORStatus readUSBStorageDeviceSettings();

    void checkUSBSD();


    QStringList administrators();
    void modifyAdminGroupUser(const QString &userName, bool addToAdminGroup);

    bool setupStartupWithSafeMode(bool startup);

    bool getServerLastUsed(QString *ip, quint16 *port = 0);
    void setServerLastUsed(const QString &serverAddress, quint16 serverPort = 0);

    void uploadSoftwareInfo();

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
    void processAdminRequestDownloadFilePacket(SOCKETID socketID, const QString &localBaseDir, const QString &fileName);
    void processFileDataRequestPacket(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);


private:
    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);

    void getLocalAssetNO(QString *newAssetNOToBeUsed = 0);
    void setLocalAssetNO(const QString &assetNO, bool tobeModified = false);


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

    void processArguments(int argc, char **argv);


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

    Settings *settings;
    QByteArray m_encryptionKey;

    QString m_localAssetNO;
    QString m_localComputerName;

    bool m_isJoinedToDomain;
    QString m_joinInfo;


#if defined(Q_OS_WIN32)

    HardwareMonitor *m_hardwareMonitor;


#endif


    Process *process;


    QString m_adminAddress;
    quint16 m_adminPort;
    QString m_adminID;
    

    QTimer *lookForServerTimer;

    QHostAddress m_serverAddress;
    quint16 m_serverUDTListeningPort;

    QString m_serverName;
    int m_serverInstanceID;

    FileManager *m_fileManager;
    QHash<int/*File TX Request ID*/, int/*Socket ID*/> fileTXRequestHash;
    QMultiHash<SOCKETID/*Socket ID*/, QByteArray/*File MD5*/> fileTXSocketHash;

    QStringList logs;

    bool m_procMonEnabled;
    ProcessMonitor *m_processMonitor;

    ClientInfo *m_myInfo;
    SystemInfo *systemInfo;






};

} //namespace HEHUI

#endif // CLIENTSERVICE_H
