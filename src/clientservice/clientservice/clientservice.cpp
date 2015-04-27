
 #include <QtCore>
#include <QDebug>

#include <QHostInfo>
#include <QByteArray>

#include "clientservice.h"
#include "../sharedms/global_shared.h"

#include "HHSharedNetwork/hnetworkutilities.h"
#include "HHSharedCore/hutilities.h"



namespace HEHUI {


ClientService::ClientService(int argc, char **argv, const QString &serviceName, const QString &description )
    :Service(argc, argv, serviceName, description)
{

    qWarning()<<"ClientService::ClientService(...)";

    setStartupType(QtServiceController::AutoStartup);
    //    setServiceFlags(CanBeSuspended);




    resourcesManager = 0;
    clientPacketsParser = 0;

    m_udpServer = 0;
    m_rtp = 0;

//    m_udtProtocol = 0;
    m_socketConnectedToServer = INVALID_SOCK_ID;
    m_socketConnectedToAdmin = INVALID_SOCK_ID;
    peerSocketThatRequiresDetailedInfo = INVALID_SOCK_ID;


//    m_udtProtocolForFileTransmission = 0;

    mainServiceStarted = false;

    databaseUtility = 0;

    settings = 0;
    m_encryptionKey = QString(CRYPTOGRAPHY_KEY).toUtf8();

    m_localAssetNO = "";
    m_localComputerName = QHostInfo::localHostName().toLower();

    m_isJoinedToDomain = false;
    m_joinInfo = "";

#ifdef Q_OS_WIN32

//    m_wm = new WindowsManagement(this);
//    if(m_localAssetNO.trimmed().isEmpty()){
//        m_localAssetNO = WinUtilities::getComputerName().toLower();
//    }

//    m_joinInfo = WinUtilities::getJoinInformation(&m_isJoinedToDomain).toLower();
//    if(m_joinInfo.trimmed().isEmpty()){
//        qCritical()<< tr("Failed to get join information!");
//    }
//    if(m_isJoinedToDomain){
//        WinUtilities::getComputerNameInfo(&m_joinInfo, 0, 0);
//    }

    m_hardwareMonitor = 0;

#endif

    process = 0;


    m_adminAddress = "";
    m_adminPort = 0;


    lookForServerTimer = 0;


    m_serverAddress = QHostAddress::Null;
    m_serverUDTListeningPort = 0;

    m_serverName = "";
    m_serverInstanceID = 0;

    m_fileManager = 0;

//    fileTXWithAdmin = 0;
//    fileTXWithAdminStatus = MS::File_TX_Done;



    m_procMonEnabled = false;
    m_processMonitor = 0;

    m_myInfo = 0;
//    m_myInfo = new ClientInfo(m_localComputerName, this);

    systemInfo = 0;


    processArguments(argc, argv);



}

ClientService::~ClientService(){
    //    logMessage("~ClientService()-1:"+QDateTime::currentDateTime().toString("ss.zzz"), QtServiceBase::Information);

    disconnect();

    if(process){
        process->stopProcess();
        delete process;
        process = 0;
    }

    if(resourcesManager){
        //resourcesManager->closeAllServers();
    }

    if(clientPacketsParser){
        delete clientPacketsParser;
        clientPacketsParser = 0;
    }

//    ClientResourcesManager::cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

//    delete m_udpServer;
//    delete m_udtProtocol;

    PacketHandlerBase::clean();


    delete databaseUtility;
    databaseUtility = 0;

    delete settings;
    settings = 0;


    if(lookForServerTimer){
        lookForServerTimer->stop();
        delete lookForServerTimer;
        lookForServerTimer = 0;
    }

    mainServiceStarted = false;

    if(m_myInfo){
        delete m_myInfo;
        m_myInfo = 0;
    }

    if(systemInfo){
        systemInfo->stopGetingRealTimeResourcesLoad();
        delete systemInfo;
        systemInfo = 0;
    }


}

bool ClientService::setDeskWallpaper(const QString &wallpaperPath){

#if defined(Q_OS_WIN32)

    QString path = wallpaperPath;
    if(path.trimmed().isEmpty() || !QFileInfo(path).exists()){
        path = ":/resources/images/wallpaper.png";
    }

    if(!WinUtilities::setDeskWallpaper(path)){
        return false;
    }

#endif

    return true;

}


bool ClientService::startMainService(){
    qDebug()<<"----ClientService::startMainService()";

    if(mainServiceStarted){
        qWarning()<<"Main service has already started!";
        return true;
    }


    QString errorMessage = "";
//    m_udpServer = resourcesManager->startIPMCServer(QHostAddress(IP_MULTICAST_GROUP_ADDRESS), quint16(IP_MULTICAST_GROUP_PORT), &errorMessage);
//    if(!m_udpServer){
//        logMessage(QString("Can not start IP Multicast listening on address '%1', port %2! %3").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
//        m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
//    }else{
//        qWarning()<<QString("IP Multicast listening on address '%1', port %2!").arg(IP_MULTICAST_GROUP_ADDRESS).arg(IP_MULTICAST_GROUP_PORT);
//    }

    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, quint16(IP_MULTICAST_GROUP_PORT), true, &errorMessage);
    if(!m_udpServer){
        logMessage(QString("Can not start UDP listening on port %1! %2").arg(IP_MULTICAST_GROUP_PORT).arg(errorMessage), QtServiceBase::Error);
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(IP_MULTICAST_GROUP_PORT);
    }

    m_rtp = resourcesManager->startRTP(QHostAddress::Any, RTP_LISTENING_PORT, true, &errorMessage);
    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)));

//    m_udtProtocol = m_rtp->getUDTProtocol();
//    if(!m_udtProtocol){
//        QString error = tr("Can not start UDT listening on port %1! %2").arg(UDT_LISTENING_PORT).arg(errorMessage);
//        logMessage(error, QtServiceBase::Error);
//        return false;
//    }else{
//        qWarning()<<QString("UDT listening on address port %1!").arg(UDT_LISTENING_PORT);
//    }
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
//    m_udtProtocol->startWaitingForIOInOneThread(10);
    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);




