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


namespace HEHUI
{

class SHAREDMSLIB_API ClientInfo : public QObject
{

public:
    ClientInfo(const QString &assetNO = "", QObject *parent = 0);
    virtual ~ClientInfo();

    ClientInfo &operator = (const ClientInfo &clientInfo);

public:
    bool isValid();
    void setJsonData(const QByteArray &data);
    QByteArray getOSJsonData() const;
    QByteArray getHardwareJsonData() const;


    void setAssetNO(const QString &assetNO);
    QString getAssetNO() const;


    void setIsJoinedToDomain(bool joined)
    {
        this->m_isJoinedToDomain = joined;
    }

    bool isJoinedToDomain() const
    {
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

//    bool getHardwareInfoSavedTODatabase() const
//    {
//        return this->hardwareInfoSavedTODatabase;
//    }

    QString getInstallationDate() const
    {
        return this->installationDate;
    }

//    QDateTime getLastHeartbeatTime() const
//    {
//        return lastHeartbeatTime;
//    }

    QString getLastOnlineTime() const
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

    bool isOnline() const
    {
        return online;
    }

    QString getOSVersion() const
    {
        return osVersion;
    }

    QString getOsKey() const
    {
        return osKey;
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

    void setInstallationDate(const QString &installationDate)
    {
        this->installationDate = installationDate;
    }


    void setLastOnlineTime(const QString &lastOnlineTime)
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

    void setOSVersion(QString os)
    {
        this->osVersion = os;
    }

    void setOsKey(QString osKey)
    {
        this->osKey = osKey;
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

    QString getOnlineUsers() const
    {
        return onlineUsers;
    }
    void setOnlineUsers(const QString &users)
    {
        this->onlineUsers = users;
    }


    void setIP(const QString &ipInfo)
    {
        this->ip = ipInfo;
    }
    QString getIP() const
    {
        return this->ip;
    }

    void setExternalIPInfo(const QString &ipInfo)
    {
        this->externalIPInfo = ipInfo;
    }
    QString getExternalIPInfo() const
    {
        return this->externalIPInfo;
    }

    bool isProcessMonitorEnabled() const
    {
        return processMonitorEnabled;
    }
    void setProcessMonitorEnabled(bool enable)
    {
        this->processMonitorEnabled = enable;
    }

    void setCPULoad(int cpuLoad)
    {
        this->cpuLoad = cpuLoad;
    }
    int getCPULoad()
    {
        return this->cpuLoad;
    }

    void setMemoryLoad(int memoryLoad)
    {
        this->memoryLoad = memoryLoad;
    }
    int getMemoryLoad()
    {
        return memoryLoad;
    }

private:

    QString assetNO;
    QString computerName;
    QString osVersion;
    QString installationDate;
    QString osKey;
    QString workgroup;
    bool m_isJoinedToDomain;
    QString users;
    QString administrators;
    QString ip;
    QString externalIPInfo;
    QString clientVersion;
    bool processMonitorEnabled;
    QString lastOnlineTime;


    QString cpu;
    QString memory;
    QString motherboardName;
    QString video;
    QString monitor;
    QString audio;
    QString storage;
    QString network;
    MS::USBSTORStatus usbSDStatus;

    int cpuLoad;
    int memoryLoad;



    bool online;

    QString clientUDTListeningAddress;
    quint16 clientUDTListeningPort;

    QString onlineUsers;



};

}

#endif /* CLIENTINFO_H_ */
