#include "mspackets.h"

#include "global_shared.h"
#include "HHSharedNetwork/PacketBase"
#include "HHSharedCore/hcryptography.h"


namespace HEHUI
{

static Cryptography cryptography;


////////////////////////////////////////////////////////////////////////

QByteArray MSPacket::sessionEncryptionKey = QByteArray("HE.HUI");
MSPacket::MSPacket(quint8 packetType, bool encrypted)
    : Packet(packetType)
{
    setEncrypted(encrypted);
    init();
}

//MSPacket::MSPacket(const PacketBase &base, quint8 packetType, bool encrypted)
//    : Packet(packetType)
//{
//    setEncrypted(encrypted);
//    init();

//    convert(base);
//}

MSPacket::~MSPacket()
{

}

void MSPacket::setSessionEncryptionKey(const QByteArray &key)
{
    sessionEncryptionKey = key;
}

void MSPacket::init()
{
    //qDebug()<<"MSPacket::init()";
}

QByteArray MSPacket::encrypt(const QByteArray &data)
{
    if(!isEncrypted() || sessionEncryptionKey.isEmpty()) {
        return data;
    }
//    Cryptography cryptography;
    QByteArray encryptedData;
    if(cryptography.teaCrypto(&encryptedData, data, sessionEncryptionKey, true)) {
        return encryptedData;
    }
    return QByteArray();
}

QByteArray MSPacket::decrypt(const QByteArray &encryptedData)
{
    if(!isEncrypted() || sessionEncryptionKey.isEmpty()) {
        return encryptedData;
    }
//    Cryptography cryptography;
    QByteArray decryptedData;
    if(cryptography.teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false)) {
        return decryptedData;
    }
    return QByteArray();
}


////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////
ServerDiscoveryPacket::ServerDiscoveryPacket()
    : MSPacket(quint8(MS::CMD_ServerDiscovery), false)
{
    init();
}

ServerDiscoveryPacket::ServerDiscoveryPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_JobProgress), false)
{
    init();
    fromPacket(base);
}

ServerDiscoveryPacket::~ServerDiscoveryPacket()
{

}

void ServerDiscoveryPacket::init()
{
    responseFromServer = 0;
    version = "";
    udpPort = 0;
    rtpPort = 0;
    tcpPort = 0;
    serverInstanceID = 0;
}

void ServerDiscoveryPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> responseFromServer >> version;
    if(responseFromServer) {
        in >>  rtpPort >> tcpPort >> serverInstanceID;
    } else {
        in >> udpPort;
    }

}

QByteArray ServerDiscoveryPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << responseFromServer << version;
    if(responseFromServer) {
        out << rtpPort << tcpPort << serverInstanceID;
    } else {
        out << udpPort;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
MessagePacket::MessagePacket()
    : MSPacket(quint8(MS::CMD_Message))
{
    init();
}

MessagePacket::MessagePacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_Message))
{
    init();
    fromPacket(base);
}

MessagePacket::~MessagePacket()
{

}

void MessagePacket::init()
{
    msgType = quint8(MS::MSG_Information);
    message = "";
}

void MessagePacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> msgType >> message;
}

QByteArray MessagePacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << msgType << message;
    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
JobProgressPacket::JobProgressPacket()
    : MSPacket(quint8(MS::CMD_JobProgress))
{
    init();
}

JobProgressPacket::JobProgressPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_JobProgress))
{
    init();
    fromPacket(base);
}

JobProgressPacket::~JobProgressPacket()
{

}

void JobProgressPacket::init()
{
    jobID = 0;
    result = 0;
    extraData = QVariant();
}

void JobProgressPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> jobID >> result >> extraData;
}

QByteArray JobProgressPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << jobID << result << extraData;
    return ba;
}
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
AdminLoginPacket::AdminLoginPacket()
    : MSPacket(quint8(MS::CMD_AdminLogin))
{
    init();
}

AdminLoginPacket::AdminLoginPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_AdminLogin))
{
    init();
    fromPacket(base);
}

AdminLoginPacket::~AdminLoginPacket()
{

}

void AdminLoginPacket::init()
{
    InfoType = LOGIN_REQUEST;

    LoginInfo.adminID = "";
    LoginInfo.password = "";
    LoginInfo.computerName = "";

    LoginResult.loggedIn = 0;
    LoginResult.message = "";
    LoginResult.readonly = 1;
}

void AdminLoginPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(LOGIN_UNKNOWN);
    in >> type;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case LOGIN_REQUEST: {
        in >> LoginInfo.adminID >> LoginInfo.password >> LoginInfo.computerName;
    }
        break;

    case LOGIN_RESULT: {
        in >> LoginResult.loggedIn >> LoginResult.message >> LoginResult.readonly;
    }
        break;

    default:
        break;
    }

}