//    m_udtProtocolForFileTransmission = resourcesManager->getUDTProtocolForFileTransmission();

    clientPacketsParser = new ClientPacketsParser(m_localAssetNO, resourcesManager, this);
    //connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);
    //connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), clientPacketsParser, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, quint16, const QString&, const QString&, int)), this, SLOT(serverFound(const QString& ,quint16, quint16, const QString&, const QString&, int)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalUpdateClientSoftwarePacketReceived()), this, SLOT(update()), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalSetupUSBSDPacketReceived(quint8, bool, const QString & )), this, SLOT(processSetupUSBSDPacket(quint8, bool, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalShowAdminPacketReceived(bool)), this, SLOT(processShowAdminPacket(bool)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalModifyAssetNOPacketReceived(const QString &, const QString &)), this, SLOT(processModifyAssetNOPacket(const QString &, const QString &)));
    connect(clientPacketsParser, SIGNAL(signalAssetNOModifiedPacketReceived(const QString &, const QString &, bool, const QString &)), this, SLOT(processAssetNOModifiedPacket(const QString &, const QString &, bool, const QString &)));

    connect(clientPacketsParser, SIGNAL(signalRenameComputerPacketReceived(const QString &, const QString &, const QString &, const QString &)), this, SLOT(processRenameComputerPacketReceived(const QString &, const QString &, const QString &, const QString &)));
    connect(clientPacketsParser, SIGNAL(signalJoinOrUnjoinDomainPacketReceived(const QString &, bool, const QString &, const QString &, const QString &)), this, SLOT(processJoinOrUnjoinDomainPacketReceived(const QString &, bool, const QString &, const QString &, const QString &)));
    connect(clientPacketsParser, SIGNAL(signalAdminRequestConnectionToClientPacketReceived(SOCKETID, const QString &, const QString &)), this, SLOT(processAdminRequestConnectionToClientPacket(SOCKETID, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminSearchClientPacketReceived(const QString &, quint16 , const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)), this, SLOT(processAdminSearchClientPacket(const QString &, quint16 , const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    
//    connect(clientPacketsParser, SIGNAL(signalAdminRequestRemoteAssistancePacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestRemoteAssistancePacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestUpdateMSUserPasswordPacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestUpdateMSUserPasswordPacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestInformUserNewPasswordPacketReceived(const QString &, const QString &, const QString &, quint16 )), this, SLOT(processAdminRequestInformUserNewPasswordPacket(const QString &, const QString &, const QString &, quint16 )), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalClientInfoRequestedPacketReceived(SOCKETID, const QString &, quint8)), this, SLOT(processClientInfoRequestedPacket(SOCKETID, const QString &, quint8)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalAdminRequestRemoteConsolePacketReceived(const QString &, const QString &, const QString &, bool, const QString &, quint16)), this, SLOT(processAdminRequestRemoteConsolePacket(const QString &, const QString &, const QString &, bool, const QString &, quint16)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalRemoteConsoleCMDFromServerPacketReceived(const QString &, const QString &, const QString &, quint16)), this, SLOT(processRemoteConsoleCMDFromServerPacket(const QString &, const QString &, const QString &, quint16)), Qt::QueuedConnection);
    
//    connect(clientPacketsParser, SIGNAL(signalServerAnnouncementPacketReceived(const QString &,const QString &, quint32, const QString &,const QString &,const QString &,bool)), this, SLOT(processServerAnnouncementPacket(const QString &, const QString &, quint32, const QString &, const QString &, const QString &,bool)), Qt::QueuedConnection);

//    connect(clientPacketsParser, SIGNAL(signalLocalUserOnlineStatusChanged(SOCKETID, const QString &, bool)), this, SLOT(processLocalUserOnlineStatusChanged(SOCKETID, const QString &, bool)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalAdminRequestTemperatures(SOCKETID, bool, bool)), this, SLOT(processAdminRequestTemperaturesPacket(SOCKETID, bool, bool)), Qt::QueuedConnection);
//    connect(clientPacketsParser, SIGNAL(signalAdminRequestScreenshot(SOCKETID, const QString &, bool)), this, SLOT(processAdminRequestScreenshotPacket(SOCKETID, const QString &, bool)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalAdminRequestShutdownPacketReceived(SOCKETID,const QString&,quint32,bool,bool)), this, SLOT(processAdminRequestShutdownPacket(SOCKETID,const QString&,quint32,bool,bool)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestLockWindowsPacketReceived(SOCKETID,const QString&,bool)), this, SLOT(processAdminRequestLockWindowsPacket(SOCKETID,const QString&,bool)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestCreateOrModifyWinUserPacketReceived(SOCKETID,const QByteArray&)), this, SLOT(processAdminRequestCreateOrModifyWinUserPacket(SOCKETID,const QByteArray&)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestDeleteUserPacketReceived(SOCKETID,const QString&)), this, SLOT(processAdminRequestDeleteUserPacket(SOCKETID,const QString&)), Qt::QueuedConnection);


    connect(clientPacketsParser, SIGNAL(signalAdminRequestChangeServiceConfigPacketReceived(SOCKETID,QString,bool,ulong)), this, SLOT(processAdminRequestChangeServiceConfigPacket(SOCKETID,QString,bool,ulong)), Qt::QueuedConnection);

    connect(clientPacketsParser, SIGNAL(signalRequestChangeProcessMonitorInfoPacketReceived(SOCKETID, const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool)), this, SLOT(processRequestChangeProcessMonitorInfoPacket(SOCKETID, const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool)), Qt::QueuedConnection);



    ////////////////////
    connect(clientPacketsParser, SIGNAL(signalFileSystemInfoRequested(SOCKETID, const QString &)), this, SLOT(fileSystemInfoRequested(SOCKETID, const QString &)));
    //File TX
    connect(clientPacketsParser, SIGNAL(signalAdminRequestUploadFile(SOCKETID, const QByteArray &, const QString &, quint64, const QString &)), this, SLOT(processAdminRequestUploadFilePacket(SOCKETID, const QByteArray &, const QString &,quint64, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalAdminRequestDownloadFile(SOCKETID, const QString &, const QString &, const QString &)), this, SLOT(processAdminRequestDownloadFilePacket(SOCKETID, const QString &, const QString &, const QString &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileDataRequested(SOCKETID, const QByteArray &, int, int )), this, SLOT(processFileDataRequestPacket(SOCKETID,const QByteArray &, int, int )), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileDataReceived(SOCKETID, const QByteArray &, int, const QByteArray &, const QByteArray &)), this, SLOT(processFileDataReceivedPacket(SOCKETID, const QByteArray &, int, const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileTXStatusChanged(SOCKETID, const QByteArray &,quint8)), this, SLOT(processFileTXStatusChangedPacket(SOCKETID, const QByteArray &, quint8)), Qt::QueuedConnection);
    connect(clientPacketsParser, SIGNAL(signalFileTXError(SOCKETID , const QByteArray &, quint8 , const QString &)), this, SLOT(processFileTXErrorFromPeer(SOCKETID , const QByteArray &, quint8 , const QString &)), Qt::QueuedConnection);





    //Single Process Thread
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::run);
    
    //IMPORTANT For Multi-thread
    //QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startparseIncomingPackets);
    //QtConcurrent::run(clientPacketsParser, &ClientPacketsParser::startprocessOutgoingPackets);


    //TODO:
    QString ip = "";
    quint16 port = 0;
    getServerLastUsed(&ip, &port);
    clientPacketsParser->sendClientLookForServerPacket(ip, port);
    lookForServerTimer = new QTimer(this);
    lookForServerTimer->setSingleShot(true);
    connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
    lookForServerTimer->start(120000);


    mainServiceStarted = true;


//    updateAdministratorPassword(WIN_ADMIN_PASSWORD);

    //qWarning()<<"Check User Account!";
    //checkUsersAccount();

    //qWarning()<<"Check USB SD!";
    //checkUSBSD();

    //qWarning()<<"Check Programes!";
    //checkProgrames();

    //setupStartupWithSafeMode(true);

    QString section = serviceName() + "/LastCheckUpdate";
    QSettings settings(QCoreApplication::applicationDirPath()+"/.settings", QSettings::IniFormat, this);
    QDateTime time = settings.value(section, QDateTime()).toDateTime();
    if(time.isNull() || (time.addDays(1) < QDateTime::currentDateTime())){

#if defined(Q_OS_WIN32)
//        wm->modifySystemSettings();

//        QSettings computerNameSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\HEHUI", QSettings::NativeFormat, this);
//        QString storedComputerName = computerNameSettings.value("ComputerName", "").toString().trimmed();
//        if(!storedComputerName.isEmpty() && (storedComputerName.toLower() != m_localComputerName) ){
//            wm->setComputerName(storedComputerName);
//        }

#endif

        qWarning()<<"Update!";
        //update();
        settings.setValue(section, QDateTime::currentDateTime());
    }
    //update();

    //    if(time.isNull() || (time.addDays(30) < QDateTime::currentDateTime())){
    //        processClientDetailedInfoRequestedPacket("", true, "", 0);
    //    }




#if defined(Q_OS_WIN32)

//    section = serviceName() + "/LastCleanTemporaryFiles";
//    time = settings.value(section, QDateTime()).toDateTime();
//    if(time.isNull() || (time.addDays(7) < QDateTime::currentDateTime())){
//        wm->cleanTemporaryFiles();
//        settings.setValue(section, QDateTime::currentDateTime());
//    }

    qWarning()<<"Clean Temporary Files!";

//    m_wm->freeMemory();
#endif
    
    qWarning();
    qWarning()<<"Main Service Started!";
    qWarning();

    return true;

}

void ClientService::serverLookedUp(const QHostInfo &host){
    qDebug()<<"--ClientService::serverLookedUp(...)";

    if (host.error() != QHostInfo::NoError) {
        qDebug() << "Server Lookup failed:" << host.errorString();
        return;
    }

    if(host.addresses().isEmpty()){return;}
    //     foreach (const QHostAddress &address, host.addresses()){
    //         qDebug() << "Found default server's address:" << address.toString();
    //     }


    QString address = host.addresses().first().toString();
    qDebug() << "Found default server's address:" << address;
    if(!m_rtp->isSocketConnected(m_socketConnectedToServer)){
        clientPacketsParser->sendClientLookForServerPacket(address);
    }

}

void ClientService::serverFound(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID){
    qDebug()<<"----ClientService::serverFound(...) serverInstanceID:"<<serverInstanceID <<" m_serverInstanceID:"<<m_serverInstanceID;

    //QMutexLocker locker(&mutex);

    if(/*!m_serverAddress.isNull() && */serverInstanceID == m_serverInstanceID && m_rtp->isSocketConnected(m_socketConnectedToServer)){
        qWarning()<<"Already Connected To Server "<<serverAddress;
        return;
    }
    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        m_rtp->closeSocket(m_socketConnectedToServer);
        m_socketConnectedToServer = INVALID_SOCK_ID;
    }
    m_serverAddress = QHostAddress::Null;
    m_serverUDTListeningPort = 0;
    m_serverName = "";
    m_serverInstanceID = 0;

    int msec = QDateTime::currentDateTime().toString("zzz").toUInt();
    msec = msec<100?(msec*1000):(msec*10);
    Utilities::msleep(msec);

    QString errorMessage;
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        m_socketConnectedToServer = m_rtp->connectToHost(QHostAddress(serverAddress), serverRTPListeningPort, 10000, &errorMessage);
    }
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        qCritical()<<tr("Can not connect to host! %1").arg(errorMessage);
        return;
    }

    if(!clientPacketsParser->sendClientOnlineStatusChangedPacket(m_socketConnectedToServer, true)){
        QString err = m_rtp->lastErrorString();
        m_rtp->closeSocket(m_socketConnectedToServer);
        m_socketConnectedToServer = INVALID_SOCK_ID;
        qCritical()<<"Error! Can not changed online status to server! "<<err;
        return;
    }

    lookForServerTimer->stop();

    m_serverAddress = QHostAddress(serverAddress);
    m_serverUDTListeningPort = serverRTPListeningPort;
    m_serverName = serverName;
    m_serverInstanceID = serverInstanceID;

    setServerLastUsed(serverAddress);

    //logMessage(QString("Server Found! Address:%1 TCP Port:%2 Name:%3").arg(serverAddress).arg(serverTCPListeningPort).arg(serverName), QtServiceBase::Information);
    qWarning();
    qWarning()<<"Server Found!"<<" Address:"<<serverAddress<<" UDT Port:"<<serverRTPListeningPort<<" TCP Port:"<<serverTCPListeningPort<<" Name:"<<serverName<<" Instance ID:"<<serverInstanceID << " Socket ID:"<<m_socketConnectedToServer;
    qWarning();



#ifdef Q_OS_WIN
    if(Utilities::versionCompare(version, QString(APP_VERSION)) == 1){       
        QTimer::singleShot(60*msec, this, SLOT(update()));
        //update();
    }

    if(logs.size()){
        bool ok = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_CheckMSUsersAccount), logs.join(" | "));
        if(ok){
            logs.clear();
        }
    }

#endif

    //clientPacketsParser->sendClientOnlinePacket(networkManager->localTCPListeningAddress(), networkManager->localTCPListeningPort(), QHostInfo::localHostName(), false);
    //QTimer::singleShot(60*msec, this, SLOT(uploadClientSummaryInfo()));
    //uploadClientInfo();
    //uploadClientSummaryInfo(m_socketConnectedToServer);

    processClientInfoRequestedPacket(m_socketConnectedToServer, "", MS::SYSINFO_OS);
    processClientInfoRequestedPacket(m_socketConnectedToServer, "", MS::SYSINFO_HARDWARE);

    uploadSoftwareInfo();

}

void ClientService::processClientInfoRequestedPacket(SOCKETID socketID, const QString &assetNO, quint8 infoType){


#ifdef Q_OS_WIN

//    if(!computerName.isEmpty()){
//        if(computerName != m_localComputerName){
//            return;
//        }
//    }

    peerSocketThatRequiresDetailedInfo = socketID;

//    if(SystemInfo::isRunning()){
//        return;
//    }


    if(!systemInfo){
        systemInfo = new SystemInfo(this);
        connect(systemInfo, SIGNAL(signalSystemInfoResultReady(const QByteArray&, quint8, SOCKETID)), this, SLOT(systemInfoResultReady(const QByteArray&, quint8, SOCKETID)), Qt::QueuedConnection);
//        connect(systemInfo, SIGNAL(finished()), this, SLOT(systemInfoThreadFinished()));
    }

    //QtConcurrent::run(QThreadPool::globalInstance(), &SystemInfo::getInstalledSoftwareInfo, socketID);


    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
        //systemInfoResultReady(systemInfo->getOSInfo(), quint8(MS::SYSINFO_OS), socketID);
        systemInfoResultReady(m_myInfo->getOSJsonData(), quint8(MS::SYSINFO_OS), socketID);
        break;

    case quint8(MS::SYSINFO_HARDWARE):
        systemInfo->getHardwareInfo(socketID);
        break;

    case quint8(MS::SYSINFO_SOFTWARE):
        systemInfo->getInstalledSoftwaresInfo(socketID);
        break;

    case quint8(MS::SYSINFO_SERVICES):
        systemInfo->getServicesInfo(socketID);
        break;

    case quint8(MS::SYSINFO_OSUSERS):
        systemInfo->getUsersInfo(socketID);
        break;

    case quint8(MS::SYSINFO_REALTIME_INFO):
        systemInfo->stopGetingRealTimeResourcesLoad();
        systemInfo->startGetingRealTimeResourcesLoad(socketID);
        break;

    default:
        qCritical()<<"ERROR! Unknown info type:"<<infoType;
        break;
    }



