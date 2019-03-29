
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
#include "processrunner.h"
#include "systeminfo.h"
#include "../processmonitor/processmonitor.h"



#include "HHSharedService/Service"
#include "HHSharedCore/DatabaseUtility"
#include "HHSharedCore/Cryptography"
#include "HHSharedSystemUtilities/SystemUtilities"




namespace HEHUI
{


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

    bool isPacketFromOnlinePeer(const MSPacket *packet, bool adminPacket);
    bool isOnlineAdminPacket(const MSPacket *packet, bool sendErrorMessageBack = true);


    void serverFound(const ServerDiscoveryPacket &packet);

    void processSetupUSBSDPacket(const USBDevPacket &packet);

    void processModifyAssetNOPacket(const ModifyAssetNOPacket &packet);
    void modifyAssetNOTimeout();

    void processClientInfoRequestedPacket(const ClientInfoPacket &packet);
    void processClientInfoRequestedPacket(SOCKETID socketID, quint8 infoType);
    void systemInfoResultReady(const QByteArray &data, quint8 infoType, SOCKETID socketID);
    void systemInfoThreadFinished();

    void processSystemInfoFromServer(const SystemInfoFromServerPacket &packet);

    void processAdminRequestRemoteConsolePacket(const QString &applicationPath, bool startProcess);
    void processRemoteConsoleCMDFromServerPacket(const QString &command);
    void processRemoteConsolePacket(const RemoteConsolePacket &packet);

    void processRenameComputerPacketReceived(const RenameComputerPacket &packet);
    void processJoinOrUnjoinDomainPacketReceived(const JoinOrUnjoinDomainPacket &packet);

    void processAdminRequestConnectionToClientPacket(const AdminConnectionToClientPacket &packet);
    void processAdminSearchClientPacket(const AdminSearchClientPacket &packet);



    void processShowAdminPacket(bool show);



    void consoleProcessStateChanged(bool running, const QString &message);
    void consoleProcessOutputRead(const QString &output);

    void processAdminRequestTemperaturesPacket(const TemperaturesPacket &packet);

    void processAdminRequestShutdownPacket(const ShutdownPacket &packet);
    void processAdminRequestLockWindowsPacket(const LockWindowsPacket &packet);

    void processAdminRequestChangeServiceConfigPacket(const ServiceConfigPacket &packet);

    void processWinUserPacket(const WinUserPacket &packet);

    void processAdminRequestDeleteUserPacket(SOCKETID adminSocketID, const QString &userName);

    void processRequestChangeProcessMonitorInfoPacket(const ProcessMonitorInfoPacket &packet);
    void initProcessMonitorInfo();


    QStringList usersOnLocalComputer();
    void uploadClientOSInfo(SOCKETID socketID);
    void uploadClientOSInfo(const QString &adminAddress, quint16 adminPort);

//    void uploadClientDetailedInfoToServer();


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

    bool getServerLastUsed(QString *ip, quint16 *port, QString *name);
    void setServerLastUsed(const QString &serverAddress, quint16 serverPort, const QString &serverName);

    void uploadSoftwareInfo();

    void checkHasAnyServerBeenFound();

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(SOCKETID socketID);

///////////////////

    void processFileTransferPacket(const FileTransferPacket &packet);

    void fileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);
    //File TX
    void startFileManager();
    void processAdminRequestUploadFilePacket(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void processAdminRequestDownloadFilePacket(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &dirToSaveFile);
    void processFileDataRequestPacket(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);

    void deleteFiles(SOCKETID socketID, const QString &localBaseDir, const QStringList files);
    void deleteLocalFiles(const QString &path, QStringList *failedFiles = 0, const QStringList &nameFilters = QStringList(), const QStringList &ignoredFiles = QStringList(), const QStringList &ignoredDirs = QStringList());
    void renameFile(SOCKETID socketID, const QString &localBaseDir, const QString &oldFileName, const QString &newFileName);

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
    int m_adminToken;
    int m_myToken;


    QTimer *lookForServerTimer;

    QHostAddress m_serverAddress;
    quint16 m_serverRTPListeningPort;

    QString m_serverName;
    int m_serverInstanceID;

    FileManager *m_fileManager;
    QHash<int/*File TX Request ID*/, int/*Socket ID*/> fileTXRequestHash;
    QMultiHash<SOCKETID/*Socket ID*/, QByteArray/*File MD5*/> fileTXSocketHash;

    QStringList logs;

    bool m_procMonEnabled;

#ifdef Q_OS_WIN
    ProcessMonitor *m_processMonitor;
#endif

    ClientInfo *m_myInfo;
    SystemInfo *systemInfo;






};

} //namespace HEHUI

#endif // CLIENTSERVICE_H
