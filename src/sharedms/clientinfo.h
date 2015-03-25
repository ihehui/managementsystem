/*
 * clientinfo.h
 *
 *  Created on: 2010-9-27
 *      Author: HeHui
 */

#ifndef CLIENTINFO_H_
#define CLIENTINFO_H_


#include <QObject>
#include <QString>
#include <QDateTime>
#include <QStringList>

#include "sharedmslib.h"
#include "../sharedms/global_shared.h"


namespace HEHUI {

class SHAREDMSLIB_API ClientInfo : public QObject{

public:
    ClientInfo(const QString &computerName = "", QObject *parent = 0);
    virtual ~ClientInfo();

    ClientInfo & operator = (const ClientInfo &clientInfo);

public:
    bool isValid();
    void setJsonData(const QByteArray &data);
    QByteArray getJsonData() const;




    void setIsJoinedToDomain(bool joined){
        this->m_isJoinedToDomain = joined;
    }

    bool isJoinedToDomain() const{
        return this->m_isJoinedToDomain;
    }

    QString getStorage() const
    {
        return this->storage;
    }

    void setStorage(QString storage)
    {
        this->storage = storage;
    }

    QString getAdministrators() const
    {
        return this->administrators;
    }

    QString getAudio() const
    {
        return this->audio;
    }

    QString getClientUDTListeningAddress() const
    {
        return this->clientUDTListeningAddress;
    }

    quint16 getClientUDTListeningPort() const
    {
        return this->clientUDTListeningPort;
    }

    QString getClientVersion() const
    {
        return this->clientVersion;
    }

    QString getComputerName() const
    {
        return this->computerName;
    }

    QString getCpu() const
    {
        return this->cpu;
    }

    bool getDetailedInfoSavedTODatabase() const
    {
        return this->detailedInfoSavedTODatabase;
    }

    QString getInstallationDate() const
    {
        return this->installationDate;
    }

    QDateTime getLastHeartbeatTime() const
    {
        return lastHeartbeatTime;
    }

    QDateTime getLastOnlineTime() const
    {
        return lastOnlineTime;
    }

    QString getMemory() const
    {
        return memory;
    }

    QString getMonitor() const
    {
        return monitor;
    }

    QString getMotherboardName() const
    {
        return motherboardName;
    }

    QString getNetwork() const
    {
        return network;
    }

    bool getOnline() const
    {
        return online;
    }

    QString getOs() const
    {
        return os;
    }

    QString getOsKey() const
    {
        return osKey;
    }

    bool getProgramsEnabled() const
    {
        return programsEnabled;
    }

    bool getSummaryInfoSavedTODatabase() const
    {
        return summaryInfoSavedTODatabase;
    }

    QString getUpdateHardwareInfoStatement() const
    {
        return updateHardwareInfoStatement;
    }

    QString getUpdateOSInfoStatement() const
    {
        return updateOSInfoStatement;
    }

    MS::USBSTORStatus getUsbSDStatus() const
    {
        return usbSDStatus;
    }

    QString getUsers() const
    {
        return users;
    }

    QString getVideo() const
    {
        return video;
    }


    QString getWorkgroup() const
    {
        return workgroup;
    }

    void setAdministrators(QString administrators)
    {
        this->administrators = administrators;
    }

    void setAudio(QString audio)
    {
        this->audio = audio;
    }


    void setClientUDTListeningAddress(QString clientUDTListeningAddress)
    {
        this->clientUDTListeningAddress = clientUDTListeningAddress;
    }

    void setClientUDTListeningPort(quint16 clientUDTListeningPort)
    {
        this->clientUDTListeningPort = clientUDTListeningPort;
    }

    void setClientVersion(QString clientVersion)
    {
        this->clientVersion = clientVersion;
    }

    void setComputerName(QString computerName)
    {
        this->computerName = computerName;
    }

    void setCpu(QString cpu)
    {
        this->cpu = cpu;
    }

    void setHardwareInfoSavedTODatabase(bool detailedInfoSavedTODatabase)
    {
        this->detailedInfoSavedTODatabase = detailedInfoSavedTODatabase;
    }

    void setInstallationDate(QString installationDate)
    {
        this->installationDate = installationDate;
    }

    void setLastHeartbeatTime(QDateTime lastHeartbeatTime)
    {
        this->lastHeartbeatTime = lastHeartbeatTime;
    }

    void setLastOnlineTime(QDateTime lastOnlineTime)
    {
        this->lastOnlineTime = lastOnlineTime;
    }