#endif

}

void ClientService::systemInfoResultReady(const QByteArray &data, quint8 infoType, SOCKETID socketID){

    qDebug()<<"ClientService::systemInfoResultReady(...)"<<" size:"<<data.size();
    //qDebug()<<"system info:"<<data;

#if defined(Q_OS_WIN32)

   if(INVALID_SOCK_ID == socketID){
       m_myInfo->setJsonData(data);
       return;
   }

    if(data.isEmpty()){
        QString message = "Error! Can not get system info!";
        clientPacketsParser->sendClientMessagePacket(socketID, message, MS::MSG_Critical);
        return;
    }

    //TODO:
    bool ret = clientPacketsParser->sendClientInfoPacket(socketID, data, infoType);
    if(!ret){
        qCritical()<<tr("ERROR! Can not upload system info to peer! %3").arg(m_rtp->lastErrorString());
    }

    if( (m_socketConnectedToServer != INVALID_SOCK_ID)
            && (socketID != m_socketConnectedToServer)
            && infoType != quint8(MS::SYSINFO_REALTIME_INFO)
            ){
        bool ret = clientPacketsParser->sendClientInfoPacket(m_socketConnectedToServer, data, infoType);
        if(!ret){
            qCritical()<<tr("ERROR! Can not upload system info to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }


//    m_wm->freeMemory();


#endif



}


void ClientService::systemInfoThreadFinished(){
    Q_ASSERT(systemInfo);
//    systemInfo->exit();
    delete systemInfo;
    systemInfo = 0;
}

void ClientService::processSetupUSBSDPacket(quint8 usbSTORStatus, bool temporarilyAllowed, const QString &adminName){

#ifdef Q_OS_WIN

    QString str = "Unknown";
    bool ok = false;
    if(usbSTORStatus == quint8(MS::USBSTOR_Disabled)){
        ok = setupUSBStorageDevice(false, false, temporarilyAllowed);
        str = "Disabled";
    }else if(usbSTORStatus == quint8(MS::USBSTOR_ReadOnly)){
        ok = setupUSBStorageDevice(true, false, temporarilyAllowed);
        str = "Read-Only";
    }else{
        ok = setupUSBStorageDevice(true, true, temporarilyAllowed);
        str = "Read-Write";
    }

//    uploadClientSummaryInfo(m_socketConnectedToServer);
//    uploadClientSummaryInfo(m_socketConnectedToAdmin);

//    if(temporarilyAllowed){
//        str = "Enabled(Provisional Licence)";
//    }else{
//        str = "Enabled(Perpetual License)";
//    }

    QString log = QString("USB Storage Device Settings Changed To:%1! Admin:%2").arg(str).arg(adminName);
    if(!ok){
        log = QString("Failed to change USB Storage Device settings to:%1!").arg(str);
    }

    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        bool ok = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_AdminSetupUSBSD), log);
        if(!ok){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }

    bool sent = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log);
    //sent = clientPacketsParser->sendClientResponseUSBInfoPacket(m_socketConnectedToAdmin, ok, log);
    if(!sent){
        qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_rtp->lastErrorString());
    }


#endif


}

void ClientService::processShowAdminPacket(bool show){

#ifdef Q_OS_WIN
    WinUtilities::showAdministratorAccountInLogonUI(show);
#endif

}

void ClientService::processModifyAssetNOPacket(const QString &newAssetNO, const QString &adminName){

    //TODO:

    if(INVALID_SOCK_ID == m_socketConnectedToServer){
        QString message = tr("No Server Connected!");
        clientPacketsParser->sendClientResponseModifyAssetNOPacket(m_socketConnectedToAdmin, m_localAssetNO, false, message);
        return;
    }

    bool sent = clientPacketsParser->sendModifyAssetNOPacket(m_socketConnectedToServer, newAssetNO, adminName);
    if(!sent){
        QString message = tr("Can not send data to server!");
        clientPacketsParser->sendClientResponseModifyAssetNOPacket(m_socketConnectedToAdmin, m_localAssetNO, false, message);
        return;
    }

    setLocalAssetNO(newAssetNO, true);
    QTimer::singleShot(5000, this, SLOT(modifyAssetNOTimeout()));

}

void ClientService::processAssetNOModifiedPacket(const QString &newAssetNO, const QString &oldAssetNO, bool modified, const QString &message){

    if(!modified){
        clientPacketsParser->sendClientResponseModifyAssetNOPacket(m_socketConnectedToAdmin, oldAssetNO, false, message);
        return;
    }

    setLocalAssetNO(newAssetNO);
    m_localAssetNO = newAssetNO;
    clientPacketsParser->setAssetNO(newAssetNO);
    clientPacketsParser->sendClientResponseModifyAssetNOPacket(m_socketConnectedToAdmin, oldAssetNO, true, "");

    QString log = QString("Computer asset NO. modified from '%1' to '%2'. Computr name:%3").arg(oldAssetNO).arg(newAssetNO).arg(m_localComputerName);
    logMessage(log);

}

void ClientService::modifyAssetNOTimeout(){
    QString newAssetNOToBeUsed = "";
    getLocalAssetNO(&newAssetNOToBeUsed);
    if(newAssetNOToBeUsed.isEmpty()){
        return;
    }
    if(newAssetNOToBeUsed != m_localAssetNO){
        QString message = tr("Failed to modify asset number! Waitting for server response timed out!");
        clientPacketsParser->sendClientResponseModifyAssetNOPacket(m_socketConnectedToAdmin, m_localAssetNO, false, message);
        return;
    }
}

void ClientService::processRenameComputerPacketReceived(const QString &newComputerName, const QString &adminName, const QString &domainAdminName, const QString &domainAdminPassword){

#ifdef Q_OS_WIN32

    bool ok = false;
    unsigned long errorCode = 0;
    if(m_isJoinedToDomain){
        ok = WinUtilities::renameMachineInDomain(newComputerName, domainAdminName, domainAdminPassword, "", &errorCode);
    }else{
        ok = WinUtilities::setComputerName(newComputerName, &errorCode);
    }

    //uploadClientSummaryInfo(m_socketConnectedToServer);
    //uploadClientSummaryInfo(m_socketConnectedToAdmin);
    processClientInfoRequestedPacket(m_socketConnectedToServer, m_localAssetNO, MS::SYSINFO_OS);
    processClientInfoRequestedPacket(m_socketConnectedToAdmin, m_localAssetNO, MS::SYSINFO_OS);


    QString log;
    if(ok){
        //QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\HEHUI", QSettings::NativeFormat, this);
        //settings.setValue("ComputerName", newComputerName);
        log = QString("The computer is renamed from '%1' to '%2'. Restart the computer to apply all changes! Admin:%3.").arg(m_localComputerName).arg(newComputerName).arg(adminName);
    }else{
        log = QString("Failed to rename computer! Admin:%1. Error Code:%2").arg(adminName).arg(errorCode);
    }

    bool sent = false;
    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_AdminSetupOSAdministrators), log);
        if(!sent){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }

    if(m_rtp->isSocketConnected(m_socketConnectedToAdmin)){
        sent = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log, ok?quint8(MS::MSG_Information):quint8(MS::MSG_Critical));
        if(!sent){
            qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_rtp->lastErrorString());
        }
    }


#else
    qWarning()<<"This function works on M$ Windows only!";
#endif

}

void ClientService::processJoinOrUnjoinDomainPacketReceived(const QString &adminName, bool isDomain, const QString &domainOrWorkgroupName, const QString &domainAdminName, const QString &domainAdminPassword){

#ifdef Q_OS_WIN32

    bool ok = false;

    unsigned long errorCode = 0;
    if(m_isJoinedToDomain){
        ok = WinUtilities::unjoinDomain(QString(domainAdminName)+"@"+m_joinInfo, QString(domainAdminPassword), "", &errorCode);
        if(ok){
            ok = WinUtilities::joinWorkgroup(domainOrWorkgroupName, &errorCode);
        }
    }else{
        if(isDomain){
            ok = WinUtilities::joinDomain(domainOrWorkgroupName, QString(domainAdminName)+"@"+domainOrWorkgroupName, QString(domainAdminPassword), "", &errorCode);
        }else{
            ok = WinUtilities::joinWorkgroup(domainOrWorkgroupName, &errorCode);
        }
    }


    QString log;
    if(ok){
        m_isJoinedToDomain = isDomain;
        m_joinInfo = domainOrWorkgroupName;
        log = QString("Computer '%1' is successfully joined to %2 '%3'! Restart the computer to apply all changes! Admin:%4.").arg(m_localComputerName).arg(isDomain?"domain":"workgroup").arg(domainOrWorkgroupName).arg(adminName);
    }else{
        log = QString("Failed to join computer '%1' to %2 '%3'! Admin:%4. Error Code:%5").arg(m_localComputerName).arg(isDomain?"domain":"workgroup").arg(domainOrWorkgroupName).arg(adminName).arg(errorCode);
    }

    bool sent = false;
    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_AdminSetupOSAdministrators), log);
        if(!sent){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }

    if(m_rtp->isSocketConnected(m_socketConnectedToAdmin)){
        sent = clientPacketsParser->sendClientMessagePacket(m_socketConnectedToAdmin, log, ok?quint8(MS::MSG_Information):quint8(MS::MSG_Critical));
        if(!sent){
            qCritical()<<tr("ERROR! Can not send message to admin from %1:%2! %3").arg(m_adminAddress).arg(m_adminPort).arg(m_rtp->lastErrorString());
        }
    }

    //uploadClientSummaryInfo(m_socketConnectedToServer);
    //uploadClientSummaryInfo(m_socketConnectedToAdmin);
    processClientInfoRequestedPacket(m_socketConnectedToServer, m_localAssetNO, MS::SYSINFO_OS);
    processClientInfoRequestedPacket(m_socketConnectedToAdmin, m_localAssetNO, MS::SYSINFO_OS);


#else
    qWarning()<<"This function works on M$ Windows only!";
#endif


}

