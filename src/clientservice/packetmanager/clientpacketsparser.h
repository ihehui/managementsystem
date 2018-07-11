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
#include "../../sharedms/mspackets.h"

#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI
{


class ClientPacketsParser : public QObject
{
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


    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS), quint16 targetPort = IP_MULTICAST_GROUP_PORT)
    {
        qDebug() << "----sendClientLookForServerPacket(...) targetAddress:" << targetAddress;

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()) {
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }
        quint16 port = targetPort;
        if(!port) {
            port = IP_MULTICAST_GROUP_PORT;
        }

        ServerDiscoveryPacket packet;
        packet.responseFromServer = 0;
        packet.version = QString(APP_VERSION);
        packet.udpPort = 0;

        QByteArray ba = packet.toByteArray();
        return m_udpServer->sendDatagram(ba, address, port );
    }


    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &userName)
    {
        qDebug() << "----sendRequestAnnouncementsPacket(...)";
        return sendRequestAnnouncementsPacket(serverSocketID, "", "", "1", m_assetNO, userName, "-1", "1970-01-01", "2099-01-01");
    }

    bool sendRequestAnnouncementsPacket(SOCKETID serverSocketID, const QString &id, const QString &keyword, const QString &validity, const QString &assetNO, const QString &userName, const QString &target, const QString &startTime, const QString &endTime)
    {
        qDebug() << "----sendRequestAnnouncementsPacket(...)";

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

    bool sendUserReplyMessagePacket(SOCKETID socketID, unsigned int announcementID, const QString &sender, const QString &receiver, const QString &receiversAssetNO, const QString &replyMessage)
    {
        qWarning() << "----sendUserReplyMessagePacket(...):";

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

    bool sendClientResponseRemoteConsoleStatusPacket(int adminSocketID, bool running, const QString &extraMessage, quint8 messageType = quint8(MS::MSG_Information))
    {
        qDebug() << "----sendClientResponseRemoteConsolePacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_STATE;
        packet.ConsoleState.isRunning = quint8(running);
        packet.ConsoleState.message = extraMessage;
        packet.ConsoleState.messageType = messageType;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendRemoteConsoleCMDResultFromClientPacket(int adminSocketID, const QString &output)
    {
        qDebug() << "----sendRemoteConsoleCMDResultFromClientPacket(...)";

        RemoteConsolePacket packet;
        packet.InfoType = RemoteConsolePacket::REMOTECONSOLE_OUTPUT;
        packet.Output.output = output;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendClientLogPacket(SOCKETID socketID, quint8 logType, const QString &log)
    {
        //qWarning()<<"----sendClientLogPacket(...) log:"<<log;

        ClientLogPacket packet;
        packet.logType = logType;
        packet.log = log;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientInfoPacket(SOCKETID socketID, const QByteArray &data, quint8 infoType)
    {

        ClientInfoPacket packet;
        packet.IsRequest = 0;
        packet.infoType = infoType;
        packet.data = data;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientInfoPacket(const QString &targetAddress, quint16 targetPort, const QByteArray &data, quint8 infoType)
    {
        //qWarning()<<"----sendClientInfoPacket(...)"<<" targetAddress:"<<targetAddress<<" targetPort:"<<targetPort;

        ClientInfoPacket packet;
        packet.IsRequest = 0;
        packet.infoType = infoType;
        packet.data = data;

        QByteArray ba = packet.toByteArray();
        return m_udpServer->sendDatagram(ba, QHostAddress(targetAddress), targetPort);
    }

    bool sendClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &computerName, bool verified, quint8 errorCode)
    {
        //qWarning()<<"----sendClientResponseAdminConnectionResultPacket(...):"<<adminAddress.toString()<<" "<<adminPort;

        AdminConnectionToClientPacket packet;
        packet.computerName = computerName;
        packet.verified = quint8(verified);
        packet.errorCode = errorCode;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendClientMessagePacket(int adminSocketID, const QString &message, quint8 clientMessageType = quint8(MS::MSG_Information))
    {

        MessagePacket packet;
        packet.msgType = clientMessageType;
        packet.message = message;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendLocalUserOnlineStatusChangedPacket(SOCKETID adminSocketID, const QString &userName, bool online)
    {
        qDebug() << "--sendLocalUserOnlineStatusChangedPacket(...)" << " userSocketID:" << adminSocketID << " adminName:" << userName << " online:" << online;

        LocalUserOnlineStatusChangedPacket packet;
        packet.userName = userName;
        packet.online = online;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(adminSocketID, &ba);
    }

    bool sendSystemInfoPacket(SOCKETID socketID, const QString &extraInfo, const QByteArray &data, quint8 infoType)
    {
        qDebug() << "----sendSystemInfoPacket(...)" << " socketID:" << socketID << " infoType:" << infoType;

        SystemInfoFromServerPacket packet;
        packet.infoType = infoType;
        packet.data = data;
        packet.extraInfo = extraInfo;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendModifyAssetNOPacket(SOCKETID socketID, bool isRequest, const QString &oldAssetNO, const QString &newAssetNO, const QString &adminID)
    {
        qWarning() << "----sendClientResponseModifyAssetNOPacket(...) oldAssetNO" << oldAssetNO << " newAssetNO:" << newAssetNO;

        ModifyAssetNOPacket packet;
        packet.isRequest = isRequest;
        packet.oldAssetNO = oldAssetNO;
        packet.newAssetNO = newAssetNO;
        packet.adminID = adminID;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendClientResponseTemperaturesPacket(SOCKETID socketID, const QString &cpuTemperature, const QString &harddiskTemperature)
    {
        qWarning() << "----sendClientResponseTemperaturesPacket(...):" << cpuTemperature << " " << harddiskTemperature;

        TemperaturesPacket packet;
        packet.InfoType = TemperaturesPacket::TEMPERATURES_RESPONSE;
        packet.TemperaturesResponse.cpuTemperature = cpuTemperature;
        packet.TemperaturesResponse.harddiskTemperature = harddiskTemperature;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }


    bool sendAdminRequestScreenshotPacket(SOCKETID userSocketID, const QString &userName, const QString &adminAddress, quint16 adminPort)
    {
        //qDebug()<<"----sendAdminRequestScreenshotPacket(...):";

        ScreenshotPacket packet;
        packet.InfoType = ScreenshotPacket::SCREENSHOT_REQUEST;
        packet.ScreenshotRequest.userName = userName;
        packet.ScreenshotRequest.adminAddress = adminAddress;
        packet.ScreenshotRequest.adminListeningPort = adminPort;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(userSocketID, &ba);
    }

    bool sendClientResponseServiceConfigChangedPacket(SOCKETID socketID, const QString &serviceName, quint64 processID, quint64 startupType)
    {
        qWarning() << "----sendClientResponseServiceConfigChangedPacket(...):";

        ServiceConfigPacket packet;
        packet.serviceName = serviceName;
        packet.startupType = startupType;
        packet.processID = processID;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

//////////////////////////////
    bool responseFileSystemInfo(SOCKETID socketID, const QString &baseDirPath, const QByteArray &fileSystemInfoData)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileSystemInfoResponse;
        packet.FileSystemInfoResponse.baseDirPath = baseDirPath;
        packet.FileSystemInfoResponse.fileSystemInfoData = fileSystemInfoData;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseDeletingFiles(SOCKETID socketID, const QString &baseDirPath, const QStringList &failedFiles)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDeletingResponse;
        packet.FileDeletingResponse.baseDirPath = baseDirPath;
        packet.FileDeletingResponse.failedFiles = failedFiles;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseRenamingFiles(SOCKETID socketID, const QString &baseDirPath, const QString &oldFileName, const QString &newFileName,bool renamed, const QString &message)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileRenamingResponse;
        packet.FileRenamingResponse.baseDirPath = baseDirPath;
        packet.FileRenamingResponse.oldFileName = oldFileName;
        packet.FileRenamingResponse.newFileName = newFileName;
        packet.FileRenamingResponse.renamed = renamed;
        packet.FileRenamingResponse.message = message;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = "")
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingRequest;
        packet.FileUploadingRequest.fileName = fileName;
        packet.FileUploadingRequest.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingRequest.size = size;
        packet.FileUploadingRequest.fileSaveDir = remoteFileSaveDir;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDownloadFile(SOCKETID socketID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingRequest;
        packet.FileDownloadingRequest.baseDir = remoteBaseDir;
        packet.FileDownloadingRequest.fileName = remoteFileName;
        packet.FileDownloadingRequest.dirToSaveFile = localFileSaveDir;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileDownloadRequest(SOCKETID socketID, bool accepted, const QString &baseDir, const QString &fileName, const QByteArray &fileMD5Sum, quint64 size, const QString &pathToSaveFile, quint8 errorCode = ERROR_CODE_NO_ERROR)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDownloadingResponse;
        packet.FileDownloadingResponse.accepted = accepted;
        packet.FileDownloadingResponse.baseDir = baseDir;
        packet.FileDownloadingResponse.fileName = fileName;
        packet.FileDownloadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileDownloadingResponse.size = size;
        packet.FileDownloadingResponse.pathToSaveFile = pathToSaveFile;
        packet.FileDownloadingResponse.errorCode = errorCode;


        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileUploadRequest(SOCKETID socketID, bool accepted, const QByteArray &fileMD5Sum, const QString &message)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileUploadingResponse;
        packet.FileUploadingResponse.fileMD5Sum = fileMD5Sum;
        packet.FileUploadingResponse.accepted = accepted;
        packet.FileUploadingResponse.message = message;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestFileData(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex)
    {
        //qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileDataRequest;
        packet.FileDataRequest.fileMD5 = fileMD5;
        packet.FileDataRequest.startPieceIndex = startPieceIndex;
        packet.FileDataRequest.endPieceIndex = endPieceIndex;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendFileData(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *pieceMD5)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileData;
        packet.FileDataResponse.fileMD5 = fileMD5;
        packet.FileDataResponse.pieceIndex = pieceIndex;
        packet.FileDataResponse.data = *data;
        packet.FileDataResponse.pieceMD5 = *pieceMD5;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXStatus;
        packet.FileTXStatus.fileMD5 = fileMD5;
        packet.FileTXStatus.status = status;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString)
    {

        FileTransferPacket packet;
        packet.InfoType = FileTransferPacket::FT_FileTXError;
        packet.FileTXError.fileMD5 = fileMD5;
        packet.FileTXError.errorCode = errorCode;
        packet.FileTXError.message = errorString;

        QByteArray ba = packet.toByteArray();
        return m_rtp->sendReliableData(socketID, &ba);
    }


private slots:


signals:

    void signalServerDeclarePacketReceived(const ServerDiscoveryPacket &packet);
    void signalClientInfoRequestedPacketReceived(const ClientInfoPacket &packet);
    void signalRemoteConsolePacketReceived(const RemoteConsolePacket &packet);
    void signalSystemInfoFromServerReceived(const SystemInfoFromServerPacket &packet);
    void signalSetupUSBSDPacketReceived(const USBDevPacket &packet);

    void signalModifyAssetNOPacketReceived(const QString &newAssetNO);
    void signalAssetNOModifiedPacketReceived(const QString &newAssetNO);
    void signalModifyAssetNOPacketReceived(const ModifyAssetNOPacket &packet);

    void signalRenameComputerPacketReceived(const RenameComputerPacket &packet);
    void signalJoinOrUnjoinDomainPacketReceived(const JoinOrUnjoinDomainPacket &packet);


    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    void signalAdminRequestConnectionToClientPacketReceived(const AdminConnectionToClientPacket &packet);
    void signalAdminSearchClientPacketReceived(const AdminSearchClientPacket &packet);

    void signalAdminRequestTemperatures(const TemperaturesPacket &packet);

    void signalUpdateClientSoftwarePacketReceived();

    void signalShowAdminPacketReceived(bool show);

    void signalAdminRequestShutdownPacketReceived(const ShutdownPacket &packet);
    void signalAdminRequestLockWindowsPacketReceived(const LockWindowsPacket &packet);

    void signalAdminRequestChangeServiceConfigPacketReceived(const ServiceConfigPacket &packet);
    void signalWinUserPacketReceived(const WinUserPacket &packet);
    void signalRequestChangeProcessMonitorInfoPacketReceived(const ProcessMonitorInfoPacket &packet);


    ///////////////////////////
    void signalFileTransferPacketReceived(const FileTransferPacket &packet);

//    void signalFileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath);
//    void signalAdminRequestUploadFile(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
//    void signalAdminRequestDownloadFile(SOCKETID socketID, const QString &localBaseDir, const QString &fileName);
//    void signalFileDataRequested(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
//    void signalFileDataReceived(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
//    void signalFileTXStatusChanged(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
//    void signalFileTXError(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);

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

#ifdef UDT_ENABLED
    UDTProtocol *m_udtProtocol;
#endif

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