QByteArray AdminLoginPacket::packBodyData()
{
    if(InfoType == LOGIN_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType);

    switch (InfoType) {
    case LOGIN_REQUEST: {
        out << LoginInfo.adminID << LoginInfo.password << LoginInfo.computerName;
    }
        break;

    case LOGIN_RESULT: {
        out << LoginResult.loggedIn << LoginResult.message << LoginResult.readonly;
    }
        break;

    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ClientInfoPacket::ClientInfoPacket()
    : MSPacket(quint8(MS::CMD_ClientInfo))
{
    init();
}

ClientInfoPacket::ClientInfoPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_ClientInfo))
{
    init();
    fromPacket(base);
}

ClientInfoPacket::~ClientInfoPacket()
{
    data.clear();
}

void ClientInfoPacket::init()
{
    IsRequest = 0;
    assetNO = "";
    infoType = quint8(MS::SYSINFO_UNKNOWN);
    data = QByteArray();
}

void ClientInfoPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> IsRequest >> assetNO >> infoType >> data;
}

QByteArray ClientInfoPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << IsRequest << assetNO << infoType << data;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
SystemInfoFromServerPacket::SystemInfoFromServerPacket()
    : MSPacket(quint8(MS::CMD_SystemInfoFromServer))
{
    init();
}

SystemInfoFromServerPacket::SystemInfoFromServerPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_SystemInfoFromServer))
{
    init();
    fromPacket(base);
}

SystemInfoFromServerPacket::~SystemInfoFromServerPacket()
{
    data.clear();
}

void SystemInfoFromServerPacket::init()
{
    infoType = quint8(MS::SYSINFO_UNKNOWN);
    data = QByteArray();
    extraInfo = "";
}

void SystemInfoFromServerPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> infoType >> data >> extraInfo;
}

QByteArray SystemInfoFromServerPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << infoType << data << extraInfo;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
SysAdminInfoPacket::SysAdminInfoPacket()
    : MSPacket(quint8(MS::CMD_SysAdminInfo))
{
    init();
}

SysAdminInfoPacket::SysAdminInfoPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_SysAdminInfo))
{
    init();
    fromPacket(base);
}

SysAdminInfoPacket::~SysAdminInfoPacket()
{
    data.clear();
}

void SysAdminInfoPacket::init()
{
    adminID = "";
    data = QByteArray();
    deleteAdmin = 0;
}

void SysAdminInfoPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> adminID >> data >> deleteAdmin;

}

QByteArray SysAdminInfoPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << adminID << data << deleteAdmin;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
SystemAlarmsPacket::SystemAlarmsPacket()
    : MSPacket(quint8(MS::CMD_SystemAlarms))
{
    init();
}

SystemAlarmsPacket::SystemAlarmsPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_SystemAlarms))
{
    init();
    fromPacket(base);
}

SystemAlarmsPacket::~SystemAlarmsPacket()
{

}

void SystemAlarmsPacket::init()
{
    InfoType = SYSTEMALARMS_UNKNOWN;

    QueryInfo.assetNO = "";
    QueryInfo.type = "-1";
    QueryInfo.acknowledged = "-1";
    QueryInfo.startTime = "";
    QueryInfo.endTime = "";

    ACKInfo.alarms = "";
    ACKInfo.deleteAlarms = 0;
}

void SystemAlarmsPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(SYSTEMALARMS_UNKNOWN);
    in >> type;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case SYSTEMALARMS_QUERY: {
        in >> QueryInfo.assetNO >> QueryInfo.type >> QueryInfo.acknowledged >> QueryInfo.startTime >> QueryInfo.endTime;
    }
        break;

    case SYSTEMALARMS_ACK: {
        in >> ACKInfo.alarms >> ACKInfo.deleteAlarms;
    }
        break;

    default:
        break;
    }

}

QByteArray SystemAlarmsPacket::packBodyData()
{
    if(InfoType == SYSTEMALARMS_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType);

    switch (InfoType) {
    case SYSTEMALARMS_QUERY: {
        out << QueryInfo.assetNO << QueryInfo.type << QueryInfo.acknowledged << QueryInfo.startTime << QueryInfo.endTime;
    }
        break;

    case SYSTEMALARMS_ACK: {
        out << ACKInfo.alarms << ACKInfo.deleteAlarms;
    }
        break;

    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
AnnouncementPacket::AnnouncementPacket()
    : MSPacket(quint8(MS::CMD_Announcement))
{
    init();
}

AnnouncementPacket::AnnouncementPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_Announcement))
{
    init();
    fromPacket(base);
}