void ClientService::processAdminRequestConnectionToClientPacket(SOCKETID adminSocketID, const QString &adminComputerName, const QString &adminName){
    qDebug()<<"--ClientService::processAdminRequestConnectionToClientPacket(...)";


    int previousSocketConnectedToAdmin = m_socketConnectedToAdmin;

#ifdef Q_OS_WIN
    bool result =false;
    QString message = "";

//    if(m_localComputerName == computerName ){
        message = "";
        result = true;
//    }else{
//        message = QString("The computer names do not match!<br>Expected Value:%1").arg(m_localComputerName);
//        result = false;
//        QString address;
//        m_rtp->getAddressInfoFromSocket(adminSocketID, &address, 0);
//        if(NetworkUtilities::isLocalAddress(address)){
//            result = true;
//        }
//    }

//    QString usersInfo = usersOnLocalComputer().join(",");
//    if( !users.trimmed().isEmpty() && (!usersInfo.contains(users, Qt::CaseInsensitive)) ){
//        message = QString("The users information does not match!<br>Expected Value:%1").arg(usersInfo);
//        result = false;
//    }


    if(!result){
        uploadClientOSInfo(m_socketConnectedToServer);
    }

    bool ok = clientPacketsParser->sendClientResponseAdminConnectionResultPacket(adminSocketID, m_localComputerName, result, message);
    if(!ok){
        qCritical()<<"Error! Failed to response admin connection request! "<<m_rtp->lastErrorString();
    }
    //    qWarning()<<"ClientService::processAdminRequestVerifyClientInfoPacket:"<<adminAddress<< " "<<adminPort;

    if(ok && result){
        m_socketConnectedToAdmin = adminSocketID;
        clientPacketsParser->setSocketConnectedToAdmin(m_socketConnectedToAdmin, adminName);

        m_rtp->getAddressInfoFromSocket(m_socketConnectedToAdmin, &m_adminAddress, &m_adminPort);
        qWarning()<<QString("Admin %1 connected form %2:%3 via %4!").arg(adminName+"@"+adminComputerName).arg(m_adminAddress).arg(m_adminPort).arg(m_rtp->socketProtocolString(m_socketConnectedToAdmin));

        //uploadClientSummaryInfo(m_socketConnectedToAdmin);
        processClientInfoRequestedPacket(m_socketConnectedToAdmin, m_localAssetNO, MS::SYSINFO_OS);

        processClientInfoRequestedPacket(m_socketConnectedToAdmin, m_localAssetNO, MS::SYSINFO_REALTIME_INFO);

    }else{
        m_rtp->closeSocket(adminSocketID);
        clientPacketsParser->setSocketConnectedToAdmin(INVALID_SOCK_ID, "");
        return;
    }

#else
    clientPacketsParser->sendClientResponseAdminConnectionResultPacket(adminSocketID, true, "It's NOT M$ Windows!");
    clientPacketsParser->sendClientResponseClientSummaryInfoPacket(adminSocketID, QByteArray());

    //m_udtProtocol->closeSocket(adminSocketID);
#endif

    if( (previousSocketConnectedToAdmin != INVALID_SOCK_ID) && (previousSocketConnectedToAdmin != m_socketConnectedToAdmin) ){
        clientPacketsParser->sendClientMessagePacket(previousSocketConnectedToAdmin, QString("Another administrator has logged on from %1!").arg(m_adminAddress), quint8(MS::MSG_Critical));
        clientPacketsParser->sendClientOnlineStatusChangedPacket(previousSocketConnectedToAdmin, false);
        m_rtp->closeSocket(previousSocketConnectedToAdmin);
//        closeFileTXWithAdmin();
    }


}

void ClientService::processAdminSearchClientPacket(const QString &adminAddress, quint16 adminPort, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

#ifdef Q_OS_WIN

    if(!computerName.trimmed().isEmpty()){
        if(!m_localComputerName.contains(computerName, Qt::CaseInsensitive)){
            return;
        }
    }
    
    if(!userName.trimmed().isEmpty()){
        QString usersInfo = usersOnLocalComputer().join(",");
        if(!usersInfo.contains(userName, Qt::CaseInsensitive)){
            return;
        }
    }
    
    if(!workgroup.trimmed().isEmpty()){
        if(!m_joinInfo.contains(workgroup, Qt::CaseInsensitive)){
            return;
        }
    }
    

    QList<QHostAddress> ips = NetworkUtilities::validIPAddresses();
    QString networkInfo = "";
    QStringList networkInfoList;
    foreach(QHostAddress ip, ips){
        //networkInfo += ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip) + ",";
        networkInfoList.append(ip.toString() + "/" + NetworkUtilities::hardwareAddress(ip));
    }
    networkInfo = networkInfoList.join(",");
    
    if(!macAddress.trimmed().isEmpty()){
        if(!networkInfo.contains(macAddress, Qt::CaseInsensitive)){
            return;
        }
    }
    if(!ipAddress.trimmed().isEmpty()){
        if(!networkInfo.contains(ipAddress, Qt::CaseInsensitive)){
            return;
        }
    }
    
    if(!osVersion.trimmed().isEmpty()){       
        if(!m_myInfo->getOSVersion().contains(osVersion, Qt::CaseInsensitive)){
            return;
        }
    }

    uploadClientOSInfo(adminAddress, adminPort);

//    m_socketConnectedToAdmin = m_udtProtocol->connectToHost(QHostAddress(adminAddress), adminPort);
//    if(m_socketConnectedToServer == INVALID_SOCK_ID){
//        qCritical()<<tr("ERROR! Can not connect to Admin %1:%2! %3").arg(adminAddress).arg(adminPort).arg(m_rtp->lastErrorString());
//        return;
//    }
//    if(m_udtProtocol->isSocketConnected(m_socketConnectedToAdmin)){
//        uploadClientSummaryInfo(m_socketConnectedToAdmin);

//    }else{
//        //TODO:wait

//    }







#endif

}

//void ClientService::processServerAnnouncementPacket(const QString &workgroupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, const QString &userName, bool mustRead){
//    //qDebug()<<"--ClientService::processServerAnnouncementPacket(..) "<<" workgroupName:"<<workgroupName<<" computerName:"<<computerName<<" announcement:"<<announcement;

//#ifdef Q_OS_WIN

//    if(!workgroupName.isEmpty()){
//        if(workgroupName.toLower() != m_joinInfo){
//            return;
//        }
//    }
    
//    if(!computerName.trimmed().isEmpty()){
//        if(!m_localComputerName.contains(computerName, Qt::CaseInsensitive)){
//            return;
//        }
//    }
//    clientPacketsParser->sendAnnouncement(adminName, announcementID, announcement);
    
//#endif
    
    
//}


//void ClientService::processAdminRequestRemoteAssistancePacket(const QString &computerName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

//    //    return;
    
//#ifdef Q_OS_WIN

//    if(!computerName.isEmpty()){
//        if(computerName != m_localComputerName){
//            return;
//        }
//    }
    
    
//    clientPacketsParser->requestRemoteAssistance(adminAddress, adminPort, adminName);

    
//    //    QString exeDirPath = QCoreApplication::applicationDirPath();


//    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "psexec.exe", QString("-i tvnserver.exe -controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
     
//    //        bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlapp -connect %1").arg(adminAddress), false, exeDirPath, true);
//    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);

//    //        clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
     
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
//    //    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "net.exe", QString("start tvnserver"), false, exeDirPath, true);


//    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "net.exe", QString("start tvnserver"), false, exeDirPath, true);
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
//    //    ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);
//    //    //bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlservice -connect %1").arg(adminAddress), false, exeDirPath, true);

//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");


//    //    QString exeDirPath = QCoreApplication::applicationDirPath();
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
//    //    bool ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-run"), false, exeDirPath, true);
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
//    //    ok = wm->runAs("administrator", getWinAdminPassword(), "tvnserver.exe", QString("-controlapp -connect %1").arg(adminAddress), false, exeDirPath, true);
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, ok?"OK":"Failed");
    
    
    
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Trying to start VNC service...");
//    //    QProcess process;
//    //    QString cmd = QString("net.exe start tvnserver");
//    //    process.start(cmd);
//    //    process.waitForStarted(10000);
//    //    //process.terminate();
    
//    //    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(QHostAddress(adminAddress), adminPort, localComputerName, "Connecting to VNC client...");
//    //    cmd = QString("tvnserver.exe -controlservice -connect %1").arg(adminAddress);
//    //    //QString cmd = QString("tvnserver.exe -controlservice -disconnectall");
//    //    QProcess process2;
//    //    process2.start(cmd);
//    //    process2.waitForStarted(10000);
//    //    //process2.terminate();

//    QString log = QString("Remote Assistance! Admin:%1").arg(adminName);
//    if(INVALID_SOCK_ID != m_socketConnectedToServer){
//        bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_AdminRequestRemoteAssistance), log);
//        if(!sent){
//            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
//        }
//    }

//#endif
    
    
//}

void ClientService::processAdminRequestUpdateMSUserPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

#ifdef Q_OS_WIN

    if(!workgroupName.isEmpty()){
        if(workgroupName.toLower() != m_joinInfo.toLower()){
            return;
        }
    }

    QString log = QString("Update Password! Admin:%1").arg(adminName);
    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_AdminInformUserNewPassword), log);
        if(!sent){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }
#endif
    
    
}

void ClientService::processAdminRequestInformUserNewPasswordPacket(const QString &workgroupName, const QString &adminName, const QString &adminAddress, quint16 adminPort){

    qWarning()<<"--processAdminRequestInformUserNewPasswordPacket(...)";
    

}



void ClientService::processAdminRequestRemoteConsolePacket(const QString &assetNO, const QString &applicationPath, const QString &adminID, bool startProcess, const QString &adminAddress, quint16 adminPort){

    if(assetNO != m_localAssetNO){
        return;
    }
    if(!m_adminAddress.isEmpty() && m_adminAddress != adminAddress){
        qWarning()<<"m_adminAddress:"<<m_adminAddress;
        qWarning()<<"adminAddress:"<<adminAddress;
        //clientPacketsParser->sendClientMessagePacket(QHostAddress(adminAddress), adminPort, localComputerName, QString("Another administrator has logged on from %1!").arg(m_adminAddress));
        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, false, QString("Another administrator has logged on from %1!").arg(m_adminAddress), quint8(MS::MSG_Critical));

        return;
    }
    
    if(!startProcess){
        if(process){
            process->stopProcess();
        }
        return;
    }

    if(!process){
        process = new Process(this);
        connect(process, SIGNAL(signalProcessStateChanged(bool, const QString &)), this, SLOT(consoleProcessStateChanged(bool, const QString &)));
        connect(process, SIGNAL(signalProcessOutputRead(const QString &)), this, SLOT(consoleProcessOutputRead(const QString &)));
    }
    

    

    //    if(process->isRunning()){
    //        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(QHostAddress(adminAddress), adminPort, localComputerName, false, "The Process is already running!");
    //        return;
    //    }

    process->startProcess(applicationPath);


}

void ClientService::processRemoteConsoleCMDFromServerPacket(const QString &assetNO, const QString &command, const QString &adminAddress, quint16 adminPort){

    if(assetNO != m_localAssetNO){
        return;
    }

    if((!process) || (!process->isRunning())){
        clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, false, "The Process is not running!", qint8(MS::MSG_Critical));
        return;
    }

    process->writeDataToProcess(command);


}


