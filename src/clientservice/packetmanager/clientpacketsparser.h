/*
 ****************************************************************************
 * clientpacketsparser.h
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
 * Last Modified On: 2010-7-13
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef CLIENTPACKETSPARSER_H_
#define CLIENTPACKETSPARSER_H_


#include <QDataStream>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>

#include "../clientresourcesmanager.h"
#include "../../sharedms/packets.h"

#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI {


class ClientPacketsParser : public QObject{
    Q_OBJECT
public:
    ClientPacketsParser(const QString &assetNO, ClientResourcesManager *manager, QObject *parent = 0);
    virtual ~ClientPacketsParser();




public slots:
    void parseIncomingPacketData(const PacketBase &packet);

    void changeLocalUserOnlineStatus(SOCKETID userSocketID, bool online, const QString &userName);
    QString userNameOfSocket(SOCKETID socketID);
    SOCKETID socketIDOfUser(const QString &userName);
    QList<SOCKETID> localUserSockets();


    void requestScreenshot(SOCKETID adminSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort);
    void setSocketConnectedToServer(SOCKETID serverSocketID);
    void setSocketConnectedToAdmin(SOCKETID socketID, const QString &adminName);
    void setAssetNO(const QString &assetNO);


    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = IP_MULTICAST_GROUP_PORT){
        qDebug()<<"----sendClientLookForServerPacket(...) targetAddress:"<<targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }
        quint16 port = targetPort;
        if(!port){
            port = IP_MULTICAST_GROUP_PORT;
        }
        
        ServerDiscoveryPacket packet;
        packet.responseFromServer = 0;
        packet.version = QString(APP_VERSION);

        return m_udpServer->sendDatagram(packet.toByteArray(), address, port );
    }


    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &userName){
        qDebug()<<"----sendRequestAnnouncementsPacket(...)";
        return sendRequestAnnouncementsPacket(serverSocketID, "", "", "1", m_assetNO, userName, "-1", "1970-01-01", "2099-01-01");
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

        return m_rtp->sendReliableData(serverSocketID, &packet.toByteArray());
    }

    bool sendUserReplyMessagePacket(SOCKETID socketID, unsigned int announcementID, const QString &sender, const QString &receiver, const QString &receiversAssetNO, const QString &replyMessage){
        qWarning()<<"----sendUserReplyMessagePacket(...):";

        AnnouncementPacket packet;
        packet.InfoType = AnnouncementPacket::ANNOUNCEMENT_REPLY;
        packet.ReplyInfo.announcementID = announcementID;
        packet.ReplyInfo.sender = sender;
        packet.ReplyInfo.receiver = receiver;
        packet.ReplyInfo.receiversAssetNO = receiversAssetNO;
        packet.ReplyInfo.replyMessage = replyMessage;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendClientResponseRemoteConsoleStatusPacket(int adminSocketID, bool running, const QString &extraMessage, quint8 messageType = quint8(MS::MSG_Information)){
        qDebug()<<"----sendClientResponseRemoteConsolePacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_STATE;
        packet.ConsoleState.isRunning = quint8(running);
        packet.ConsoleState.message = extraMessage;
        packet.ConsoleState.messageType = messageType;

        return m_rtp->sendReliableData(adminSocketID, &packet.toByteArray());
    }

    bool sendRemoteConsoleCMDResultFromClientPacket(int adminSocketID, const QString &output){
        qDebug()<<"----sendRemoteConsoleCMDResultFromClientPacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_OUTPUT;
        packet.Output.output = output;

        return m_rtp->sendReliableData(adminSocketID, &packet.toByteArray());
    }

    bool sendClientLogPacket(SOCKETID socketID, quint8 logType, const QString &log){
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;

        ClientLogPacket packet;
        packet.logType = logType;
        packet.log = log;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendClientInfoPacket(SOCKETID socketID, const QByteArray &data, quint8 infoType){

        ClientInfoPacket packet;
        packet.IsRequest = 0;
        packet.infoType = infoType;
        packet.data = data;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendClientInfoPacket(const QString &targetAddress, quint16 targetPort, const QByteArray &data, quint8 infoType){
        //qWarning()<<"----sendClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        ClientInfoPacket packet;
        packet.IsRequest = 0;
        packet.infoType = infoType;
        packet.data = data;

        return m_udpServer->sendDatagram(packet.toByteArray(), QHostAddress(targetAddress), targetPort);
    }

    bool sendClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &computerName, bool verified, quint8 errorCode){
        //qWarning()<<"----sendClientResponseAdminConnectionResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        AdminConnectionToClientPacket packet;
        packet.computerName = computerName;
        packet.verified = quint8(verified);
        packet.errorCode = errorCode;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendClientMessagePacket(int adminSocketID, const QString &message, quint8 clientMessageType = quint8(MS::MSG_Information)){

        MessagePacket packet;
        packet.msgType = clientMessageType;
        packet.message = message;

        return m_rtp->sendReliableData(adminSocketID, &packet.toByteArray());
    }

    bool sendLocalUserOnlineStatusChangedPacket(SOCKETID adminSocketID, const QString &userName, bool online){
        qDebug()<<"--sendLocalUserOnlineStatusChangedPacket(...)"<<" userSocketID:"<<adminSocketID<<" adminName:"<<userName<<" online:"<<online;

        LocalUserOnlineStatusChangedPacket packet;
        packet.userName = userName;
        packet.online = online;

        return m_rtp->sendReliableData(adminSocketID, &packet.toByteArray());
    }
    
    bool sendSystemInfoPacket(SOCKETID socketID, const QString &extraInfo, const QByteArray &data, quint8 infoType){
        qDebug()<<"----sendSystemInfoPacket(...)"<<" socketID:"<<socketID<<" infoType:"<<infoType;

        SystemInfoFromServerPacket packet;
        packet.infoType = infoType;
        packet.data = data;
        packet.extraInfo = extraInfo;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendModifyAssetNOPacket(SOCKETID socketID, bool isRequest, const QString &oldAssetNO, const QString &newAssetNO, const QString &adminID){
        qWarning()<<"----sendClientResponseModifyAssetNOPacket(...) oldAssetNO"<<oldAssetNO<<" newAssetNO:"<<newAssetNO;

        ModifyAssetNOPacket packet;
        packet.isRequest = isRequest;
        packet.oldAssetNO = oldAssetNO;
        packet.newAssetNO = newAssetNO;
        packet.adminID = adminID;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }


    bool sendClientResponseTemperaturesPacket(SOCKETID socketID, const QString &cpuTemperature, const QString &harddiskTemperature){
        qWarning()<<"----sendClientResponseTemperaturesPacket(...):"<<cpuTemperature<<" "<<harddiskTemperature;

        TemperaturesPacket packet;
        packet.InfoType = TemperaturesPacket::TEMPERATURES_RESPONSE;
        packet.TemperaturesResponse.cpuTemperature = cpuTemperature;
        packet.TemperaturesResponse.harddiskTemperature = harddiskTemperature;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }


    bool sendAdminRequestScreenshotPacket(SOCKETID userSocketID, const QString &userName, quint16 adminPort){
        //qDebug()<<"----sendAdminRequestScreenshotPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_REQUEST;
        packet.ScreenshotRequest.userName = userName;
        packet.ScreenshotRequest.adminListeningPort = adminPort;

        return m_rtp->sendReliableData(userSocketID, &packet.toByteArray());
    }

    bool sendClientResponseServiceConfigChangedPacket(SOCKETID socketID, const QString &serviceName, quint64 processID, quint64 startupType){
        qWarning()<<"----sendClientResponseServiceConfigChangedPacket(...):";

        ServiceConfigPacket packet;
        packet.serviceName = serviceName;
        packet.startupType = startupType;
        packet.processID = processID;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

//////////////////////////////
    bool responseFileSystemInfo(SOCKETID socketID, QString parentDirPath, const QByteArray &fileSystemInfoData){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileSystemInfoResponse;
        packet.FileSystemInfoResponse.parentDirPath = parentDirPath;
        packet.FileSystemInfoResponse.fileSystemInfoData = fileSystemInfoData;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool requestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = ""){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingRequest;
        packet.FileUploadingRequest.fileName = fileName;
        packet.FileUploadingRequest.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingRequest.size = size;
        packet.FileUploadingRequest.fileSaveDir = remoteFileSaveDir;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool requestDownloadFile(SOCKETID socketID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingRequest;
        packet.FileDownloadingRequest.baseDir = remoteBaseDir;
        packet.FileDownloadingRequest.fileName = remoteFileName;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool responseFileDownloadRequest(SOCKETID socketID, bool accepted, const QString &baseDir, const QString &fileName, const QByteArray &fileMD5Sum, quint64 size){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingResponse;
        packet.FileDownloadingResponse.accepted = accepted;
        packet.FileDownloadingResponse.baseDir = baseDir;
        packet.FileDownloadingResponse.fileName = fileName;
        packet.FileDownloadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileDownloadingResponse.size = size;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool responseFileUploadRequest(SOCKETID socketID, bool accepted, const QByteArray &fileMD5Sum, const QString &message){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingResponse;
        packet.FileUploadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingResponse.accepted = accepted;
        packet.FileUploadingResponse.message = message;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool requestFileData(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
        qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDataRequest;
        packet.FileDataRequest.fileMD5 = fileMD5;
        packet.FileDataRequest.startPieceIndex = startPieceIndex;
        packet.FileDataRequest.endPieceIndex = endPieceIndex;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool sendFileData(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *pieceMD5){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileData;
        packet.FileDataResponse.fileMD5 = fileMD5;
        packet.FileDataResponse.pieceIndex = pieceIndex;
        packet.FileDataResponse.data = *data;
        packet.FileDataResponse.pieceMD5 = *pieceMD5;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }
    
    bool fileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXStatus;
        packet.FileTXStatus.fileMD5 = fileMD5;
        packet.FileTXStatus.status = status;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    bool fileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXError;
        packet.FileTXError.fileMD5 = fileMD5;
        packet.FileTXError.errorCode = errorCode;
        packet.FileTXError.message = errorString;

        return m_rtp->sendReliableData(socketID, &packet.toByteArray());
    }

    
private slots:


signals:
    //void  signalHeartbeatPacketReceived(const QString &computerName);
    //void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    void signalServerOnlineStatusChangedPacketReceived(bool online, const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    void signalClientInfoRequestedPacketReceived(SOCKETID socketID, quint8 infoType);
    void signalAdminRequestRemoteConsolePacketReceived(const QString &applicationPath, bool startProcess);
    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    void signalSystemInfoFromServerReceived(const QString &extraInfo, const QByteArray &clientInfo, quint8 infoType);

    //void signalServerAnnouncementPacketReceived(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, const QString &userName, bool mustRead);

    void signalUpdateClientSoftwarePacketReceived();

    void signalSetupUSBSDPacketReceived(quint8 usbSTORStatus);
    void signalShowAdminPacketReceived(bool show);
    void signalModifyAdminGroupUserPacketReceived(const QString &assetNO, const QString &userName, bool addToAdminGroup, const QString &adminName, const QString &adminAddress, quint16 adminPort);

    void signalModifyAssetNOPacketReceived(const QString &newAssetNO);
    void signalAssetNOModifiedPacketReceived(const QString &newAssetNO);

    void signalRenameComputerPacketReceived(const QString &newComputerName, const QString &domainAdminName, const QString &domainAdminPassword);
    void signalJoinOrUnjoinDomainPacketReceived(bool join, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword);

    void signalAdminRequestConnectionToClientPacketReceived(SOCKETID socketID, const QString &adminComputerName, const QString &users);
    void signalAdminSearchClientPacketReceived(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName);
    
//    void signalLocalUserOnlineStatusChanged(SOCKETID userSocketID, const QString &userName, bool online);

    void signalAdminRequestTemperatures(SOCKETID adminSocketID, bool cpu, bool harddisk);
//    void signalAdminRequestScreenshot(SOCKETID socketID, const QString &userName, bool fullScreen);

    void signalAdminRequestShutdownPacketReceived(SOCKETID adminSocketID, const QString &message, quint32 waitTime, bool force, bool reboot);
    void signalAdminRequestLockWindowsPacketReceived(SOCKETID adminSocketID, const QString &userName, bool logoff);
    void signalAdminRequestCreateOrModifyWinUserPacketReceived(SOCKETID adminSocketID, const QByteArray &userData);


    void signalAdminRequestChangeServiceConfigPacketReceived(SOCKETID socketID, const QString &serviceName, bool startService, unsigned long startupType);
    void signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules);


///////////////////////////
    void signalFileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);

    void signalAdminRequestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalAdminRequestDownloadFile(SOCKETID socketID, const QString &localBaseDir, const QString &fileName);
    void signalFileDataRequested(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

private:


private:
    QString m_assetNO;
    //QString m_localComputerName;

    QHostAddress serverAddress;
    quint16 serverRTPListeningPort;
    QString serverName;


    QHostAddress m_localUDTListeningAddress;
//    quint16 m_localUDTServerListeningPort;
    quint16 m_localTCPServerListeningPort;
    quint16 m_localENETListeningPort;

    ClientResourcesManager *m_resourcesManager;

    UDPServer *m_udpServer;

    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QMutex m_localUserSocketsHashMutex;
    QHash<SOCKETID /*Socket ID*/, QString /*User Name*/> m_localUserSocketsHash;

    SOCKETID m_socketConnectedToServer;
    SOCKETID m_socketConnectedToAdmin;
    QString m_adminName;

};

}

#endif /* CLIENTPACKETSPARSER_H_ */