AnnouncementPacket::~AnnouncementPacket()
{

}

void AnnouncementPacket::init()
{
    InfoType = ANNOUNCEMENT_UNKNOWN;
    JobID = 0;

    QueryInfo.announcementID = "";
    QueryInfo.keyword = "";
    QueryInfo.validity = "-1";
    QueryInfo.assetNO = "";
    QueryInfo.userName = "";
    QueryInfo.target = "-1";
    QueryInfo.startTime = "";
    QueryInfo.endTime = "";

    CreateInfo.localTempID = 0;
    CreateInfo.adminID = "";
    CreateInfo.type = quint8(MS::ANNOUNCEMENT_NORMAL);
    CreateInfo.content = "";
    CreateInfo.confirmationRequired = 1;
    CreateInfo.validityPeriod = 60;
    CreateInfo.targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
    CreateInfo.targets = "";

    UpdateInfo.adminName = "";
    UpdateInfo.announcementID = 0;
    UpdateInfo.targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
    UpdateInfo.active = 1;
    UpdateInfo.addedTargets = "";
    UpdateInfo.deletedTargets = "";

    ReplyInfo.announcementID = 0;
    ReplyInfo.sender = "";
    ReplyInfo.receiver = "";
    ReplyInfo.receiversAssetNO = "";
    ReplyInfo.replyMessage = "";

    QueryTargetsInfo.announcementID = 0;

}

void AnnouncementPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);


    quint8 type = quint8(ANNOUNCEMENT_UNKNOWN);
    in >> type >> JobID;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case ANNOUNCEMENT_QUERY: {
        in >> QueryInfo.announcementID >> QueryInfo.keyword >> QueryInfo.validity >> QueryInfo.assetNO >> QueryInfo.userName >> QueryInfo.target >> QueryInfo.startTime >> QueryInfo.endTime;
    }
        break;

    case ANNOUNCEMENT_CREATE: {
        in >> CreateInfo.localTempID >> CreateInfo.adminID >> CreateInfo.type >> CreateInfo.content >> CreateInfo.confirmationRequired >> CreateInfo.validityPeriod >> CreateInfo.targetType >> CreateInfo.targets;
    }
        break;

    case ANNOUNCEMENT_UPDATE: {
        in << UpdateInfo.adminName << UpdateInfo.announcementID << UpdateInfo.targetType << UpdateInfo.active << UpdateInfo.addedTargets << UpdateInfo.deletedTargets;
    }
        break;

    case ANNOUNCEMENT_REPLY: {
        in >> ReplyInfo.announcementID >> ReplyInfo.sender >> ReplyInfo.receiver >> ReplyInfo.receiversAssetNO >> ReplyInfo.replyMessage ;
    }
        break;

    case ANNOUNCEMENT_QUERY_TARGETS: {
        in >> QueryTargetsInfo.announcementID;
    }
        break;

    default:
        break;
    }

}

QByteArray AnnouncementPacket::packBodyData()
{
    if(InfoType == ANNOUNCEMENT_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType) << JobID;

    switch (InfoType) {
    case ANNOUNCEMENT_QUERY: {
        out << QueryInfo.announcementID << QueryInfo.keyword << QueryInfo.validity << QueryInfo.assetNO << QueryInfo.userName << QueryInfo.target << QueryInfo.startTime << QueryInfo.endTime;
    }
        break;

    case ANNOUNCEMENT_CREATE: {
        out << CreateInfo.localTempID << CreateInfo.adminID << CreateInfo.type << CreateInfo.content << CreateInfo.confirmationRequired << CreateInfo.validityPeriod << CreateInfo.targetType << CreateInfo.targets;
    }
        break;

    case ANNOUNCEMENT_UPDATE: {
        out << UpdateInfo.adminName << UpdateInfo.announcementID << UpdateInfo.targetType << UpdateInfo.active << UpdateInfo.addedTargets << UpdateInfo.deletedTargets;
    }
        break;

    case ANNOUNCEMENT_REPLY: {
        out << ReplyInfo.announcementID << ReplyInfo.sender << ReplyInfo.receiver << ReplyInfo.receiversAssetNO << ReplyInfo.replyMessage ;
    }
        break;

    case ANNOUNCEMENT_QUERY_TARGETS: {
        out << QueryTargetsInfo.announcementID;
    }
        break;

    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
RemoteConsolePacket::RemoteConsolePacket()
    : MSPacket(quint8(MS::CMD_RemoteConsole))
{
    init();
}

RemoteConsolePacket::RemoteConsolePacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_RemoteConsole))
{
    init();
    fromPacket(base);
}