void ClientService::consoleProcessStateChanged(bool running, const QString &message){
    clientPacketsParser->sendClientResponseRemoteConsoleStatusPacket(m_socketConnectedToAdmin, running, message);
}

void ClientService::consoleProcessOutputRead(const QString &output){
    clientPacketsParser->sendRemoteConsoleCMDResultFromClientPacket(m_socketConnectedToAdmin, output);
}

//void ClientService::processLocalUserOnlineStatusChanged(SOCKETID userSocketID, const QString &userName, bool online){

//    if(INVALID_SOCK_ID != m_socketConnectedToAdmin){
//        clientPacketsParser->sendLocalUserOnlineStatusChangedPacket(m_socketConnectedToAdmin, userName, online);
//    }

//}

void ClientService::processAdminRequestTemperaturesPacket(SOCKETID socketID, bool cpu, bool harddisk){

    if((!cpu) && (!harddisk)){
        if(m_hardwareMonitor){
            delete m_hardwareMonitor;
            m_hardwareMonitor = 0;
        }
        return;
    }


    if(!m_hardwareMonitor){
        m_hardwareMonitor = new HardwareMonitor(this);
    }

    QString cpuTemperature, harddiskTemperature;
    if(cpu){
        cpuTemperature = m_hardwareMonitor->getCPUTemperature();
        qDebug()<<"-------------cpuTemperature:"<<cpuTemperature;
    }
    if(harddisk){
        harddiskTemperature = m_hardwareMonitor->getHardDiskTemperature();
        qDebug()<<"-------------harddiskTemperature:"<<harddiskTemperature;

    }

    clientPacketsParser->sendClientResponseTemperaturesPacket(socketID, cpuTemperature, harddiskTemperature);

}

//void ClientService::processAdminRequestScreenshotPacket(SOCKETID socketID, const QString &userName, bool fullScreen){
//    clientPacketsParser->requestScreenshot(socketID, userName);
//}


void ClientService::processAdminRequestShutdownPacket(SOCKETID adminSocketID, const QString &message, quint32 waitTime, bool force, bool reboot){
    WinUtilities::Shutdown("", message, waitTime, force, reboot);
}

void ClientService::processAdminRequestLockWindowsPacket(SOCKETID adminSocketID, const QString &userName, bool logoff){
    if(logoff){
        WinUtilities::runAs(userName, "", "", "logoff.exe");
    }else{
        WinUtilities::runAs(userName, "", "", "rundll32.Exe", "user32.dll LockWorkStation", true);
    }
}

void ClientService::processAdminRequestCreateOrModifyWinUserPacket(SOCKETID adminSocketID, const QByteArray &userData){

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(userData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        QString message = QString("Error! Failed to create or modify user. %1").arg(error.errorString());
        clientPacketsParser->sendClientMessagePacket(adminSocketID, message, MS::MSG_Critical);
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){
        QString message = QString("Error! Failed to create or modify user. Invalid user JSON data.");
        clientPacketsParser->sendClientMessagePacket(adminSocketID, message, MS::MSG_Critical);
        qCritical()<<message;
        return;
    }

    unsigned long errorCode = 0;
    bool ok = WinUtilities::createOrModifyUser(&object, &errorCode);
    if(!ok){
        QString message = QString("Error! User creation or modification failed. ");
        clientPacketsParser->sendClientMessagePacket(adminSocketID, message, MS::MSG_Critical);
        qCritical()<<message;
    }

    systemInfo->getUsersInfo(adminSocketID);


}

void ClientService::processAdminRequestDeleteUserPacket(SOCKETID adminSocketID, const QString &userName){
    unsigned long errorCode = 0;
    bool ok = WinUtilities::deleteLocalUser(userName, &errorCode);
    if(!ok){
        QString message = QString("Error! Failed to delete user '%1'. Error code: %2").arg(userName).arg(errorCode);
        clientPacketsParser->sendClientMessagePacket(adminSocketID, message, MS::MSG_Critical);
    }

}

void ClientService::processAdminRequestChangeServiceConfigPacket(SOCKETID socketID, const QString &serviceName, bool startService, unsigned long startupType){
    unsigned long errorCode = 0;
    if(startService){
        WinUtilities::serviceStart(serviceName, &errorCode);
    }else{
        WinUtilities::serviceStop(serviceName, &errorCode);
    }
    if(errorCode != 0){
        QString message = QString("Error! Failed to %1 service '%2'. Error code: %3").arg(startService?tr("start"):tr("stop")).arg(serviceName).arg(errorCode);
        clientPacketsParser->sendClientMessagePacket(socketID, message, MS::MSG_Critical);
    }

    errorCode = 0;
    if(startupType != 0xFFFFFFFF){
        WinUtilities::serviceChangeStartType(serviceName, startupType, &errorCode);
    }
    if(errorCode != 0){
        QString message = QString("Error! Failed to change service startup type. Error code: %1").arg(errorCode);
        clientPacketsParser->sendClientMessagePacket(socketID, message, MS::MSG_Critical);
    }


    WinUtilities::ServiceInfo info;
    if(WinUtilities::serviceQueryInfo(serviceName, &info)){
        clientPacketsParser->sendClientResponseServiceConfigChangedPacket(socketID, serviceName, info.processID, info.startType);
    }

}

void ClientService::processRequestChangeProcessMonitorInfoPacket(SOCKETID socketID, const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules){

    m_procMonEnabled = enableProcMon;

    QStringList strList;
    strList.append(QString::number(enableProcMon));
    strList.append(QString::number(enablePassthrough));
    strList.append(QString::number(enableLogAllowedProcess));
    strList.append(QString::number(enableLogBlockedProcess));
    strList.append(QString::number(useGlobalRules));
    QByteArray infoArray = strList.join(";").toUtf8();
    settings->setValueWithEncryption("ProcMon/BasicInfo", infoArray, m_encryptionKey);

    settings->setValueWithEncryption("ProcMon/LocalRules", localRulesData, m_encryptionKey);
    settings->setValueWithEncryption("ProcMon/GlobalRules", globalRulesData, m_encryptionKey);


    if(m_procMonEnabled){
        if(!m_processMonitor){
            m_processMonitor = new ProcessMonitor(this);
        }

        if(useGlobalRules){
            m_processMonitor->cleanRules(true);
            m_processMonitor->setRulesData(globalRulesData);
        }
        m_processMonitor->cleanRules(false);
        m_processMonitor->setRulesData(localRulesData);

        m_processMonitor->setBasicInfo(useGlobalRules, enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess);

        if(!m_processMonitor->init()){
            qCritical()<<QString("ERROR! Failed to init ProcessMonitor. Error code: %1").arg(m_processMonitor->lastErrorCode());
            //TODO
        }
    }else{
        if(m_processMonitor){
            delete m_processMonitor;
            m_processMonitor = 0;
        }
    }

    m_myInfo->setProcessMonitorEnabled(m_procMonEnabled);



}

void ClientService::initProcessMonitorInfo(){

    bool enableProcMon = false;
    bool enablePassthrough = false;
    bool enableLogAllowedProcess = false;
    bool enableLogBlockedProcess = false ;
    bool useGlobalRules = false;

    QString basicInfo = settings->getValueWithDecryption("ProcMon/BasicInfo", m_encryptionKey, "").toString();
    if(!basicInfo.isEmpty()){
        QStringList strList = basicInfo.split(";");
        if(strList.size() == 5){
            int index = 0;
            enableProcMon = strList.at(index++).toUInt();
            enablePassthrough = strList.at(index++).toUInt();
            enableLogAllowedProcess = strList.at(index++).toUInt();
            enableLogBlockedProcess = strList.at(index++).toUInt();
            useGlobalRules = strList.at(index++).toUInt();
        }
    }

    m_procMonEnabled = enableProcMon;
    if(!m_procMonEnabled){
        return;
    }

    QByteArray localRules = settings->getValueWithDecryption("ProcMon/LocalRules", m_encryptionKey, QByteArray()).toByteArray();

    QByteArray globalRules;
    if(useGlobalRules){
        globalRules = settings->getValueWithDecryption("ProcMon/GlobalRules", m_encryptionKey, QByteArray()).toByteArray();
    }


    if(!m_processMonitor){
        m_processMonitor = new ProcessMonitor(this);
    }

    if(useGlobalRules){
        m_processMonitor->setRulesData(globalRules);
    }
    m_processMonitor->setRulesData(localRules);

     m_processMonitor->setBasicInfo(useGlobalRules, enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess);

    if(!m_processMonitor->init()){
        qCritical()<<QString("ERROR! Failed to init ProcessMonitor. Error code: %1").arg(m_processMonitor->lastErrorCode());
        //TODO
    }


}

QStringList ClientService::usersOnLocalComputer(){

#ifdef Q_OS_WIN

    QStringList users = WinUtilities::localUsers();
    users.removeAll("system$");
    users.removeAll("administrator");
    users.removeAll("guest");
    users.removeAll("helpassistant");
    users.removeAll("support_388945a0");
    users.removeAll("aspnet");
    users.removeAll("homegroupuser$");
    users.removeDuplicates();
    //users.removeAll(wm->getComputerName() + "$");

    return users;
#else
    return QStringList();
#endif

}


void ClientService::uploadClientOSInfo(SOCKETID socketID){
    qDebug()<<"--ClientService::uploadClientSummaryInfo(...) socketID:"<<socketID;

    if(INVALID_SOCK_ID == socketID){
        return;
    }

#ifdef Q_OS_WIN

    //QByteArray info = getClientSummaryInfo();

    QByteArray info = m_myInfo->getOSJsonData();
    clientPacketsParser->sendClientInfoPacket(socketID, info, MS::SYSINFO_OS);

//    info.clear();
//    info = m_myInfo->getHardwareJsonData();
//    clientPacketsParser->sendClientInfoPacket(socketID, info, MS::SYSINFO_HARDWARE);


#endif




}

void ClientService::uploadClientOSInfo(const QString &adminAddress, quint16 adminPort){
    qDebug()<<"--ClientService::uploadClientSummaryInfo(...)";


#ifdef Q_OS_WIN

    //QByteArray info = getClientSummaryInfo();
    QByteArray info = m_myInfo->getOSJsonData();
    clientPacketsParser->sendClientInfoPacket(adminAddress, adminPort, info, MS::SYSINFO_OS);

//    info.clear();
//    info = m_myInfo->getHardwareJsonData();
//    clientPacketsParser->sendClientInfoPacket(adminAddress, adminPort, info, MS::SYSINFO_HARDWARE);

#endif


}

//void ClientService::uploadClientDetailedInfoToServer(){

//}

