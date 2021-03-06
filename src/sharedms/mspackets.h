#ifndef MSPACKETS_H_
#define MSPACKETS_H_

#include <QPoint>


#include "sharedmslib.h"
#include "HHSharedNetwork/PacketBase"

namespace HEHUI
{


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API MSPacket : public Packet
{
public:
    MSPacket(quint8 packetType, bool encrypted = true);
//    MSPacket(const PacketBase &base, quint8 packetType, bool encrypted = true);
    virtual ~MSPacket();

public:
    static void setSessionEncryptionKey(const QByteArray &key);

private:
    void init();
    virtual void parsePacketBody(QByteArray &packetBody) = 0;
    virtual QByteArray packBodyData() = 0;

protected:
    QByteArray encrypt(const QByteArray &data);
    QByteArray decrypt(const QByteArray &encryptedData);

private:
    static QByteArray sessionEncryptionKey;

};
////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ServerDiscoveryPacket : public MSPacket
{
public:
    ServerDiscoveryPacket();
    ServerDiscoveryPacket(const PacketBase &base);
    ~ServerDiscoveryPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 responseFromServer;
    QString version;
    quint16 udpPort;
    quint16 rtpPort;
    quint16 tcpPort;
    int serverInstanceID;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API DataForwardPacket : public MSPacket
{
public:
    DataForwardPacket();
    DataForwardPacket(const PacketBase &base);
    ~DataForwardPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QByteArray data;
    QString peer; //sender or receiver ID
    quint8 isRequest;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API MessagePacket : public MSPacket
{
public:
    MessagePacket();
    MessagePacket(const PacketBase &base);
    ~MessagePacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 msgType;
    QString message;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API JobProgressPacket : public MSPacket
{
public:
    JobProgressPacket();
    JobProgressPacket(const PacketBase &base);
    ~JobProgressPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint32 jobID;
    quint8 result;
    QVariant extraData;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API AdminLoginPacket : public MSPacket
{
public:
    AdminLoginPacket();
    AdminLoginPacket(const PacketBase &base);
    ~AdminLoginPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {LOGIN_UNKNOWN = 0, LOGIN_REQUEST, LOGIN_RESULT};

    PacketInfoType InfoType;

    struct LoginInfoStruct {
        QString adminID;
        QString password;
        QString computerName;
    } LoginInfo;

    struct LoginResultStruct {
        quint8 loggedIn;
        QString message;
        quint8 readonly;
    } LoginResult;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ClientInfoPacket : public MSPacket
{
public:
    ClientInfoPacket();
    ClientInfoPacket(const PacketBase &base);
    ~ClientInfoPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 IsRequest;
    QString assetNO;
    quint8 infoType;
    QByteArray data;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API SystemInfoFromServerPacket : public MSPacket
{
public:
    SystemInfoFromServerPacket();
    SystemInfoFromServerPacket(const PacketBase &base);
    ~SystemInfoFromServerPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 infoType;
    QByteArray data;
    QString extraInfo;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API SysAdminInfoPacket : public MSPacket
{
public:
    SysAdminInfoPacket();
    SysAdminInfoPacket(const PacketBase &base);
    ~SysAdminInfoPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString adminID;
    QByteArray data;
    quint8 deleteAdmin;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API SystemAlarmsPacket : public MSPacket
{
public:
    SystemAlarmsPacket();
    SystemAlarmsPacket(const PacketBase &base);
    ~SystemAlarmsPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {SYSTEMALARMS_UNKNOWN = 0, SYSTEMALARMS_QUERY, SYSTEMALARMS_ACK};

    PacketInfoType InfoType;

    struct QueryInfoStruct {
        QString assetNO;
        QString type;
        QString acknowledged;
        QString startTime;
        QString endTime;
    } QueryInfo;

    struct ACKInfoStruct {
        QString alarms;
        quint8 deleteAlarms;
    } ACKInfo;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API AnnouncementPacket : public MSPacket
{
public:
    AnnouncementPacket();
    AnnouncementPacket(const PacketBase &base);
    ~AnnouncementPacket();

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {ANNOUNCEMENT_UNKNOWN = 0, ANNOUNCEMENT_QUERY, ANNOUNCEMENT_CREATE, ANNOUNCEMENT_UPDATE, ANNOUNCEMENT_REPLY, ANNOUNCEMENT_QUERY_TARGETS};
    PacketInfoType InfoType;
    quint32 JobID;

    struct QueryInfoStruct {
        QString announcementID;
        QString keyword;
        QString validity;
        QString assetNO;
        QString userName;
        QString target;
        QString startTime;
        QString endTime;
    } QueryInfo;

    struct CreateInfoStruct {
        unsigned int localTempID;
        QString adminID;
        quint8 type;
        QString content;
        quint8 confirmationRequired;
        int validityPeriod;
        quint8 targetType;
        QString targets;
    } CreateInfo;

    struct UpdateInfoStruct {
        QString adminName;
        unsigned int announcementID;
        quint8 targetType;
        quint8 active;
        QString addedTargets;
        QString deletedTargets;
    } UpdateInfo;

    struct ReplyInfoStruct {
        unsigned int announcementID;
        QString sender;
        QString receiver;
        QString receiversAssetNO;
        QString replyMessage;
    } ReplyInfo;

    struct QueryTargetsInfoStruct {
        unsigned int announcementID;
    } QueryTargetsInfo;


};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API RemoteConsolePacket : public MSPacket
{
public:
    RemoteConsolePacket();
    RemoteConsolePacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {REMOTECONSOLE_UNKNOWN = 0, REMOTECONSOLE_OPEN, REMOTECONSOLE_STATE, REMOTECONSOLE_COMMAND, REMOTECONSOLE_OUTPUT};

    PacketInfoType InfoType;

    struct OpenConsoleStruct {
        QString applicationPath;
        quint8 startProcess;
    } OpenConsole;

    struct ConsoleStateStruct {
        quint8 isRunning;
        QString message;
        quint8 messageType;
    } ConsoleState;

    struct CommandStruct {
        QString command;
    } Command;

    struct OutputStruct {
        QString output;
    } Output;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ClientLogPacket : public MSPacket
{
public:
    ClientLogPacket();
    ClientLogPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 logType;
    QString log;
    qint64 timeToSecsSinceEpoch;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API USBDevPacket : public MSPacket
{
public:
    USBDevPacket();
    USBDevPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 usbSTORStatus;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API AdminConnectionToClientPacket : public MSPacket
{
public:
    AdminConnectionToClientPacket();
    AdminConnectionToClientPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {
        ADMINCONNECTION_UNKNOWN = 0,

        ADMINCONNECTION_ADMIN_ASK_SERVER_AUTH,
        ADMINCONNECTION_SERVER_ASK_CLIENT_AUTH,

        ADMINCONNECTION_RESPONSE_AUTH,

        ADMINCONNECTION_CONNECTION_REQUEST,
        ADMINCONNECTION_CONNECTION_RESULT
    };

    PacketInfoType InfoType;

    QString adminID;
    QString clientID;
    int adminToken;
    int clientToken;
    quint8 ok;
    QString hostName;
    quint8 errorCode;
    QString errorMessage;


};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API AdminSearchClientPacket : public MSPacket
{
public:
    AdminSearchClientPacket();
    AdminSearchClientPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString computerName;
    QString userName;
    QString workgroup;
    QString macAddress;
    QString ipAddress;
    QString osVersion;
    QString adminID;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API LocalUserOnlineStatusChangedPacket : public MSPacket
{
public:
    LocalUserOnlineStatusChangedPacket();
    LocalUserOnlineStatusChangedPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString userName;
    quint8 online;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API FileTransferPacket : public MSPacket
{
public:
    FileTransferPacket();
    FileTransferPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {
        FT_UNKNOWN = 0,
        FT_FileSystemInfoRequest,
        FT_FileSystemInfoResponse,

        FT_FileDeletingRequest,
        FT_FileDeletingResponse,

        FT_FileRenamingRequest,
        FT_FileRenamingResponse,

        FT_FileDownloadingRequest,
        FT_FileDownloadingResponse,

        FT_FileUploadingRequest,
        FT_FileUploadingResponse,

        FT_FileDataRequest,
        FT_FileData,

        FT_FileTXStatus,
        FT_FileTXError
    };
    PacketInfoType InfoType;

    struct FileSystemInfoRequestStruct {
        QString parentDirPath;
    } FileSystemInfoRequest;
    struct FileSystemInfoResponseStruct {
        QString baseDirPath;
        QByteArray fileSystemInfoData;
    } FileSystemInfoResponse;

    struct FileDeletingRequestStruct {
        QString baseDirPath;
        QStringList files;
    } FileDeletingRequest;
    struct FileDeletingResponseStruct {
        QString baseDirPath;
        QStringList failedFiles;
    } FileDeletingResponse;

    struct FileRenamingRequestStruct {
        QString baseDirPath;
        QString oldFileName;
        QString newFileName;
    } FileRenamingRequest;
    struct FileRenamingResponseStruct {
        QString baseDirPath;
        QString oldFileName;
        QString newFileName;
        quint8 renamed;
        QString message;
    } FileRenamingResponse;

    struct FileDownloadingRequestStruct {
        QString baseDir;
        QString fileName;
        QString dirToSaveFile;
    } FileDownloadingRequest;
    struct FileDownloadingResponseStruct {
        quint8 accepted;
        QString baseDir;
        QString fileName;
        QByteArray fileMD5Sum;
        quint64 size;
        QString pathToSaveFile;
        quint8 errorCode;
    } FileDownloadingResponse;

    struct FileUploadingRequestStruct {
        QString fileName;
        QByteArray fileMD5Sum;
        quint64 size;
        QString fileSaveDir;
    } FileUploadingRequest;
    struct FileUploadingResponseStruct {
        QByteArray fileMD5Sum;
        quint8 accepted;
        QString message;
    } FileUploadingResponse;

    struct FileDataRequestStruct {
        QByteArray fileMD5;
        int startPieceIndex;
        int endPieceIndex;
    } FileDataRequest;
    struct FileDataResponseStruct {
        QByteArray fileMD5;
        int pieceIndex;
        QByteArray data;
        QByteArray pieceMD5;
    } FileDataResponse;

    struct FileTXStatusStruct {
        QByteArray fileMD5;
        quint8 status;
    } FileTXStatus;

    struct FileTXErrorStruct {
        QByteArray fileMD5;
        quint8 errorCode;
        QString message;
    } FileTXError;


};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ModifyAssetNOPacket : public MSPacket
{
public:
    ModifyAssetNOPacket();
    ModifyAssetNOPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    quint8 isRequest;
    QString oldAssetNO;
    QString newAssetNO;
    QString adminID;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API RenameComputerPacket : public MSPacket
{
public:
    RenameComputerPacket();
    RenameComputerPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString assetNO;
    QString newComputerName;
    QString domainAdminName;
    QString domainAdminPassword;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API JoinOrUnjoinDomainPacket : public MSPacket
{
public:
    JoinOrUnjoinDomainPacket();
    JoinOrUnjoinDomainPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString assetNO;
    QString domainOrWorkgroupName;
    QString domainAdminName;
    QString domainAdminPassword;
    quint8 joinWorkgroup;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API TemperaturesPacket : public MSPacket
{
public:
    TemperaturesPacket();
    TemperaturesPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {TEMPERATURES_UNKNOWN = 0, TEMPERATURES_REQUEST, TEMPERATURES_RESPONSE};

    PacketInfoType InfoType;

    struct TemperaturesRequestStruct {
        quint8 requestCPU;
        quint8 requestHD;
    } TemperaturesRequest;

    struct TemperaturesResponseStruct {
        QString cpuTemperature;
        QString harddiskTemperature;
    } TemperaturesResponse;


};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ScreenshotPacket : public MSPacket
{
public:
    ScreenshotPacket();
    ScreenshotPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    enum PacketInfoType {SCREENSHOT_UNKNOWN = 0, SCREENSHOT_REQUEST, SCREENSHOT_DESKTOP_INFO, SCREENSHOT_DATA};
    PacketInfoType InfoType;

    struct ScreenshotRequestStruct {
        QString adminID;
        QString userName;
        QString adminAddress;
        quint16 adminListeningPort;
    } ScreenshotRequest;

    struct DesktopInfoStruct {
        int desktopWidth;
        int desktopHeight;
        int blockWidth;
        int blockHeight;
    } DesktopInfo;

    struct ScreenshotDataStruct {
        QList<QPoint> locations;
        QList<QByteArray> images;
    } ScreenshotData;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ShutdownPacket : public MSPacket
{
public:
    ShutdownPacket();
    ShutdownPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString message;
    quint32 waitTime;
    quint8 force;
    quint8 reboot;

};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API LockWindowsPacket : public MSPacket
{
public:
    LockWindowsPacket();
    LockWindowsPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString userName;
    quint8 logoff;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API WinUserPacket : public MSPacket
{
public:
    WinUserPacket();
    WinUserPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QByteArray userData;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ServiceConfigPacket : public MSPacket
{
public:
    ServiceConfigPacket();
    ServiceConfigPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QString serviceName;
    quint64 startupType;
    quint8  startService;
    quint64 processID;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
class SHAREDMSLIB_API ProcessMonitorInfoPacket : public MSPacket
{
public:
    ProcessMonitorInfoPacket();
    ProcessMonitorInfoPacket(const PacketBase &base);

private:
    void init();
    void parsePacketBody(QByteArray &packetBody);
    QByteArray packBodyData();

public:
    QByteArray localRules,  globalRules;
    quint8 enableProcMon;
    quint8 enablePassthrough;
    quint8 enableLogAllowedProcess;
    quint8 enableLogBlockedProcess;
    quint8 useGlobalRules;
    QString assetNO;
};
////////////////////////////////////////////////////////////////////////




} //namespace HEHUI

#endif // MSPACKETS_H_