void RemoteConsolePacket::init()
{
    InfoType = REMOTECONSOLE_UNKNOWN;

    OpenConsole.applicationPath = "";
    OpenConsole.startProcess = 1;

    ConsoleState.isRunning = 0;
    ConsoleState.message = "";
    ConsoleState.messageType = quint8(MS::MSG_Information);

    Command.command = "";

    Output.output = "";
}

void RemoteConsolePacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(REMOTECONSOLE_UNKNOWN);
    in >> type;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case REMOTECONSOLE_OPEN: {
        in >> OpenConsole.applicationPath >> OpenConsole.startProcess;
    }
        break;

    case REMOTECONSOLE_STATE: {
        in >> ConsoleState.isRunning >> ConsoleState.message >> ConsoleState.messageType;
    }
        break;

    case REMOTECONSOLE_COMMAND: {
        in >> Command.command;
    }
        break;

    case REMOTECONSOLE_OUTPUT: {
        in >> Output.output;
    }
        break;

    default:
        break;
    }

}

QByteArray RemoteConsolePacket::packBodyData()
{
    if(InfoType == REMOTECONSOLE_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType);

    switch (InfoType) {
    case REMOTECONSOLE_OPEN: {
        out << OpenConsole.applicationPath << OpenConsole.startProcess;
    }
        break;

    case REMOTECONSOLE_STATE: {
        out << ConsoleState.isRunning << ConsoleState.message << ConsoleState.messageType;
    }
        break;

    case REMOTECONSOLE_COMMAND: {
        out << Command.command ;
    }
        break;

    case REMOTECONSOLE_OUTPUT: {
        out << Output.output;
    }
        break;

    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ClientLogPacket::ClientLogPacket()
    : MSPacket(quint8(MS::CMD_ClientLog))
{
    init();
}

ClientLogPacket::ClientLogPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_ClientLog))
{
    init();
    fromPacket(base);
}

void ClientLogPacket::init()
{
    log = "";
    logType = quint8(MS::LOG_Unknown) ;
}

void ClientLogPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> logType >> log;
}

QByteArray ClientLogPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << logType << log;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
USBDevPacket::USBDevPacket()
    : MSPacket(quint8(MS::CMD_FileTransfer))
{
    init();
}

USBDevPacket::USBDevPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_FileTransfer))
{
    init();
    fromPacket(base);
}

void USBDevPacket::init()
{
    usbSTORStatus = quint8(MS::USBSTOR_Unknown);
}

void USBDevPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> usbSTORStatus;
}

QByteArray USBDevPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << usbSTORStatus;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
AdminConnectionToClientPacket::AdminConnectionToClientPacket()
    : MSPacket(quint8(MS::CMD_AdminConnectionToClient))
{
    init();
}

AdminConnectionToClientPacket::AdminConnectionToClientPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_AdminConnectionToClient))
{
    init();
    fromPacket(base);
}

void AdminConnectionToClientPacket::init()
{
    computerName = "";
    adminID = "";
    verified = 0;
    errorCode = quint8(MS::ERROR_NO_ERROR);
}

void AdminConnectionToClientPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> adminID >> computerName >> verified >> errorCode;
}

QByteArray AdminConnectionToClientPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << adminID << computerName << verified << errorCode;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
AdminSearchClientPacket::AdminSearchClientPacket()
    : MSPacket(quint8(MS::CMD_AdminSearchClient))
{
    init();
}

AdminSearchClientPacket::AdminSearchClientPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_AdminSearchClient))
{
    init();
    fromPacket(base);
}

void AdminSearchClientPacket::init()
{
    computerName = "";
    userName = "";
    workgroup = "";
    macAddress = "";
    ipAddress = "";
    osVersion = "";
    adminID = "";
}

void AdminSearchClientPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> computerName >> userName >> workgroup >> macAddress >> ipAddress >> osVersion >> adminID;
}

QByteArray AdminSearchClientPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminID;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
LocalUserOnlineStatusChangedPacket::LocalUserOnlineStatusChangedPacket()
    : MSPacket(quint8(MS::CMD_LocalUserOnlineStatusChanged))
{
    init();
}

LocalUserOnlineStatusChangedPacket::LocalUserOnlineStatusChangedPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_LocalUserOnlineStatusChanged))
{
    init();
    fromPacket(base);
}

void LocalUserOnlineStatusChangedPacket::init()
{
    userName = "";
    online = 0;
}

void LocalUserOnlineStatusChangedPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> userName >> online;
}

QByteArray LocalUserOnlineStatusChangedPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << userName << online;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
FileTransferPacket::FileTransferPacket()
    : MSPacket(quint8(MS::CMD_FileTransfer))
{
    init();
}

FileTransferPacket::FileTransferPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_FileTransfer))
{
    init();
    fromPacket(base);
}

void FileTransferPacket::init()
{
    InfoType = FT_UNKNOWN;

    FileSystemInfoRequest.parentDirPath = "";

    FileSystemInfoResponse.baseDirPath = "";
    FileSystemInfoResponse.fileSystemInfoData = QByteArray();

    FileDeletingRequest.baseDirPath = "";
    FileDeletingRequest.files.clear();

    FileDeletingResponse.baseDirPath = "";
    FileDeletingResponse.failedFiles.clear();

    FileRenamingRequest.baseDirPath = "";
    FileRenamingRequest.oldFileName = "";
    FileRenamingRequest.newFileName = "";

    FileRenamingResponse.baseDirPath = "";
    FileRenamingResponse.oldFileName = "";
    FileRenamingResponse.newFileName = "";
    FileRenamingResponse.renamed = 0;
    FileRenamingResponse.message = "";


    FileDownloadingRequest.baseDir = "";
    FileDownloadingRequest.fileName = "";
    FileDownloadingRequest.dirToSaveFile = "./";

    FileDownloadingResponse.baseDir = "";
    FileDownloadingResponse.fileName = "";
    FileDownloadingResponse.accepted = 1;
    FileDownloadingResponse.fileMD5Sum = QByteArray();
    FileDownloadingResponse.size = 0;
    FileDownloadingResponse.pathToSaveFile = "";
    FileDownloadingResponse.errorCode = MS::ERROR_NO_ERROR;

    FileUploadingRequest.fileName = "";
    FileUploadingRequest.fileMD5Sum = QByteArray();
    FileUploadingRequest.size = 0;
    FileUploadingRequest.fileSaveDir = "./";

    FileUploadingResponse.fileMD5Sum = QByteArray();
    FileUploadingResponse.accepted = 1;
    FileUploadingResponse.message = "";

    FileDataRequest.fileMD5 = QByteArray();
    FileDataRequest.startPieceIndex = 0;
    FileDataRequest.endPieceIndex = 0;

    FileDataResponse.fileMD5 = QByteArray();
    FileDataResponse.pieceIndex = 0;
    FileDataResponse.data = QByteArray();
    FileDataResponse.pieceMD5 = QByteArray();

    FileTXStatus.fileMD5 = QByteArray();
    FileTXStatus.status = quint8(MS::File_TX_Preparing);

    FileTXError.fileMD5 = QByteArray();
    FileTXError.errorCode = 0;
    FileTXError.message = "";


}

void FileTransferPacket::parsePacketBody(QByteArray &packetBody)
{

    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(FT_UNKNOWN);
    in >> type;

    InfoType = PacketInfoType(type);
    switch (InfoType) {
    case FT_FileSystemInfoRequest: {
        in >> FileSystemInfoRequest.parentDirPath;
    }
        break;

    case FT_FileSystemInfoResponse: {
        in >> FileSystemInfoResponse.baseDirPath >> FileSystemInfoResponse.fileSystemInfoData;
    }
        break;

    case FT_FileDeletingRequest: {
        in >> FileDeletingRequest.baseDirPath >> FileDeletingRequest.files;
    }
        break;

    case FT_FileDeletingResponse: {
        in >> FileDeletingResponse.baseDirPath >> FileDeletingResponse.failedFiles;
    }
        break;

    case FT_FileRenamingRequest: {
        in >> FileRenamingRequest.baseDirPath >> FileRenamingRequest.oldFileName >> FileRenamingRequest.newFileName;
    }
        break;

    case FT_FileRenamingResponse: {
        in >> FileRenamingResponse.baseDirPath >> FileRenamingResponse.oldFileName >> FileRenamingResponse.newFileName >> FileRenamingResponse.renamed >> FileRenamingResponse.message;
    }
        break;

    case FT_FileDownloadingRequest: {
        in >> FileDownloadingRequest.baseDir >> FileDownloadingRequest.fileName >> FileDownloadingRequest.dirToSaveFile;
    }
        break;

    case FT_FileDownloadingResponse: {
        in >> FileDownloadingResponse.accepted >> FileDownloadingResponse.baseDir >> FileDownloadingResponse.fileName >> FileDownloadingResponse.fileMD5Sum >> FileDownloadingResponse.size >> FileDownloadingResponse.pathToSaveFile >> FileDownloadingResponse.errorCode;
    }
        break;

    case FT_FileUploadingRequest: {
        in >> FileUploadingRequest.fileName >> FileUploadingRequest.fileMD5Sum >> FileUploadingRequest.size >> FileUploadingRequest.fileSaveDir;
    }
        break;

    case FT_FileUploadingResponse: {
        in >> FileUploadingResponse.accepted >> FileUploadingResponse.fileMD5Sum >> FileUploadingResponse.message;
    }
        break;

    case FT_FileDataRequest: {
        in >> FileDataRequest.fileMD5 >> FileDataRequest.startPieceIndex >> FileDataRequest.endPieceIndex;
    }
        break;

    case FT_FileData: {
        in >> FileDataResponse.fileMD5 >> FileDataResponse.pieceIndex >> FileDataResponse.data >> FileDataResponse.pieceMD5;
    }
        break;

    case FT_FileTXStatus: {
        in >> FileTXStatus.fileMD5 >> FileTXStatus.status;
    }
        break;

    case FT_FileTXError: {
        in >> FileTXError.fileMD5 >> FileTXError.errorCode >> FileTXError.message;
    }
        break;

    default:
        break;
    }

}