    void setMemory(QString memory)
    {
        this->memory = memory;
    }

    void setMonitor(QString monitor)
    {
        this->monitor = monitor;
    }

    void setMotherboardName(QString motherboardName)
    {
        this->motherboardName = motherboardName;
    }

    void setNetwork(QString network)
    {
        this->network = network;
    }

    void setOnline(bool online)
    {
        this->online = online;
    }

    void setOs(QString os)
    {
        this->os = os;
    }

    void setOsKey(QString osKey)
    {
        this->osKey = osKey;
    }

    void setProgramsEnabled(bool programsEnabled)
    {
        this->programsEnabled = programsEnabled;
    }

    void setOSInfoSavedTODatabase(bool summaryInfoSavedTODatabase)
    {
        this->summaryInfoSavedTODatabase = summaryInfoSavedTODatabase;
    }

    void setUpdateHardwareInfoStatement(QString updateDetailedInfoStatement)
    {
        this->updateHardwareInfoStatement = updateDetailedInfoStatement;
    }

    void setUpdateOSInfoStatement(QString updateSummaryInfoStatement)
    {
        this->updateOSInfoStatement = updateSummaryInfoStatement;
    }

    void setUsbSDStatus(quint8 usbSDStatus)
    {
        this->usbSDStatus = MS::USBSTORStatus(usbSDStatus);
    }

    void setUsers(QString users)
    {
        this->users = users;
    }

    void setVideo(QString video)
    {
        this->video = video;
    }

    void setWorkgroup(QString workgroup)
    {
        this->workgroup = workgroup;
    }

    //    QStringList getInstalledSoftwaresInfo(){
    //        return installedSoftwaresInfo;
    //    }
    //    void setInstalledSoftwaresInfo(const QStringList &list){
    //        this->installedSoftwaresInfo = list;
    //    }

    int getinstalledSoftwaresCount() const{
        return this->installedSoftwaresCount;
    }
    void setInstalledSoftwaresCount(int count){
        this->installedSoftwaresCount = count;
    }

    QString getUpdateInstalledSoftwaresInfoStatement() const{
        return updateInstalledSoftwaresInfoStatement;
    }
    void setUpdateInstalledSoftwaresInfoStatement(const QString &statement){
        this->updateInstalledSoftwaresInfoStatement = statement;
    }
    bool isInstalledSoftwaresInfoSavedTODatabase() const{
        return installedSoftwaresInfoSavedTODatabase;
    }
    void setInstalledSoftwaresInfoSavedTODatabase(bool saved){
        this->installedSoftwaresInfoSavedTODatabase = saved;
    }

    void setUpdateAlarmsInfoStatement(const QString &statement){
        this->updateAlarmsInfoStatement = statement;
    }
    QString getUpdateAlarmsInfoStatement()const {
        return updateAlarmsInfoStatement;
    }


    QString getOnlineUsers() const{
        return onlineUsers;
    }
    void setOnlineUsers(const QString &users){
        this->onlineUsers = users;
    }


    void setIPInfo(const QString &ipInfo){
        this->ipInfo = ipInfo;
    }
    QString getIPInfo() const {
        return this->ipInfo;
    }

private:

    QString computerName;
    QString os;
    QString installationDate;
    QString osKey;
    QString workgroup;
    bool m_isJoinedToDomain;
    QString users;
    QString administrators;
    QString ipInfo;
    QString clientVersion;


    QString cpu;
    QString memory;
    QString motherboardName;
    QString video;
    QString monitor;
    QString audio;
    QString storage;
    QString network;
    MS::USBSTORStatus usbSDStatus;




    bool programsEnabled;
    QDateTime lastOnlineTime;

    QString updateOSInfoStatement;
    bool summaryInfoSavedTODatabase;


    ///////////////////////////////////////////
    QString updateHardwareInfoStatement;
    bool detailedInfoSavedTODatabase;

    /////////////////////////////////////////////////

    //    QStringList installedSoftwaresInfo;
    int installedSoftwaresCount;
    QString updateInstalledSoftwaresInfoStatement;
    bool installedSoftwaresInfoSavedTODatabase;

    /////////////////////////////////////////////////

    QString updateAlarmsInfoStatement;
    //bool alarmsInfoSavedTODatabase;


    QDateTime lastHeartbeatTime;

    bool online;

    QString clientUDTListeningAddress;
    quint16 clientUDTListeningPort;

    QString onlineUsers;






};

}

#endif /* CLIENTINFO_H_ */