bool ClientService::updateAdministratorPassword(const QString &newPassword){

#ifdef Q_OS_WIN

    QString administratorPassword = newPassword.trimmed();

    if(administratorPassword.isEmpty()){
        administratorPassword = getWinAdminPassword();
    }else{
        setWinAdminPassword(administratorPassword);
    }

//    QStringList users = usersOnLocalComputer();
//    QString usersInfo = users.join(",");

    unsigned long errorCode = 0;
    if(!WinUtilities::updateUserPassword("administrator", administratorPassword, &errorCode, true)){
        QString error = WinUtilities::WinSysErrorMsg(errorCode);
        if(INVALID_SOCK_ID != m_socketConnectedToServer){
            bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_UpdateMSUserPassword), error);
            if(!sent){
                qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
            }
        }
        return false;
    }


    if(INVALID_SOCK_ID != m_socketConnectedToServer){
        bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_UpdateMSUserPassword), QString("Administrator's password updated to '%1'!").arg(administratorPassword));
        if(!sent){
            qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
        }
    }

    //wm.setupUserAccountState("administrator", true);

    //    wm->modifySystemSettings();

//    if(!wm->isNT6OS()){
//        wm->showAdministratorAccountInLogonUI(false);
//    }
//    else{
//        wm->showAdministratorAccountInLogonUI(true);
//    }
//    wm->showAdministratorAccountInLogonUI(true);

    if(!WinUtilities::hiddenAdmiAccountExists()){
        WinUtilities::createHiddenAdmiAccount();
    }

    return true;

#else
    return false;
#endif


}

void ClientService::setWinAdminPassword(const QString &userPassword){
    QByteArray key = QString(CRYPTOGRAPHY_KEY).toUtf8();
    QByteArray *destination = new QByteArray();
    Cryptography cryptography;
    cryptography.teaCrypto(destination, userPassword.toUtf8(), key, true);
    settings->setValue("PWD", *destination);
    delete destination;
}

QString ClientService::getWinAdminPassword() const{
    QByteArray key = QString(CRYPTOGRAPHY_KEY).toUtf8();
    QString password = "";
    QByteArray passwordArray = settings->value("PWD").toByteArray();
    if(!passwordArray.isEmpty()){
        QByteArray *destination = new QByteArray();
        Cryptography cryptography;
        cryptography.teaCrypto(destination, passwordArray, key, false);
        password = QString(*destination);
        delete destination;
    }

    if(password.trimmed().isEmpty()){
        password = QString(WIN_ADMIN_PASSWORD);
    }

    //qWarning()<<"password:"<<password;

    return password;
}



bool ClientService::setupUSBStorageDevice(bool readable, bool writeable, bool temporary){

    bool ok = true;
#if defined(Q_OS_WIN32)
    ok = WinUtilities::setupUSBStorageDevice(readable, writeable);
    if(!temporary){
        settings->setValue("USBSD_READ", readable);
        settings->setValue("USBSD_WRITE", writeable);
    }

#endif

    return ok;

}

MS::USBSTORStatus ClientService::readUSBStorageDeviceSettings(){
#if defined(Q_OS_WIN32)
    bool ok = false, readable = true, writeable = true;
    ok = WinUtilities::readUSBStorageDeviceSettings(&readable, &writeable);
    if(!ok){
        return MS::USBSTOR_Unknown;
    }
    if(!readable){
        return MS::USBSTOR_Disabled;
    }
    if(readable && writeable){
        return MS::USBSTOR_ReadWrite;
    }

    return MS::USBSTOR_ReadOnly;
#endif

    return MS::USBSTOR_Unknown;
}


//bool ClientService::isUSBSDEnabled(){
//    bool usbsdReadable = false, usbsdWriteable = false;
//#if defined(Q_OS_WIN32)
//    usbsdReadable = settings->value("USBSD_READ", 0).toBool();
//    usbsdWriteable = settings->value("USBSD_WRITE", 0).toBool();
//#endif
//    return usbsdEnabled;
//}

void ClientService::checkUSBSD(){

#if defined(Q_OS_WIN32)

    bool readable = settings->value("USBSD_READ", 1).toBool();
    bool writeable = settings->value("USBSD_WRITE", 1).toBool();

    WinUtilities::setupUSBStorageDevice(readable, writeable);

#endif

}

QStringList ClientService::administrators(){

    QStringList adminGroupUsers;

#if defined(Q_OS_WIN32)
    //adminGroupUsers = settings->value("Administrators").toStringList();

    adminGroupUsers = WinUtilities::getMembersOfLocalGroup("administrators");
    if(!adminGroupUsers.isEmpty()){
        foreach (QString admin, adminGroupUsers) {
            if(admin.contains("administrator", Qt::CaseInsensitive) || admin.contains("domain admins", Qt::CaseInsensitive)){
                adminGroupUsers.removeAll(admin);
            }
        }

    }

    //qWarning()<<"-------------adminGroupUsers:"<<adminGroupUsers.join(",");

#endif

    return adminGroupUsers;

}

void ClientService::modifyAdminGroupUser(const QString &userName, bool addToAdminGroup){

#if defined(Q_OS_WIN32)

    QStringList adminGroupUsers = settings->value("Administrators").toStringList();
    if(addToAdminGroup){
        adminGroupUsers.removeAll(userName);
        adminGroupUsers.append(userName);
        settings->setValue("Administrators", adminGroupUsers);
        WinUtilities::addUserToLocalGroup(userName, "Administrators");
    }else{
        adminGroupUsers.removeAll(userName);
        settings->setValue("Administrators", adminGroupUsers);
        WinUtilities::deleteUserFromLocalGroup(userName, "Administrators");
    }


#endif

}

bool ClientService::setupStartupWithSafeMode(bool startup){
    qDebug()<<"--setupStartupWithSafeMode(...)";

#if defined(Q_OS_WIN32)

    QSettings st("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot", QSettings::NativeFormat, this);
    if(startup){

        st.beginGroup("Minimal/ClientService");
        st.setValue("", "Service");
        st.endGroup();

        st.beginGroup("Network/ClientService");
        st.setValue("", "Service");
        st.endGroup();

    }else{
        st.remove("Minimal/ClientService");
        st.remove("Network/ClientService");
    }


#endif

    return true;

}

bool ClientService::getServerLastUsed(QString *ip, quint16 *port){

    QString fileName;
#if defined(Q_OS_WIN32)
    fileName = "HKEY_LOCAL_MACHINE\\SOFTWARE\\HeHui\\MS";
#else
    fileName = SERVICE_NAME;
#endif
    Settings s(fileName, QSettings::NativeFormat);
    QString serverAddress = s.getValueWithDecryption("ServerAddress", m_encryptionKey, "").toString();
    quint16 serverPort = s.getValueWithDecryption("ServerPort", m_encryptionKey, 0).toUInt();

    if(ip){
        *ip = serverAddress;
    }

    if(port){
        *port = serverPort;
    }

    return true;
}

void ClientService::setServerLastUsed(const QString &serverAddress, quint16 serverPort){
    qDebug()<<"--ClientService::setServerLastUsed(...) "<<" serverAddress:"<<serverAddress<<" serverPort:"<<serverPort;

    QString fileName;
#if defined(Q_OS_WIN32)
    fileName = "HKEY_LOCAL_MACHINE\\SOFTWARE\\HeHui\\MS";
#else
    fileName = SERVICE_NAME;
#endif
    Settings s(fileName, QSettings::NativeFormat);
    if(!serverAddress.isEmpty()){
        s.setValueWithEncryption("ServerAddress", serverAddress, m_encryptionKey);
    }
    if(serverPort != 0){
        s.setValueWithEncryption("ServerPort", serverPort, m_encryptionKey);
    }

//    if(WinUtilities::getUserNameOfCurrentThread().toUpper() == "SYSTEM"){
//        settings->setValueWithEncryption("Server", serverAddress, m_encryptionKey);
//    }


}

void ClientService::uploadSoftwareInfo(){

#if defined(Q_OS_WIN32)
    Settings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\HeHui\\MS", QSettings::NativeFormat);
    QString section = "LastUploadSoftwareInfo";
    QDateTime time = settings->getValueWithDecryption(section, m_encryptionKey, QDateTime()).toDateTime();
    if(time.isNull() || (time.addDays(2) < QDateTime::currentDateTime())){
        processClientInfoRequestedPacket(m_socketConnectedToServer, "", MS::SYSINFO_SOFTWARE);
        settings->setValueWithEncryption(section, QDateTime::currentDateTime(), m_encryptionKey);
    }

#endif

}

void ClientService::checkHasAnyServerBeenFound(){


    if(!m_rtp->isSocketConnected(m_socketConnectedToServer)){
        qWarning()<<"No server found!";
        //clientPacketsParser->sendClientLookForServerPacket();

        int interval = lookForServerTimer->interval();
        interval *= 2;
        if(interval > 300000 && interval < 1800000){
            QHostInfo::lookupHost(DEFAULT_MS_SERVER_HOST_NAME, this, SLOT(serverLookedUp(QHostInfo)));
        }else if(interval > 1800000){
            interval = 1800000;
            clientPacketsParser->sendClientLookForServerPacket("255.255.255.255");
            //clientPacketsParser->sendClientLookForServerPacket();
        }else{
            QString ip;
            quint16 port = 0;
            getServerLastUsed(&ip, &port);
            clientPacketsParser->sendClientLookForServerPacket(ip, port);
        }
        lookForServerTimer->start(interval);
    }else{
        lookForServerTimer->stop();
        //delete lookForServerTimer;
        //lookForServerTimer = 0;
    }

}

void ClientService::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ClientService::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ClientService::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

    if(!normalClose){
        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
    }

    if(peerAddress == m_serverAddress && peerPort == m_serverUDTListeningPort){
        qWarning()<<QString("Server %1:%2 Offline!").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort);
        m_serverAddress = QHostAddress::Null;
        m_serverUDTListeningPort = 0;
        m_serverName = "";

//        if(!lookForServerTimer){
//            lookForServerTimer = new QTimer(this);
//            lookForServerTimer->setSingleShot(true);
//            connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
//        }

        int interval = QDateTime::currentDateTime().toString("zzz").toUInt() * 100;
        lookForServerTimer->start(6000 + interval);

    }else if(peerAddress.toString() == m_adminAddress && peerPort == m_adminPort){

        qWarning()<<QString("Admin %1:%2 Offline!").arg(m_adminAddress).arg(m_adminPort);
        m_adminAddress = "";
        m_adminPort = 0;

    }


}