QByteArray FileTransferPacket::packBodyData()
{
    if(InfoType == FT_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << quint8(InfoType);

    switch (InfoType) {
    case FT_FileSystemInfoRequest: {
        out << FileSystemInfoRequest.parentDirPath;
    }
        break;

    case FT_FileSystemInfoResponse: {
        out << FileSystemInfoResponse.baseDirPath << FileSystemInfoResponse.fileSystemInfoData;
    }
        break;

    case FT_FileDeletingRequest: {
        out << FileDeletingRequest.baseDirPath << FileDeletingRequest.files;
    }
        break;

    case FT_FileDeletingResponse: {
        out << FileDeletingResponse.baseDirPath << FileDeletingResponse.failedFiles;
    }
        break;

    case FT_FileRenamingRequest: {
        out << FileRenamingRequest.baseDirPath << FileRenamingRequest.oldFileName << FileRenamingRequest.newFileName;
    }
        break;

    case FT_FileRenamingResponse: {
        out << FileRenamingResponse.baseDirPath << FileRenamingResponse.oldFileName << FileRenamingResponse.newFileName << FileRenamingResponse.renamed << FileRenamingResponse.message;
    }
        break;

    case FT_FileDownloadingRequest: {
        out << FileDownloadingRequest.baseDir << FileDownloadingRequest.fileName << FileDownloadingRequest.dirToSaveFile;
    }
        break;

    case FT_FileDownloadingResponse: {
        out << FileDownloadingResponse.accepted << FileDownloadingResponse.baseDir << FileDownloadingResponse.fileName << FileDownloadingResponse.fileMD5Sum << FileDownloadingResponse.size << FileDownloadingResponse.pathToSaveFile << FileDownloadingResponse.errorCode;
    }
        break;

    case FT_FileUploadingRequest: {
        out << FileUploadingRequest.fileName << FileUploadingRequest.fileMD5Sum << FileUploadingRequest.size << FileUploadingRequest.fileSaveDir;
    }
        break;

    case FT_FileUploadingResponse: {
        out << FileUploadingResponse.accepted << FileUploadingResponse.fileMD5Sum << FileUploadingResponse.message;
    }
        break;

    case FT_FileDataRequest: {
        out << FileDataRequest.fileMD5 << FileDataRequest.startPieceIndex << FileDataRequest.endPieceIndex;
    }
        break;

    case FT_FileData: {
        out << FileDataResponse.fileMD5 << FileDataResponse.pieceIndex << FileDataResponse.data << FileDataResponse.pieceMD5;
    }
        break;

    case FT_FileTXStatus: {
        out << FileTXStatus.fileMD5 << FileTXStatus.status;
    }
        break;

    case FT_FileTXError: {
        out << FileTXError.fileMD5 << FileTXError.errorCode << FileTXError.message;
    }
        break;

    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ModifyAssetNOPacket::ModifyAssetNOPacket()
    : MSPacket(quint8(MS::CMD_ModifyAssetNO))
{
    init();
}

ModifyAssetNOPacket::ModifyAssetNOPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_ModifyAssetNO))
{
    init();
    fromPacket(base);
}

void ModifyAssetNOPacket::init()
{
    isRequest = 1;
    oldAssetNO = "";
    newAssetNO = "";
    adminID = "";
}

void ModifyAssetNOPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> isRequest >> oldAssetNO >> newAssetNO >> adminID;
}

