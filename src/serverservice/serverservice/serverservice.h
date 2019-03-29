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
#include "../../sharedms/adminuserinfo.h"

#include "packetmanager/serverpacketsparser.h"
#include "../resourcesmanagerinstance.h"

#include "HHSharedCore/DatabaseUtility"
#include "HHSharedService/Service"



namespace HEHUI
{


class ServerService : public Service
{
    Q_OBJECT
public:
    ServerService(int argc, char **argv, const QString &serviceName = "ServerService", const QString &description = "Server service for management system.");
    ~ServerService();



signals:
    void signalSendRealtimeInfo(int cpuLoad, int memoryLoad);


private slots:
    bool startMainService();
    //void saveClientLog(const QString &computerName, const QString &users, const QString &log, const QString &clientAddress);
    void saveClientLog(const ClientLogPacket &packet);

    void sendServerOnlinePacket();

//    bool updateOrSaveClientInfoToDatabase(ClientInfo *info);
//    void updateOrSaveAllClientsInfoToDatabase();

    void processDataForwardPacket(const DataForwardPacket &packet);

    void processClientInfoPacket(const ClientInfoPacket &packet);
    void clientInfoRequested(SOCKETID socketID, const QString &assetNO, quint8 infoType);

    void clientInfoPacketReceived(const QString &assetNO, const QByteArray &clientInfo, quint8 infoType, const QString &externalIPInfo);
    void processOSInfo(ClientInfo *info, const QByteArray &osData);
    void processHardwareInfo(ClientInfo *info, const QByteArray &hardwareData);
    void processSoftwareInfo(ClientInfo *info, const QByteArray &data);


    void processUpdateSysAdminInfoPacket(const SysAdminInfoPacket &packet);

    void processSystemAlarmsPacket(const SystemAlarmsPacket &packet);
    void sendAlarmsInfo(SOCKETID socketID, const QString &assetNO, const QString &type, const QString &acknowledged, const QString &startTime, const QString &endTime);
    void acknowledgeSystemAlarms(SOCKETID adminSocketID, const QString &alarms, bool deleteAlarms);

    void sendRealtimeInfo(int cpuLoad, int memoryLoad);

    void processAnnouncementPacket(const AnnouncementPacket &packet);
    bool sendAnnouncementsInfo(SOCKETID socketID, const QString &id, const QString &keyword, const QString &validity, const QString &assetNO, const QString &userName, const QString &target, const QString &startTime, const QString &endTime);
    bool createAnnouncement(SOCKETID adminSocketID, quint32 jobID, unsigned int tempID, const QString &adminName, quint8 type, const QString &content, bool confirmationRequired, int validityPeriod, quint8 targetType, const QString &targets);
    bool updateAnnouncement(SOCKETID adminSocketID, quint32 jobID, const QString &adminName, unsigned int announcementID, quint8 targetType, bool active, const QString &addedTargets, const QString &deletedTargets);
    bool updateAnnouncementTargets(unsigned int announcementID, const QString &addedTargets, const QString &deletedTargets);
    bool sendAnnouncementTargetsInfo(SOCKETID socketID, unsigned int announcementID);
    void replyMessageReceived(SOCKETID socketID, const QString &senderAssetNO, unsigned int announcementID, const QString &sender, const QString &receiver,  const QString &receiversAssetNO, const QString &message);
    bool sendAnnouncementRepliesInfo(SOCKETID socketID, const QString &announcementID, const QString &receiver);

    void processModifyAssetNOPacket(const ModifyAssetNOPacket &packet);
    void processProcessMonitorInfoPacket(const ProcessMonitorInfoPacket &packet);

    void processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientAssetNO, bool online, const QString &ip, quint16 port);

    void processAdminLoginPacket(const AdminLoginPacket &packet);
    void processAdminOnlineStatusChangedPacket(SOCKETID socketID, const QString &adminComputerName, const QString &adminName, bool online);
    void processAdminConnectionToClientPacket(const AdminConnectionToClientPacket &packet);
    bool isPacketFromOnlinePeer(const MSPacket *packet, bool adminPacket);

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);
    void peerDisconnected(SOCKETID socketID);


private:
    bool openDatabase(bool reopen = false);
    bool execQuery(const QString &statement , QString *errorString = 0);
    void getAllClientsInfoFromDB();
    void getAllAdminsInfoFromDB();
    void getSystemAlarmsCount();
    bool getAnnouncementsInfo(QByteArray *data, const QString &id, const QString &keyword, const QString &validity, const QString &assetNO, const QString &userName, const QString &target, const QString &startTime, const QString &endTime);


    unsigned int getCurrentDBUTCTime();

    void getOSInfo(SOCKETID socketID, const QString &assetNO);
    void getHardwareInfo(SOCKETID socketID, const QString &assetNO);
    void getSoftwareInfo(SOCKETID socketID, const QString &assetNO);

    void sendAdminsInfo(SOCKETID socketID);

    void sendServerInfo(SOCKETID adminSocketID);

    void startGetingRealTimeResourcesLoad();
    void stopGetingRealTimeResourcesLoad();
    void getRealTimeResourcseLoad();


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

    void processArguments(int argc, char **argv);



private:
    unsigned int m_startupUTCTime;

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

    QHash<QString /*Admin ID*/, AdminUserInfo *> adminsHash;
    QHash<SOCKETID /*Admin SOCKETID ID*/, QString /*Admin ID*/> onlineAdminSockets;


    //int onlineAdminsCount;
    bool m_isUsingMySQL;

    bool m_getRealTimeResourcesLoad;
    int m_realTimeResourcseLoadInterval; //ms

    uint m_unacknowledgedAlarmsCount;
    uint m_totalAlarmsCount;
    QString m_disksInfo;
    int m_disksInfoCounter;

};

} //namespace HEHUI

#endif // SERVERSERVICE_H