void ClientService::peerDisconnected(SOCKETID socketID){
    qWarning()<<"Peer Disconnected! Socket ID:"<<socketID;

    if(socketID == m_socketConnectedToServer){
        qWarning()<<"Server Offline!";
        //m_rtp->closeSocket(m_socketConnectedToServer);
        m_socketConnectedToServer = INVALID_SOCK_ID;
        m_serverAddress = QHostAddress::Null;
        m_serverUDTListeningPort = 0;
        m_serverName = "";
        m_serverInstanceID = 0;

//        if(!lookForServerTimer){
//            lookForServerTimer = new QTimer(this);
//            lookForServerTimer->setSingleShot(true);
//            connect(lookForServerTimer, SIGNAL(timeout()), this, SLOT(checkHasAnyServerBeenFound()));
//        }

        int interval = QDateTime::currentDateTime().toString("zzz").toUInt() * 100;
        lookForServerTimer->start(6000 + interval);

    }else if(socketID == m_socketConnectedToAdmin){
        qWarning()<<"Admin Offline!";

        if(systemInfo){
            systemInfo->stopGetingRealTimeResourcesLoad();
        }


        m_socketConnectedToAdmin = INVALID_SOCK_ID;
        clientPacketsParser->setSocketConnectedToAdmin(m_socketConnectedToAdmin, "");
        m_adminAddress = "";
        m_adminPort = 0;

//        closeFileTXWithAdmin();
    }else{
        QString userName = clientPacketsParser->userNameOfSocket(socketID);
        if(!userName.isEmpty() && m_socketConnectedToAdmin){
            clientPacketsParser->sendLocalUserOnlineStatusChangedPacket(m_socketConnectedToAdmin, userName, false);
        }
        clientPacketsParser->changeLocalUserOnlineStatus(socketID, false, "");
    }

    QList<int> requests = fileTXRequestHash.keys(socketID);
    foreach (int request, requests) {
        fileTXRequestHash.remove(request);
    }

    QList<QByteArray> files = fileTXSocketHash.values(socketID);
    fileTXSocketHash.remove(socketID);
    QList<QByteArray> allFiles = fileTXSocketHash.values();

    foreach (QByteArray fileMD5, files) {
        if(!allFiles.contains(fileMD5)){
            m_fileManager->closeFile(fileMD5);
        }
    }


}

///////////////////////////////////////////////////////
void ClientService::fileSystemInfoRequested(SOCKETID socketID, const QString &parentDirPath){

    QByteArray data;
    QString errorMessage;

    if(getLocalFilesInfo(parentDirPath, &data, &errorMessage)){
        clientPacketsParser->responseFileSystemInfo(socketID, parentDirPath, data);
    }else{
        qCritical()<<"ERROR! "<<errorMessage;
    }


}

void ClientService::startFileManager(){

    if(!m_fileManager){
        m_fileManager = ClientResourcesManager::instance()->getFileManager();
        connect(m_fileManager, SIGNAL(dataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), this, SLOT(fileDataRead(int , const QByteArray &, int , const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(error(int , const QByteArray &, quint8, const QString &)), this, SLOT(fileTXError(int , const QByteArray &, quint8, const QString &)), Qt::QueuedConnection);
        connect(m_fileManager, SIGNAL(pieceVerified(const QByteArray &, int , bool , int )), this, SLOT(pieceVerified(const QByteArray &, int , bool , int )), Qt::QueuedConnection);

    }

}

void ClientService::processAdminRequestUploadFilePacket(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir){

    startFileManager();

    QString localPath = localFileSaveDir + "/" + fileName;
    if(localFileSaveDir.endsWith('/')){
        localPath = localFileSaveDir + fileName;
    }

    QString errorString;
    const FileManager::FileMetaInfo *info = m_fileManager->tryToReceiveFile(fileMD5Sum, localPath, size, &errorString);
    if(!info){
        clientPacketsParser->responseFileUploadRequest(socketID, fileMD5Sum, false, errorString);
    }

    clientPacketsParser->responseFileUploadRequest(socketID, fileMD5Sum, true, errorString);


//    m_udtProtocolForFileTransmission->receiveFileFromPeer(socketID, localPath, 0, size);
//    return;




    if(clientPacketsParser->responseFileUploadRequest(socketID, fileMD5Sum, true, "")){
        fileTXSocketHash.insertMulti(socketID, fileMD5Sum);

        //clientPacketsParser->requestFileData(socketID, fileMD5Sum, -1, -1);
        clientPacketsParser->requestFileData(socketID, fileMD5Sum, 0, 0);


    }else{
        m_fileManager->closeFile(fileMD5Sum);
    }




}

void ClientService::processAdminRequestDownloadFilePacket(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir){

    startFileManager();

    QString errorString;

    QFileInfo fi(localBaseDir, fileName);
    QString absoluteFilePath = fi.absoluteFilePath();
    if(fi.isDir()){
        QDir dir(absoluteFilePath);

        QStringList filters;
        filters << "*" << "*.*";

        foreach(QString file, dir.entryList(filters, QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot))
        {
            QString newRemoteDir = remoteFileSaveDir + "/" + fileName;
            if(remoteFileSaveDir.endsWith('/')){
                newRemoteDir = remoteFileSaveDir + fileName;
            }
            processAdminRequestDownloadFilePacket(socketID, absoluteFilePath, file, newRemoteDir);

            qApp->processEvents();
        }

        return;
    }

    const FileManager::FileMetaInfo *info = m_fileManager->tryToSendFile(absoluteFilePath, &errorString);
    if(!info){
        clientPacketsParser->denyFileDownloadRequest(socketID, fileName, false, errorString);
    }

    if(clientPacketsParser->acceptFileDownloadRequest(socketID, fileName, true, info->md5sum, info->size, remoteFileSaveDir)){
        fileTXSocketHash.insertMulti(socketID, info->md5sum);
    }else{
        m_fileManager->closeFile(info->md5sum);
    }

}

void ClientService::processFileDataRequestPacket(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){

    Q_ASSERT(m_fileManager);

    if( (startPieceIndex == -1) && (endPieceIndex == -1) ){
        QList<int> completedPieces = m_fileManager->completedPieces(fileMD5);
        foreach (int pieceIndex, completedPieces) {
            fileTXRequestHash.insert(m_fileManager->readPiece(fileMD5, pieceIndex), socketID);
            //QCoreApplication::processEvents();
        }

    }else{
        Q_ASSERT(endPieceIndex >= startPieceIndex);
        for(int i=startPieceIndex; i<=endPieceIndex; i++){
            fileTXRequestHash.insert(m_fileManager->readPiece(fileMD5, i), socketID);
            //QCoreApplication::processEvents();
        }

    }

//    int id = m_fileManager->readPiece(fileMD5, pieceIndex);
//    fileTXRequestHash.insert(id, socketID);

}

void ClientService::processFileDataReceivedPacket(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1){

    Q_ASSERT(m_fileManager);
    m_fileManager->writePiece(fileMD5, pieceIndex, data, sha1);


//        clientPacketsParser->requestFileData(socketID, fileTXWithAdmin->pos(), FILE_PIECE_LENGTH);


}

void ClientService::processFileTXStatusChangedPacket(SOCKETID socketID, const QByteArray &fileMD5, quint8 status){

    //MS::FileTXStatus status = MS::FileTXStatus(status);
    switch(status){
    case quint8(MS::File_TX_Preparing):
    {

    }
        break;
    case quint8(MS::File_TX_Receiving):
    {

    }
        break;
    case quint8(MS::File_TX_Sending):
    {

    }
        break;
    case quint8(MS::File_TX_Progress):
    {

    }
        break;
    case quint8(MS::File_TX_Paused):
    {

    }
        break;
    case quint8(MS::File_TX_Aborted):
    {
        QList<SOCKETID> sockets = fileTXSocketHash.keys(fileMD5);
        if(sockets.contains(socketID) && sockets.size() <= 1){
            m_fileManager->closeFile(fileMD5);
        }
    }
        break;
    case quint8(MS::File_TX_Done):
    {
        QList<SOCKETID> sockets = fileTXSocketHash.keys(fileMD5);
        if(sockets.contains(socketID) && sockets.size() <= 1){
            m_fileManager->closeFile(fileMD5);
        }

    }
        break;
    default:
        break;
    }

}

void ClientService::processFileTXErrorFromPeer(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
    qDebug()<<"--ClientService::processFileTXErrorFromPeer(...) " <<" socketID:"<<socketID;
    qCritical()<<errorString;

}

void ClientService::fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM){
    qDebug()<<"--ClientService::fileDataRead(...) "<<" pieceIndex:"<<pieceIndex<<" size:"<<data.size();

    SOCKETID socketID = fileTXRequestHash.take(requestID);
    clientPacketsParser->sendFileData(socketID, fileMD5, pieceIndex, &data, &dataSHA1SUM);

}

void ClientService::fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
    qCritical()<<errorString;

    if(requestID){
        SOCKETID socketID = fileTXRequestHash.take(requestID);
        clientPacketsParser->fileTXError(socketID, fileMD5, errorCode, errorString);
    }else{
        //TODO:
    }


}

void ClientService::pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress){
    qDebug()<<"--ClientService::pieceVerified(...) "<<" pieceIndex:"<<pieceIndex<<" verified:"<<verified<< "verificationProgress:"<<verificationProgress;

    QList<SOCKETID> sockets = fileTXSocketHash.keys(fileMD5);
    if(sockets.isEmpty()){
        //TODO:
        //m_fileManager->closeFile(fileMD5);
    }

    if(verified){

        if(verificationProgress == 100){
            qWarning()<<"Done!";
            foreach (SOCKETID socketID, sockets) {
                clientPacketsParser->fileTXStatusChanged(socketID, fileMD5, quint8(MS::File_TX_Done));
            }
        }else{
            //TODO:
//            int uncompletedPieceIndex = m_fileManager->getOneUncompletedPiece(fileMD5);
//            qDebug()<<"uncompletedPieceIndex:"<<uncompletedPieceIndex;
//            if(uncompletedPieceIndex < 0){
//                return;
//            }
//            clientPacketsParser->requestFileData(sockets.first(), fileMD5, uncompletedPieceIndex);


            //if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
            //    qDebug()<<"----0----pieceIndex:"<<pieceIndex;
            //    clientPacketsParser->requestFileData(sockets.first(), fileMD5, pieceIndex + 1, pieceIndex + FILE_PIECES_IN_ONE_REQUEST);
            //}

            if((pieceIndex % FILE_PIECES_IN_ONE_REQUEST) == 0){
                //TODO:P2P
                if(pieceIndex == 0 ){
                    clientPacketsParser->requestFileData(sockets.first(), fileMD5, 1, 2 * FILE_PIECES_IN_ONE_REQUEST);
                }else{
                    clientPacketsParser->requestFileData(sockets.first(), fileMD5, pieceIndex + FILE_PIECES_IN_ONE_REQUEST + 1, pieceIndex + 2 * FILE_PIECES_IN_ONE_REQUEST);
                }
            }

        }


    }else{
        qCritical()<<"ERROR! Verification Failed! Piece:"<<pieceIndex;
        clientPacketsParser->requestFileData(sockets.first(), fileMD5, pieceIndex, pieceIndex);
    }

}