QByteArray ModifyAssetNOPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << isRequest << oldAssetNO << newAssetNO << adminID;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
RenameComputerPacket::RenameComputerPacket()
    : MSPacket(quint8(MS::CMD_RenameComputer))
{
    init();
}

RenameComputerPacket::RenameComputerPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_RenameComputer))
{
    init();
    fromPacket(base);
}

void RenameComputerPacket::init()
{
    assetNO = "";
    newComputerName = "";
    domainAdminName = "";
    domainAdminPassword = "";
}

void RenameComputerPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> assetNO >> newComputerName >> domainAdminName >> domainAdminPassword;
}

QByteArray RenameComputerPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << assetNO << newComputerName << domainAdminName << domainAdminPassword;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
JoinOrUnjoinDomainPacket::JoinOrUnjoinDomainPacket()
    : MSPacket(quint8(MS::CMD_JoinOrUnjoinDomain))
{
    init();
}

JoinOrUnjoinDomainPacket::JoinOrUnjoinDomainPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_JoinOrUnjoinDomain))
{
    init();
    fromPacket(base);
}

void JoinOrUnjoinDomainPacket::init()
{
    assetNO = "";
    domainOrWorkgroupName = "";
    domainAdminName = "";
    domainAdminPassword = "";
    joinWorkgroup = 1;
}

void JoinOrUnjoinDomainPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> assetNO >> domainOrWorkgroupName >> domainAdminName >> domainAdminPassword >> joinWorkgroup;
}

QByteArray JoinOrUnjoinDomainPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << assetNO << domainOrWorkgroupName << domainAdminName << domainAdminPassword << joinWorkgroup;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
TemperaturesPacket::TemperaturesPacket()
    : MSPacket(quint8(MS::CMD_Temperatures))
{
    init();
}

TemperaturesPacket::TemperaturesPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_Temperatures))
{
    init();
    fromPacket(base);
}

void TemperaturesPacket::init()
{
    InfoType = TEMPERATURES_UNKNOWN;

    TemperaturesRequest.requestCPU = true;
    TemperaturesRequest.requestHD = false;

    TemperaturesResponse.cpuTemperature = "";
    TemperaturesResponse.harddiskTemperature = "";
}

void TemperaturesPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(TEMPERATURES_UNKNOWN);
    in >> type;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case TEMPERATURES_REQUEST: {
        in >> TemperaturesRequest.requestCPU >> TemperaturesRequest.requestCPU;
    }
        break;

    case TEMPERATURES_RESPONSE: {
        in >> TemperaturesResponse.cpuTemperature >> TemperaturesResponse.harddiskTemperature;
    }
        break;


    default:
        break;
    }

}

QByteArray TemperaturesPacket::packBodyData()
{
    if(InfoType == TEMPERATURES_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType);

    switch (InfoType) {
    case TEMPERATURES_REQUEST: {
        out << TemperaturesRequest.requestCPU << TemperaturesRequest.requestCPU;
    }
        break;

    case TEMPERATURES_RESPONSE: {
        out << TemperaturesResponse.cpuTemperature << TemperaturesResponse.harddiskTemperature;
    }
        break;


    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ScreenshotPacket::ScreenshotPacket()
    : MSPacket(quint8(MS::CMD_Screenshot))
{
    init();
}

ScreenshotPacket::ScreenshotPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_Screenshot))
{
    init();
    fromPacket(base);
}

void ScreenshotPacket::init()
{
    InfoType = SCREENSHOT_UNKNOWN;

    ScreenshotRequest.adminID = "";
    ScreenshotRequest.userName = "";
    ScreenshotRequest.adminAddress = "";
    ScreenshotRequest.adminListeningPort = 0;

    DesktopInfo.desktopWidth = 0;
    DesktopInfo.desktopHeight = 0;
    DesktopInfo.blockWidth = 0;
    DesktopInfo.blockHeight = 0;
}

void ScreenshotPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    quint8 type = quint8(SCREENSHOT_UNKNOWN);
    in >> type ;
    InfoType = PacketInfoType(type);

    switch (InfoType) {
    case SCREENSHOT_REQUEST: {
        in >> ScreenshotRequest.adminID >> ScreenshotRequest.userName >> ScreenshotRequest.adminAddress >> ScreenshotRequest.adminListeningPort;
    }
        break;

    case SCREENSHOT_DESKTOP_INFO: {
        in >> DesktopInfo.desktopWidth >> DesktopInfo.desktopHeight >> DesktopInfo.blockWidth >> DesktopInfo.blockHeight;
    }
        break;

    case SCREENSHOT_DATA: {
        while (!in.atEnd()) {
            int x = 0, y = 0;
            QByteArray image;

            in >> x >> y >> image;

            ScreenshotData.locations.append(QPoint(x, y));
            ScreenshotData.images.append(image);
        }
    }
        break;


    default:
        break;
    }

}

