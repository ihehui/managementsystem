/*
 ****************************************************************************
 * serverservice.h
 *
 * Created on: 2010-7-12
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
 * Last Modified on: 2010-9-3
 * Last Modified by: 贺辉
 ***************************************************************************
 */



#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include <QTimer>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/clientinfo.h"

#include "packetmanager/serverpacketsparser.h"
#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedService/hservice.h"



namespace HEHUI {


class ServerService : public Service
{
    Q_OBJECT
public:
    ServerService(int argc, char **argv, const QString &serviceName = "ServerService", const QString &description = "Server service for management system.");
    ~ServerService();



signals:


private slots:
    bool startMainService();
    //void saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress);
    void saveClientLog(const QString &assetNO, const QString &clientAddress, quint8 logType, const QString &log, const QString &clientTime);

    void sendServerOnlinePacket();

//    bool updateOrSaveClientInfoToDatabase(ClientInfo *info);
//    void updateOrSaveAllClientsInfoToDatabase();

    void clientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType);
    void processOSInfo(ClientInfo *info, const QByteArray &osData);
    void processHardwareInfo(ClientInfo *info, const QByteArray &hardwareData);
    void processSoftwareInfo(ClientInfo *info, const QByteArray &data);

    void processRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);


    void getRecordsInDatabase();

//    void processHeartbeatPacket(const QString &clientAddress, const QString &computerName);

    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientAssetNO, bool online, const QString &ip, quint16 port);
    void processAdminOnlineStatusChangedPacket(SOCKETID socketID, const QString &adminComputerName, const QString &adminName, bool online);


    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);

    void peerDisconnected(SOCKETID socketID);


private:
    bool openDatabase(bool reopen = false);
    bool saveDataToDB(const QString &statement , QString *errorString = 0);

    bool isRecordExistInDB(const QString &assetNO);


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

    void processArguments(int argc, char **argv);



private:

    ResourcesManagerInstance *resourcesManager;
    ServerPacketsParser *serverPacketsParser;

    UDPServer *m_udpServer;
    RTP *m_rtp;

//    UDTProtocol *m_udtProtocol;
    //UDTSOCKET m_socketConnectedToAdmin;

    bool mainServiceStarted;

    DatabaseUtility *databaseUtility;
    QSqlQuery *query;

    QTimer *sendServerOnlinePacketTimer;

    QHash<QString/*Asset NO.*/, ClientInfo *> clientInfoHash;
    QHash<SOCKETID /*Socket ID*/, QString/*Asset NO.*/> clientSocketsHash;
    QHash<SOCKETID /*Socket ID*/, QString/*Admin Name*/> adminSocketsHash;


    int onlineAdminsCount;
    bool m_isUsingMySQL;


    QStringList recordsInDatabase;
    QStringList logs;


};

} //namespace HEHUI

#endif // SERVERSERVICE_H