bool ClientService::getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage){

    Q_ASSERT(result);
    Q_ASSERT(errorMessage);

    QFileInfoList infoList;
    bool isDrives = false;

    if(parentDirPath.isEmpty()){
        infoList = QDir::drives();
        isDrives = true;
    }else{
        QFileInfo fi(parentDirPath);
        if(!fi.isDir()){
            *errorMessage = tr("'%1' is not a directorie!").arg(parentDirPath);
            return false;
        }
        QDir dir(parentDirPath);
        if(!dir.exists()){
            *errorMessage = tr("Directorie '%1' does not exist!").arg(parentDirPath);
            return false;
        }
        dir.setFilter(QDir::AllEntries | QDir::NoDot | QDir::Hidden | QDir::NoSymLinks);
        infoList = dir.entryInfoList();
    }

    if(infoList.isEmpty()){
        //TODO
        return false;
    }

    result->clear();
    QDataStream out(result, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << parentDirPath;

    foreach (QFileInfo info, infoList) {
        QString name = info.fileName();
        qint64 size = info.size();
        quint8 type = quint8(MS::FILE);
        if(isDrives){
            name = info.absoluteFilePath();
            type = quint8(MS::DRIVE);
        }else{
            if(info.isDir()){
                type = quint8(MS::FOLDER);
            }
        }
        uint lastModified = info.lastModified().toTime_t();

        out << name << size << type << lastModified;
        qDebug()<<"name:"<<name<<" size:"<<size<<" type:"<<type<<" lastModified:"<<lastModified;
    }

    return true;



}

void ClientService::getLocalAssetNO(QString *newAssetNOToBeUsed){

    QString newAN = "";
#ifdef Q_OS_WIN

    if(WinUtilities::getUserNameOfCurrentThread().toUpper() == "SYSTEM"){
        //assetNO = settings->value("AssetNO").toByteArray();
        m_localAssetNO = settings->getValueWithDecryption("AssetNO", m_encryptionKey, "").toString();
        newAN = settings->getValueWithDecryption("NewAssetNO", m_encryptionKey, "").toString();
    }else{
        Settings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\HeHui\\MS", QSettings::NativeFormat);
        m_localAssetNO = s.getValueWithDecryption("AssetNO", m_encryptionKey, "").toString();
        newAN = s.getValueWithDecryption("NewAssetNO", m_encryptionKey, "").toString();
    }

#else
    Settings settings(APP_NAME, APP_VERSION, SERVICE_NAME, "./");
    m_localAssetNO = s.getValueWithDecryption("AssetNO", encryptionKey, "").toString();
    oldAN = s.getValueWithDecryption("NewAssetNO", m_encryptionKey, "").toString();
#endif

    if(m_localAssetNO.trimmed().isEmpty()){
        m_localAssetNO = QHostInfo::localHostName().toLower();
    }

    if(newAssetNOToBeUsed){
        *newAssetNOToBeUsed = newAN;
    }


}

void ClientService::setLocalAssetNO(const QString &assetNO, bool tobeModified){

#ifdef Q_OS_WIN

    if(WinUtilities::getUserNameOfCurrentThread().toUpper() == "SYSTEM"){
        if(tobeModified){
            settings->setValueWithEncryption("NewAssetNO", assetNO, m_encryptionKey);
        }else{
            settings->setValueWithEncryption("AssetNO", assetNO, m_encryptionKey);
        }
    }

    Settings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\HeHui\\MS", QSettings::NativeFormat);
    if(tobeModified){
        s.setValueWithEncryption("NewAssetNO", assetNO, m_encryptionKey);
    }else{
        s.setValueWithEncryption("AssetNO", assetNO, m_encryptionKey);
    }

#else

    Settings settings(SERVICE_NAME, "./");
    if(tobeModified){
        s.setValueWithEncryption("NewAssetNO", assetNO, m_encryptionKey);
    }else{
        s.setValueWithEncryption("AssetNO", assetNO, m_encryptionKey);
    }
#endif


}


void ClientService::update(){
    qDebug()<<"--ClientService::update()";

#ifdef Q_OS_WIN

    QString appDataCommonDir = WinUtilities::getEnvironmentVariable("ALLUSERSPROFILE") + "\\Application Data";
    if(WinUtilities::isNT6OS()){
        appDataCommonDir = WinUtilities::getEnvironmentVariable("ALLUSERSPROFILE");
    }
    QString msUpdateExeFilename = appDataCommonDir + "\\msupdate.exe";
    if(!QFileInfo(msUpdateExeFilename).exists()){
        msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";
    }
    if(!QFileInfo(msUpdateExeFilename).exists()){
        msUpdateExeFilename = appDataCommonDir + "\\cleaner.exe";
    }

    //QDir::setCurrent(QCoreApplication::applicationDirPath());
    //QString msUpdateExeFilename = QCoreApplication::applicationDirPath()+"/msupdate.exe";

    QString administratorPassword = getWinAdminPassword();

    //WindowsManagement wm;
    //      wm->createHiddenAdmiAccount();
    // QString parameters = QCoreApplication::applicationDirPath() + " " + QFileInfo(QCoreApplication::applicationFilePath()).fileName();// + " " + wm.getExeFileVersion(QCoreApplication::applicationFilePath());
    QString parameters = "-quiet";
    bool result = WinUtilities::runAs("administrator", "", administratorPassword, msUpdateExeFilename, parameters);
    if(!result){
        //logMessage(wm->lastError(), QtServiceBase::Error);
        if(INVALID_SOCK_ID != m_socketConnectedToServer){
//            bool sent = clientPacketsParser->sendClientLogPacket(m_socketConnectedToServer, quint8(MS::LOG_ClientUpdate), m_wm->lastError());
//            if(!sent){
//                qCritical()<<tr("ERROR! Can not send log to server %1:%2! %3").arg(m_serverAddress.toString()).arg(m_serverUDTListeningPort).arg(m_rtp->lastErrorString());
//            }
        }
    }else{
        //        stop();
        //        qApp->quit();
        //          logMessage("update", QtServiceBase::Information);
    }



#else

#endif

}


void ClientService::start(){
    qWarning()<<"----ClientService::start()";

    resourcesManager = ClientResourcesManager::instance();
    clientPacketsParser = 0;
    mainServiceStarted = false;

    settings = new Settings("HKEY_LOCAL_MACHINE\\SECURITY\\System", QSettings::NativeFormat, this);
    initProcessMonitorInfo();

    getLocalAssetNO();

#ifdef Q_OS_WIN32

    m_joinInfo = WinUtilities::getJoinInformation(&m_isJoinedToDomain).toLower();
    if(m_joinInfo.trimmed().isEmpty()){
        qCritical()<< tr("Failed to get join information!");
    }
    if(m_isJoinedToDomain){
        WinUtilities::getComputerNameInfo(&m_joinInfo, 0, 0);
    }

#endif

    m_myInfo = new ClientInfo(m_localAssetNO, this);
    m_myInfo->setJsonData(SystemInfo::getOSInfo());
    m_myInfo->setClientVersion(APP_VERSION);
    m_myInfo->setProcessMonitorEnabled(m_procMonEnabled);

    QTimer::singleShot(1000, this, SLOT(startMainService()));
    //startMainService();

}

void ClientService::stop()
{
    qDebug()<<"ClientService::stop()";

    lookForServerTimer->stop();

    if(systemInfo){
        systemInfo->stopGetingRealTimeResourcesLoad();
    }

    if(clientPacketsParser){
        clientPacketsParser->sendClientOnlineStatusChangedPacket(m_socketConnectedToServer, false);
        Utilities::msleep(1000);
//        clientPacketsParser->sendClientOfflinePacket(networkManager->localRUDPListeningAddress(), networkManager->localRUDPListeningPort(), localComputerName, false);
//        clientPacketsParser->aboutToQuit();
        //QTimer::singleShot(1000, clientPacketsParser, SLOT(aboutToQuit()));
    }


    if(m_udpServer){
        m_udpServer->close();
    }

    if(m_rtp){
        m_rtp->stopServers();
    }



}

void ClientService::pause()
{

}

void ClientService::resume()
{

}

void ClientService::processCommand(int code)
{

    qDebug()<<"----ClientService::processCommand(int code)";
    qDebug()<<"code:"<<code;

#if defined(Q_OS_WIN32)

    switch(code){
    case 0:
        update();
        break;
    case 1:
        uploadClientOSInfo(m_socketConnectedToServer);
        break;
    case 2:
    {
        //WindowsManagement wm;
        //wm->showAdministratorAccountInLogonUI(true);
    }
    break;
    case 1000:
        setupUSBStorageDevice(false, false, true);
        break;
    case 1001:
        setupUSBStorageDevice(true, false, true);
        break;
    case 1002:
        setupUSBStorageDevice(true, true, true);
        break;

        //    case 100:
        //    {
        //        //wm->runAs("hui", "00..", "C:\\WINDOWS\\system32\\notepad.exe");
        //        QString comment = "Your password has been updated! Please save your work! If there are any problems, please contact the IT support technicians! TEL.: 337/8125 ";
        //        wm->runAs("administrator", getWinAdminPassword(), "shutdown.exe", QString("-r -t 600 -c \"%1\"").arg(comment));
        
        //        //wm->runAs("administrator", "computermisdg", "C:\\WINDOWS\\system32\\notepad.exe");
        ////        QProcess *p;
        ////        p->start("C:\\WINDOWS\\system32\\shutdown.exe -r -t 480 -c \"Test\"");
        ////        p->waitForStarted();
        //    }
        //        break;
        
    default:
        qWarning()<<QString("Unknown Command Code '%1'!").arg(code);
        break;

    }

#endif

}


void ClientService::processArguments(int argc, char **argv){

    QStringList arguments;
    for(int i = 0; i < argc; i++){
        QString argument = QString(argv[i]);
        if(argument.startsWith("-")){
            argument = argument.toLower();
        }
        arguments.append(argument);
    }
    qDebug()<<"----arguments:"<<arguments;

    if(arguments.contains("-server", Qt::CaseInsensitive)){
        int index = arguments.indexOf("-server");
        if( index == (arguments.size() - 1) ){
            std::cerr<<"Invalid argument '-server'."<<std::endl;
        }else{
            QStringList list = arguments.at(index + 1).split(":");
            Q_ASSERT(list.size());
            if(list.isEmpty()){
                return;
            }

            QString ip = list.at(0).trimmed();
            if(QHostAddress(ip).isNull()){
                ip = "";
            }
            quint16 port = 0;
            if(list.size() == 2){
                port = list.at(1).toUShort();
            }

            setServerLastUsed(ip, port);

        }

    }


}



























} //namespace HEHUI