QByteArray ScreenshotPacket::packBodyData()
{
    if(InfoType == SCREENSHOT_UNKNOWN) {
        return QByteArray();
    }

    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << quint8(InfoType);

    switch (InfoType) {
    case SCREENSHOT_REQUEST: {
        out << ScreenshotRequest.adminID << ScreenshotRequest.userName << ScreenshotRequest.adminAddress << ScreenshotRequest.adminListeningPort;
    }
        break;

    case SCREENSHOT_DESKTOP_INFO: {
        out << DesktopInfo.desktopWidth << DesktopInfo.desktopHeight << DesktopInfo.blockWidth << DesktopInfo.blockHeight;
    }
        break;

    case SCREENSHOT_DATA: {
        for(int i = 0; i < ScreenshotData.locations.size(); i++) {
            QPoint point = ScreenshotData.locations.at(i);
            out <<  point.x() << point.y() << ScreenshotData.images.at(i);
        }
    }
        break;


    default:
        break;
    }

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ShutdownPacket::ShutdownPacket()
    : MSPacket(quint8(MS::CMD_Shutdown))
{
    init();
}

ShutdownPacket::ShutdownPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_Shutdown))
{
    init();
    fromPacket(base);
}

void ShutdownPacket::init()
{
    message = "";
    waitTime = 0;
    force = 1;
    reboot = 0 ;
}

void ShutdownPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> message >> waitTime >> force >> reboot ;
}

QByteArray ShutdownPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << message << waitTime << force << reboot ;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
LockWindowsPacket::LockWindowsPacket()
    : MSPacket(quint8(MS::CMD_LockWindows))
{
    init();
}

LockWindowsPacket::LockWindowsPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_LockWindows))
{
    init();
    fromPacket(base);
}

void LockWindowsPacket::init()
{
    userName = "";
    logoff = 0;
}

void LockWindowsPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> userName >> logoff ;
}

QByteArray LockWindowsPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << userName << logoff ;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
WinUserPacket::WinUserPacket()
    : MSPacket(quint8(MS::CMD_WinUser))
{
    init();
}

WinUserPacket::WinUserPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_WinUser))
{
    init();
    fromPacket(base);
}

void WinUserPacket::init()
{
    userData = 0;
}

void WinUserPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> userData ;
}

QByteArray WinUserPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << userData ;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ServiceConfigPacket::ServiceConfigPacket()
    : MSPacket(quint8(MS::CMD_ServiceConfig))
{
    init();
}

ServiceConfigPacket::ServiceConfigPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_ServiceConfig))
{
    init();
    fromPacket(base);
}

void ServiceConfigPacket::init()
{
    serviceName = "";
    startupType = 0xFFFFFFFF;
    startService = 0;
    processID = 0;
}

void ServiceConfigPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> serviceName >> startupType >> startService >> processID;
}

QByteArray ServiceConfigPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << serviceName << startupType << startService << processID;

    return ba;
}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
ProcessMonitorInfoPacket::ProcessMonitorInfoPacket()
    : MSPacket(quint8(MS::CMD_ProcessMonitorInfo))
{
    init();
}

ProcessMonitorInfoPacket::ProcessMonitorInfoPacket(const PacketBase &base)
    : MSPacket(quint8(MS::CMD_ProcessMonitorInfo))
{
    init();
    fromPacket(base);
}

void ProcessMonitorInfoPacket::init()
{
    localRules = QByteArray();
    globalRules = QByteArray();
    enableProcMon = 0;
    enablePassthrough = 1;
    enableLogAllowedProcess = 0;
    enableLogBlockedProcess = 1;
    useGlobalRules = 1;
    assetNO = "";
}

void ProcessMonitorInfoPacket::parsePacketBody(QByteArray &packetBody)
{
    QDataStream in(&packetBody, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    in >> localRules >> globalRules >> enableProcMon >> enablePassthrough >> enableLogAllowedProcess >> enableLogBlockedProcess >> useGlobalRules >> assetNO;
}

QByteArray ProcessMonitorInfoPacket::packBodyData()
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << localRules << globalRules << enableProcMon << enablePassthrough << enableLogAllowedProcess << enableLogBlockedProcess << useGlobalRules << assetNO;

    return ba;
}
////////////////////////////////////////////////////////////////////////




} //namespace HEHUI
