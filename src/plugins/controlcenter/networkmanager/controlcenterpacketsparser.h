/*
 ****************************************************************************
 * controlcenterpacketsparser.h
 *
 * Created On: 2010-7-13
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2015-4-10
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef CONTROLCENTERPACKETSPARSER_H_
#define CONTROLCENTERPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/packets.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"



namespace HEHUI {


class ControlCenterPacketsParser : public QObject{
    Q_OBJECT
public:
    ControlCenterPacketsParser(ResourcesManagerInstance *manager, QObject *parent = 0);
    virtual ~ControlCenterPacketsParser();

public slots:

    void parseIncomingPacketData(const PacketBase &packet);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = quint16(IP_MULTICAST_GROUP_PORT)){
        qDebug()<<"----sendClientLookForServerPacket(...) "<<" targetAddress:"<<targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }

        ServerDiscoveryPacket packet;
        packet.responseFromServer = 0;
        packet.version = QString(APP_VERSION);
        packet.udpPort = m_udpServer->localPort();

        return m_udpServer->sendDatagram(packet.toByteArray(), address, targetPort);
    }

    bool sendAdminLoginPacket(SOCKETID socketID, const QString &adminName, const QString &password){
        qDebug()<<"----sendAdminLoginPacket(...)";

        AdminLoginPacket packet;
        packet.InfoType = AdminLoginPacket::LOGIN_REQUEST;
        packet.LoginInfo.adminID = adminName;
        packet.LoginInfo.password = password;
        packet.LoginInfo.computerName = m_localComputerName;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestSysAlarmsPacket(SOCKETID serverSocketID, const QString &assetNO, const QString &type, const QString &acknowledged, const QString &startTime, const QString &endTime){
        qDebug()<<"----sendRequestSysAlarmsPacket(...)";

        SystemAlarmsPacket packet;
        packet.InfoType = SystemAlarmsPacket::SYSTEMALARMS_QUERY;
        packet.QueryInfo.assetNO = assetNO;
        packet.QueryInfo.type = type;
        packet.QueryInfo.acknowledged = acknowledged;
        packet.QueryInfo.startTime = startTime;
        packet.QueryInfo.endTime = endTime;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendAcknowledgeSysAlarmsPacket(SOCKETID serverSocketID, const QString &adminID, const QStringList &alarms, bool deleteAlarms = false){
        qDebug()<<"----sendAcknowledgeSysAlarmsPacket(...)";

        SystemAlarmsPacket packet;
        packet.InfoType = SystemAlarmsPacket::SYSTEMALARMS_ACK;
        packet.ACKInfo.alarms = alarms.join(",");
        packet.ACKInfo.deleteAlarms = deleteAlarms;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &id, const QString &keyword, const QString &validity, const QString &assetNO, const QString &userName, const QString &target, const QString &startTime, const QString &endTime){
        qDebug()<<"----sendRequestAnnouncementsPacket(...)";

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_QUERY;
        packet.QueryInfo.announcementID = id;
        packet.QueryInfo.keyword = keyword;
        packet.QueryInfo.validity = validity;
        packet.QueryInfo.assetNO = assetNO;
        packet.QueryInfo.userName = userName;
        packet.QueryInfo.target = target;
        packet.QueryInfo.startTime = startTime;
        packet.QueryInfo.endTime = endTime;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendRequestAnnouncementTargetsPacket(SOCKETID serverSocketID, const QString &announcementID){
        qDebug()<<"----sendRequestAnnouncementTargetsPacket(...)";

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_QUERY_TARGETS;
        packet.QueryTargetsInfo.announcementID = announcementID.toUInt();

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendRequestClientInfoPacket(SOCKETID socketID, const QString &assetNO, quint8 infoType){

        ClientInfoPacket packet;
        packet.IsRequest = 1;
        packet.assetNO = assetNO;
        packet.infoType = infoType;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestUpdateSysAdminInfoPacket(SOCKETID serverSocketID, const QString &sysAdminID, const QByteArray &infoData, bool deleteAdmin = false){

        SysAdminInfoPacket packet;
        packet.adminID = sysAdminID;
        packet.data = infoData;
        packet.deleteAdmin = deleteAdmin;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendAdminRequestRemoteConsolePacket(SOCKETID socketID, const QString &assetNO, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_OPEN;
        packet.OpenConsole.applicationPath = applicationPath;
        packet.OpenConsole.startProcess = quint8(startProcess);

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRemoteConsoleCMDFromAdminPacket(SOCKETID socketID, const QString &assetNO, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_OPEN;
        packet.Command.command = command;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendSetupUSBSDPacket(SOCKETID socketID, quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminID){

        USBDevPacket packet;
        packet.usbSTORStatus = usbSTORStatus;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendModifyAssetNOPacket(SOCKETID socketID, const QString &newAssetNO, const QString &oldAssetNO, const QString &adminID){

        ModifyAssetNOPacket packet;
        packet.isRequest = 1;
        packet.oldAssetNO = oldAssetNO;
        packet.newAssetNO = newAssetNO;
        packet.adminID = adminID;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRenameComputerPacket(SOCKETID socketID, const QString &assetNO, const QString &newComputerName, const QString &adminID, const QString &domainAdminName, const QString &domainAdminPassword){

        RenameComputerPacket packet;
        packet.assetNO = assetNO;
        packet.newComputerName = newComputerName;
        packet.domainAdminName = domainAdminName;
        packet.domainAdminPassword = domainAdminPassword;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendJoinOrUnjoinDomainPacket(SOCKETID socketID, const QString &assetNO, const QString &adminID, bool joinWorkgroup, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword){

        JoinOrUnjoinDomainPacket packet;
        packet.assetNO = assetNO;
        packet.domainOrWorkgroupName = domainOrWorkgroupName;
        packet.domainAdminName = domainAdminName;
        packet.domainAdminPassword = domainAdminPassword;
        packet.joinWorkgroup = joinWorkgroup;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAdminRequestConnectionToClientPacket(SOCKETID socketID, const QString &adminComputerName, const QString &adminID){

        AdminConnectionToClientPacket packet;
        packet.computerName = adminComputerName;
        packet.adminID = adminID;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }
    
    bool sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminID){

        AdminSearchClientPacket packet;
        packet.computerName = computerName;
        packet.userName = userName;
        packet.workgroup = workgroup;
        packet.macAddress = macAddress;
        packet.ipAddress = ipAddress;
        packet.osVersion = osVersion;
        packet.adminID = adminID;

        return m_udpServer->sendDatagram(packet.toByteArray(), targetAddress, quint16(IP_MULTICAST_GROUP_PORT));
    }

    bool sendCreateAnnouncementPacket(quint32 jobID, const QString &peerAddress, quint16 peerPort, unsigned int localTempID, const QString &adminName, quint8 type, const QString &content, bool confirmationRequired = true, int validityPeriod = 60, quint8 targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE), const QString &targets = ""){
        qDebug()<<"--sendCreateAnnouncementPacket(...) "<<" peerAddress:"<<peerAddress<<" content:"<<content;

        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_CREATE;
        packet.JobID = jobID;
        packet.CreateInfo.localTempID = localTempID;
        packet.CreateInfo.adminID = adminName;
        packet.CreateInfo.type = type;
        packet.CreateInfo.content = content;
        packet.CreateInfo.confirmationRequired = quint8(confirmationRequired);
        packet.CreateInfo.validityPeriod = validityPeriod;
        packet.CreateInfo.targetType = targetType;
        packet.CreateInfo.targets = targets;

        return m_udpServer->sendDatagram(packet.toByteArray(), targetAddress, peerPort);
    }

    bool sendCreateAnnouncementPacket(SOCKETID socketID, quint32 jobID, unsigned int localTempID, const QString &adminName, quint8 type, const QString &content, bool confirmationRequired = true, int validityPeriod = 60, quint8 targetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE), const QString &targets = ""){

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_CREATE;
        packet.JobID = jobID;
        packet.CreateInfo.localTempID = localTempID;
        packet.CreateInfo.adminID = adminName;
        packet.CreateInfo.type = type;
        packet.CreateInfo.content = content;
        packet.CreateInfo.confirmationRequired = quint8(confirmationRequired);
        packet.CreateInfo.validityPeriod = validityPeriod;
        packet.CreateInfo.targetType = targetType;
        packet.CreateInfo.targets = targets;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendUpdateAnnouncementPacket(SOCKETID socketID, quint32 jobID, const QString &adminName, unsigned int announcementID, quint8 targetType, bool active, const QString &addedTargets, const QString &deletedTargets){

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_UPDATE;
        packet.JobID = jobID;
        packet.UpdateInfo.adminName = adminName;
        packet.UpdateInfo.announcementID = announcementID;
        packet.UpdateInfo.targetType = targetType;
        packet.UpdateInfo.active = quint8(active);
        packet.UpdateInfo.addedTargets = addedTargets;
        packet.UpdateInfo.deletedTargets = deletedTargets;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendAdminReplyMessagePacket(SOCKETID socketID, unsigned int announcementID, const QString &sender, const QString &receiver, const QString &receiversAssetNO, const QString &replyMessage){
        qWarning()<<"----sendAdminReplyMessagePacket(...):";

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_REPLY;
        packet.ReplyInfo.announcementID = announcementID;
        packet.ReplyInfo.sender = sender;
        packet.ReplyInfo.receiver = receiver;
        packet.ReplyInfo.receiversAssetNO = receiversAssetNO;
        packet.ReplyInfo.replyMessage = replyMessage;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestTemperaturesPacket(SOCKETID socketID, bool cpu = true, bool harddisk = false){

        TemperaturesPacket packet;
        packet.InfoType = TemperaturesPacket::TEMPERATURES_REQUEST;
        packet.TemperaturesRequest.requestCPU = cpu;
        packet.TemperaturesRequest.requestHD = harddisk;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestScreenshotPacket(SOCKETID socketID, const QString &userName){

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_REQUEST;
        packet.ScreenshotRequest.userName = userName;
        packet.ScreenshotRequest.adminListeningPort = m_localTCPServerListeningPort;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestShutdownPacket(SOCKETID socketID, QString message = "", quint32 waitTime = 0, bool force = true, bool reboot = true){

        ShutdownPacket packet;
        packet.message = message;
        packet.waitTime = waitTime;
        packet.force = force;
        packet.reboot = reboot;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestLockWindowsPacket(SOCKETID socketID, QString userName, bool logoff = true){

        LockWindowsPacket packet;
        packet.userName = userName;
        packet.logoff = logoff;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestCreateOrModifyWinUserPacket(SOCKETID socketID, const QByteArray &userData){

        WinUserPacket packet;
        packet.userData = userData;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestChangeServiceConfigPacket(SOCKETID socketID, const QString &serviceName, bool startService, quint64 startupType){

        ServiceConfigPacket packet;
        packet.serviceName = serviceName;
        packet.startupType = startupType;
        packet.startService = startService;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRules, const QByteArray &globalRules, quint8 enableProcMon, quint8 enablePassthrough, quint8 enableLogAllowedProcess, quint8 enableLogBlockedProcess, quint8 useGlobalRules, const QString &assetNO ){

        ProcessMonitorInfoPacket packet;
        packet.localRules = localRules;
        packet.globalRules = globalRules;
        packet.enableProcMon = enableProcMon;
        packet.enablePassthrough = enablePassthrough;
        packet.enableLogAllowedProcess = enableLogAllowedProcess;
        packet.enableLogBlockedProcess = enableLogBlockedProcess;
        packet.useGlobalRules = useGlobalRules;
        packet.assetNO = assetNO;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

//////////////////////////////

    bool requestFileSystemInfo(SOCKETID socketID, const QString &parentDirPath){
        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileSystemInfoRequest;
        packet.FileSystemInfoRequest.parentDirPath = parentDirPath;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileSystemInfo(SOCKETID socketID, QString parentDirPath, const QByteArray &fileSystemInfoData){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileSystemInfoResponse;
        packet.FileSystemInfoResponse.baseDirPath = parentDirPath;
        packet.FileSystemInfoResponse.fileSystemInfoData = fileSystemInfoData;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDeleteFiles(SOCKETID socketID, const QString &remoteBaseDir, const QStringList &remoteFiles){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDeletingRequest;
        packet.FileDeletingRequest.baseDirPath = remoteBaseDir;
        packet.FileDeletingRequest.files = remoteFiles;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestRenameFile(SOCKETID socketID, const QString &remoteBaseDir, const QString &oldFileName, const QString &newFileName){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileRenamingRequest;
        packet.FileRenamingRequest.baseDirPath = remoteBaseDir;
        packet.FileRenamingRequest.oldFileName = oldFileName;
        packet.FileRenamingRequest.newFileName = newFileName;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = ""){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingRequest;
        packet.FileUploadingRequest.fileName = fileName;
        packet.FileUploadingRequest.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingRequest.size = size;
        packet.FileUploadingRequest.fileSaveDir = remoteFileSaveDir;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDownloadFile(SOCKETID socketID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingRequest;
        packet.FileDownloadingRequest.baseDir = remoteBaseDir;
        packet.FileDownloadingRequest.fileName = remoteFileName;
        packet.FileDownloadingRequest.dirToSaveFile = localFileSaveDir;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileDownloadRequest(SOCKETID socketID, bool accepted, const QString &baseDir, const QString &fileName, const QByteArray &fileMD5Sum, quint64 size, const QString &pathToSaveFile){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingResponse;
        packet.FileDownloadingResponse.accepted = accepted;
        packet.FileDownloadingResponse.baseDir = baseDir;
        packet.FileDownloadingResponse.fileName = fileName;
        packet.FileDownloadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileDownloadingResponse.size = size;
        packet.FileDownloadingResponse.pathToSaveFile = pathToSaveFile;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileUploadRequest(SOCKETID socketID, bool accepted, const QByteArray &fileMD5Sum, const QString &message){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingResponse;
        packet.FileUploadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingResponse.accepted = accepted;
        packet.FileUploadingResponse.message = message;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestFileData(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
        //qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDataRequest;
        packet.FileDataRequest.fileMD5 = fileMD5;
        packet.FileDataRequest.startPieceIndex = startPieceIndex;
        packet.FileDataRequest.endPieceIndex = endPieceIndex;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendFileData(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *pieceMD5){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileData;
        packet.FileDataResponse.fileMD5 = fileMD5;
        packet.FileDataResponse.pieceIndex = pieceIndex;
        packet.FileDataResponse.data = *data;
        packet.FileDataResponse.pieceMD5 = *pieceMD5;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXStatus;
        packet.FileTXStatus.fileMD5 = fileMD5;
        packet.FileTXStatus.status = status;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXError;
        packet.FileTXError.fileMD5 = fileMD5;
        packet.FileTXError.errorCode = errorCode;
        packet.FileTXError.message = errorString;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    QString lastErrorMessage(){return m_rtp->lastErrorString();}



signals:

    void signalServerDeclarePacketReceived(const ServerDiscoveryPacket &packet);
    void signalJobFinished(quint32 jobID, quint8 result, const QVariant &extraData);
    void signalMessagePacketReceived(const MessagePacket &packet);
    void signalClientInfoPacketReceived(const ClientInfoPacket &packet);
    void signalSystemInfoFromServerReceived(const SystemInfoFromServerPacket &packet);

    void signalClientResponseUSBInfoPacketReceived(const USBDevPacket &packet);
    void signalRemoteConsolePacketReceived(const RemoteConsolePacket &packet);
    void signalServerResponseAdminLoginResultPacketReceived(const AdminLoginPacket &packet);

    void signalClientResponseAdminConnectionResultPacketReceived(const AdminConnectionToClientPacket &packet);
    void signalAssetNOModifiedPacketReceived(const ModifyAssetNOPacket &packet);
    void signalUserOnlineStatusChanged(const LocalUserOnlineStatusChangedPacket &packet);
    void signalTemperaturesPacketReceived(const TemperaturesPacket &packet);
    void signalScreenshotPacketReceived(const ScreenshotPacket &packet);

    void signalServiceConfigChangedPacketReceived(const ServiceConfigPacket &packet);


    void signalUserResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);
    void signalClientOnlineStatusChanged(SOCKETID socketID, const QString &assetNO, bool online);
    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);



    ///////////////////////////

    void signalFileTransferPacketReceived(const FileTransferPacket &packet);

//    void signalFileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);
//    void signalAdminRequestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
//    void signalAdminRequestDownloadFile(SOCKETID socketID, const QString &localBaseDir, const QString &fileName);
//    void signalFileDataRequested(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
//    void signalFileDataReceived(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
//    void signalFileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
//    void signalFileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);


public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
    //    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    //    void heartbeat();
    //    void confirmPacketReceipt(quint16 packetSerialNumber);



private:

    ResourcesManagerInstance *m_resourcesManager;
    UDPServer *m_udpServer;
    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QHostAddress serverAddress;
    quint16 serverRTPListeningPort;


    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;


    QString m_localComputerName;

//        quint16 localUDTListeningPort;
    quint16 m_localTCPServerListeningPort;
    quint16 m_localENETListeningPort;


};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
