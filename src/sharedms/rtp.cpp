
#include <QDebug>
#include "rtp.h"

#include "global_shared.h"
#include "mspackets.h"

namespace HEHUI
{


RTP::RTP(QObject *parent) :
    QObject(parent)
{

    qRegisterMetaType<SOCKETID>("SOCKETID");
    qRegisterMetaType<PacketBase>("PacketBase");

    qRegisterMetaType<ServerDiscoveryPacket>("ServerDiscoveryPacket");
    qRegisterMetaType<DataForwardPacket>("DataForwardPacket");
    qRegisterMetaType<MessagePacket>("MessagePacket");
    qRegisterMetaType<JobProgressPacket>("JobProgressPacket");
    qRegisterMetaType<AdminLoginPacket>("AdminLoginPacket");
    qRegisterMetaType<ClientInfoPacket>("ClientInfoPacket");
    qRegisterMetaType<SystemInfoFromServerPacket>("SystemInfoFromServerPacket");
    qRegisterMetaType<SysAdminInfoPacket>("SysAdminInfoPacket");
    qRegisterMetaType<SystemAlarmsPacket>("SystemAlarmsPacket");
    qRegisterMetaType<AnnouncementPacket>("AnnouncementPacket");
    qRegisterMetaType<RemoteConsolePacket>("RemoteConsolePacket");
    qRegisterMetaType<ClientLogPacket>("ClientLogPacket");
    qRegisterMetaType<USBDevPacket>("USBDevPacket");
    qRegisterMetaType<AdminConnectionToClientPacket>("AdminConnectionToClientPacket");
    qRegisterMetaType<AdminSearchClientPacket>("AdminSearchClientPacket");
    qRegisterMetaType<LocalUserOnlineStatusChangedPacket>("LocalUserOnlineStatusChangedPacket");
    qRegisterMetaType<FileTransferPacket>("FileTransferPacket");
    qRegisterMetaType<ModifyAssetNOPacket>("ModifyAssetNOPacket");
    qRegisterMetaType<RenameComputerPacket>("RenameComputerPacket");
    qRegisterMetaType<JoinOrUnjoinDomainPacket>("JoinOrUnjoinDomainPacket");
    qRegisterMetaType<TemperaturesPacket>("TemperaturesPacket");
    qRegisterMetaType<ScreenshotPacket>("ScreenshotPacket");
    qRegisterMetaType<ShutdownPacket>("ShutdownPacket");
    qRegisterMetaType<LockWindowsPacket>("LockWindowsPacket");
    qRegisterMetaType<WinUserPacket>("WinUserPacket");
    qRegisterMetaType<ServiceConfigPacket>("ServiceConfigPacket");
    qRegisterMetaType<ProcessMonitorInfoPacket>("ProcessMonitorInfoPacket");


#ifdef UDT_ENABLED
    m_udtProtocol = 0;
//    m_udtProtocol = new UDTProtocol(true, 0, this);
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SIGNAL(disconnected(int)));
#endif

    m_tcpServer = 0;
//    m_tcpServer = new TCPServer(this);
//    connect(m_tcpServer, SIGNAL(disconnected(SOCKETID)), this, SIGNAL(disconnected(SOCKETID)));

    m_enetProtocol = 0;
//    m_enetProtocol = new ENETProtocol(this);
//    connect(m_enetProtocol, SIGNAL(connected(quint32, const QString &, quint16)), this, SLOT(connected(quint32, const QString &, quint16)));
//    connect(m_enetProtocol, SIGNAL(disconnected(SOCKETID, const QString &, quint16)), this, SIGNAL(disconnected(SOCKETID, const QString &, quint16)));


}

RTP::~RTP()
{
    qDebug() << "--RTP::~RTP()";

#ifdef UDT_ENABLED
    if(m_udtProtocol) {
        //m_udtProtocol->close();
        delete m_udtProtocol;
        m_udtProtocol = 0;
    }
#endif

    if(m_tcpServer) {
        m_tcpServer->closeServer();
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    if(m_enetProtocol) {
        m_enetProtocol->close();
        delete m_enetProtocol;
        m_enetProtocol = 0;
    }

}

void RTP::startServers(const QHostAddress &localAddress, quint16 localPort, bool tryOtherPort, QString *errorMessage)
{

    QString err;

//    UDTSOCKET socket = m_udtProtocol->listen(localPort, localAddress);
//    if(socket == INVALID_SOCK_ID && tryOtherPort){
//        socket = m_udtProtocol->listen();
//    }
//    if(socket == INVALID_SOCK_ID){
//        err = m_udtProtocol->getLastErrorMessage();
//    }

//    bool ok = m_tcpServer->listen(localAddress, localPort);
//    if((!ok) && tryOtherPort){
//        ok = m_tcpServer->listen(localAddress);
//    }
//    if(!ok){
//        err += " " + m_tcpServer->errorString();
//    }

    startTCPServer(localAddress, localPort, tryOtherPort, &err);
    startENETProtocol(localAddress, localPort, tryOtherPort, &err);

    if(errorMessage) {
        *errorMessage = err;
    }


}

void RTP::stopServers()
{

#ifdef UDT_ENABLED
    if(m_udtProtocol) {
        m_udtProtocol->close();
    }
#endif

    if(m_tcpServer) {
        m_tcpServer->closeServer();
    }

    if(m_enetProtocol) {
        m_enetProtocol->close();
    }

}


#ifdef UDT_ENABLED
UDTProtocol *RTP::startUDTProtocol(const QHostAddress &localAddress, quint16 localPort, bool tryOtherPort, QString *errorMessage)
{

    if(!m_udtProtocol) {
        m_udtProtocol = new UDTProtocol(true, 0, this);
        connect(m_udtProtocol, SIGNAL(connected(quint32, const QString &, quint16)), this, SLOT(udtPeerConnected(quint32, const QString &, quint16)));
        connect(m_udtProtocol, SIGNAL(disconnected(SOCKETID/*, const QString &, quint16*/)), this, SIGNAL(disconnected(SOCKETID/*, const QString &, quint16*/)));
    }

    UDTSOCKET socket = m_udtProtocol->listen(localPort, localAddress);
    if(socket == UDTProtocolBase::INVALID_UDT_SOCK && tryOtherPort) {
        socket = m_udtProtocol->listen();
    }

    if(socket == UDTProtocolBase::INVALID_UDT_SOCK) {
        if(errorMessage) {
            *errorMessage = m_udtProtocol->getLastErrorMessage();
        }
        delete m_udtProtocol;
        m_udtProtocol = 0;
        return 0;
    }

    return m_udtProtocol;

}

quint16 RTP::getUDTServerPort()
{
    if(m_udtProtocol) {
        return m_udtProtocol->getUDTListeningPort();
    }
    return 0;
}

#endif


TCPServer *RTP::startTCPServer(const QHostAddress &address, quint16 port, bool tryOtherPort, QString *errorMessage)
{
    qDebug() << "--RTP::startTCPServer(...)";

    if(!m_tcpServer) {
        m_tcpServer = new TCPServer(this);
        connect(m_tcpServer, SIGNAL(connected(quint32, const QString &, quint16)), this, SLOT(tcpPeerConnected(quint32, const QString &, quint16)));
        connect(m_tcpServer, SIGNAL(disconnected(SOCKETID)), this, SIGNAL(disconnected(SOCKETID)));
    }

    if( (!m_tcpServer->listen(address, port)) && tryOtherPort) {
        m_tcpServer->listen();
    }

    if(!m_tcpServer->isListening()) {
        if(errorMessage) {
            *errorMessage = m_tcpServer->errorString();
            qCritical() << *errorMessage;
        }
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    quint16 listeningPort = port;
    if(!listeningPort){
        listeningPort = m_tcpServer->getTCPServerListeningPort();
    }
    qDebug() << "TCP listening on port " << listeningPort;

    return m_tcpServer;

}

quint16 RTP::getTCPServerPort()
{
    quint16 port;
    m_tcpServer->serverAddressInfo(0, &port);
    return port;
}

ENETProtocol *RTP::startENETProtocol(const QHostAddress &address, quint16 port, bool tryOtherPort, QString *errorMessage)
{

    if(!m_enetProtocol) {
        m_enetProtocol = new ENETProtocol(this);
        connect(m_enetProtocol, SIGNAL(connected(SOCKETID, const QString &, quint16)), this, SLOT(enetPeerConnected(SOCKETID, const QString &, quint16)));
        connect(m_enetProtocol, SIGNAL(disconnected(SOCKETID)), this, SIGNAL(disconnected(SOCKETID)));
    }

    if( (!m_enetProtocol->listen(port, address)) && tryOtherPort) {
        m_enetProtocol->listen(0);
    }

    if(!m_enetProtocol->isListening()) {
        if(errorMessage) {
            *errorMessage = m_enetProtocol->errorString();
            qCritical() << *errorMessage;
        }
        delete m_enetProtocol;
        m_enetProtocol = 0;
    }

    m_enetProtocol->startWaitingForIOInAnotherThread(50);


    quint16 listeningPort = port;
    if(!listeningPort){
        m_enetProtocol->getLocalListeningAddressInfo(0, &listeningPort);
    }
    qDebug() << "ENET listening on port " << listeningPort;

    return m_enetProtocol;
}

quint16 RTP::getENETProtocolPort()
{
    quint16 port;
    m_enetProtocol->getLocalListeningAddressInfo(0, &port);
    return port;
}


SOCKETID RTP::connectToHost( const QHostAddress &hostAddress, quint16 port, int waitMsecs, QString *errorMessage, Protocol protocol)
{
    qDebug() << "--RTP::connectToHost(...) " << " Protocol:" << protocol;

    SOCKETID socketID = INVALID_SOCK_ID;
    QString err;
    bool connected = false;

    switch (protocol) {
    case AUTO: {
        socketID = connectToHost(hostAddress, port, waitMsecs, &err, ENET);
        if(INVALID_SOCK_ID == socketID) {
            socketID = connectToHost(hostAddress, port, waitMsecs, &err, TCP);
        }
        if(INVALID_SOCK_ID != socketID) {
            connected = true;
            return socketID;
        }
    }
    break;
    case TCP: {
        if(!m_tcpServer) {
            err = tr("TCP server not running.");
            break;
        }
        socketID = m_tcpServer->connectToHost(hostAddress, port, waitMsecs);
        if(!m_tcpServer->isConnected(socketID) ) {
            err += tr("\nCan not connect to host %1:%2 via TCP! %3").arg(hostAddress.toString()).arg(port).arg(m_tcpServer->socketErrorString(socketID));
            qCritical() << err;
            m_tcpServer->abort(socketID);
            socketID = INVALID_SOCK_ID;
        } else {
            connected = true;
        }
    }
    break;
    case ENET: {
        if(!m_enetProtocol) {
            err = tr("ENET not running.");
            break;
        }
        if(!m_enetProtocol->connectToHost(hostAddress, port, &socketID, waitMsecs)) {
            err += tr("\nCan not connect to host %1:%2 via ENET! %3").arg(hostAddress.toString()).arg(port).arg(m_enetProtocol->errorString());
            qCritical() << err;
        } else {
            connected = true;
        }

    }
    break;

#ifdef UDT_ENABLED
    case UDT: {
        if(!m_udtProtocol) {
            err = tr("UDT not running.");
            break;
        }
        UDTSOCKET udtSocketID = m_udtProtocol->connectToHost(hostAddress, port, 0, true, waitMsecs);
        if( (udtSocketID == UDTProtocolBase::INVALID_UDT_SOCK) || (!m_udtProtocol->isSocketConnected(udtSocketID)) ) {
            err += tr("Can not connect to host %1:%2 via UDT! %3").arg(hostAddress.toString()).arg(port).arg(m_udtProtocol->getLastErrorMessage());
            qCritical() << err;
        } else {
            socketID = (SOCKETID)udtSocketID;
            connected = true;
            qDebug() << QString("Peer %1:%2 connected via UDT! ").arg(hostAddress.toString()).arg(port);
        }

    }
    break;
 #endif

    default:
        break;
    }

    if(errorMessage) {
        *errorMessage = err;
    }

    if(connected) {
        //TODO
        if(m_socketInfoHash.contains(socketID)) {
            Protocol ptl = m_socketInfoHash.value(socketID);
            if(ptl != protocol) {
                closeSocket(socketID);
            }
        }
        m_socketInfoHash.insert(socketID, protocol);

        qDebug() << QString("Connected to %1:%2. Protocol:%3, SOCKETID:%4").arg(hostAddress.toString()).arg(port).arg(protocol).arg(socketID);
    }

    return socketID;

}

void RTP::closeSocket(SOCKETID socketID)
{
    qDebug() << "--RTP::closeSocket(...)" << "  SOCKETID:" << socketID;

    //m_udtProtocol->closeSocket(socketID);
//    m_tcpServer->disconnectFromHost(socketID);
//    m_enetProtocol->disconnect(socketID);

    if(!m_socketInfoHash.contains(socketID)) {
        return;
    }

    Protocol p = m_socketInfoHash.value(socketID);
    switch (p) {
    case TCP:
        Q_ASSERT(m_tcpServer);
        m_tcpServer->disconnectFromHost(socketID);
        break;
    case ENET:
        Q_ASSERT(m_enetProtocol);
        m_enetProtocol->disconnect(socketID);
        break;

#ifdef UDT_ENABLED
    case UDT:
        Q_ASSERT(m_udtProtocol);
        m_udtProtocol->closeSocket(socketID);
        break;
 #endif

    default:
        break;
    }

    m_socketInfoHash.remove(socketID);

}

bool RTP::isSocketConnected(SOCKETID socketID)
{

//    bool connected = false;
//    if(m_udtProtocol->isSocketExist(socketID)){
//        connected = m_udtProtocol->isSocketConnected(socketID);
//    }else{
//        connected = m_tcpServer->isConnected(socketID);
//    }

//    return connected;

    return m_socketInfoHash.contains(socketID);

}

bool RTP::getAddressInfoFromSocket(SOCKETID socketID, QString *address, quint16 *port, bool getPeerInfo)
{

//    if(m_udtProtocol->isSocketExist(socketID)){
//        return m_udtProtocol->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
//    }else{
//        return m_tcpServer->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
//    }

    Protocol p = m_socketInfoHash.value(socketID);
    switch (p) {
    case TCP:
        return m_tcpServer->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
        break;
    case ENET:
        return m_enetProtocol->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
        break;

#ifdef UDT_ENABLED
    case UDT:
        return m_udtProtocol->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
        break;
#endif

    default:
        break;
    }

    return false;

}


QString RTP::socketProtocolString(SOCKETID socketID)
{

    Protocol p = m_socketInfoHash.value(socketID);
    QString str = "Unknown";

    switch (p) {
    case TCP:
        str = "TCP";
        break;
    case ENET:
        str = "ENET";
        break;
    case UDT:
        str = "UDT";
        break;
    default:
        break;
    }

    return str;
}

bool RTP::sendReliableData(SOCKETID socketID, const QByteArray *byteArray)
{

    bool ok = false;
    Protocol ptl = m_socketInfoHash.value(socketID);
    switch (ptl) {
    case TCP:
        Q_ASSERT(m_tcpServer);
        ok = m_tcpServer->sendData(socketID, byteArray);
        m_lastErrorString = m_tcpServer->socketErrorString(socketID);
        break;
    case ENET:
        Q_ASSERT(m_enetProtocol);
        ok = m_enetProtocol->sendData(socketID, byteArray);
        m_lastErrorString = m_enetProtocol->errorString();
        break;

#ifdef UDT_ENABLED
    case UDT:
        Q_ASSERT(m_udtProtocol);
        ok = m_udtProtocol->sendData(socketID, byteArray);
        m_lastErrorString = m_udtProtocol->getLastErrorMessage();
        break;
#endif

    default:
        break;
    }

//    if(m_udtProtocol->isSocketExist(socketID)){
//        ok = m_udtProtocol->sendData(socketID, byteArray);
//        m_lastErrorString = m_udtProtocol->getLastErrorMessage();
//    }else{
//        ok = m_tcpServer->sendData(socketID, byteArray);
//        m_lastErrorString = m_tcpServer->socketErrorString(socketID);
//    }

    return ok;

}

void RTP::tcpPeerConnected(SOCKETID socketID, const QString &address, quint16 port)
{
    qDebug() << "RTP::tcpPeerConnected(...)";

    if(m_socketInfoHash.contains(socketID)) {
        Protocol ptl = m_socketInfoHash.value(socketID);
        if(ptl != TCP) {
            m_tcpServer->disconnectFromHost(socketID);
        }
    } else {
        m_socketInfoHash.insert(socketID, TCP);
    }

}

void RTP::enetPeerConnected(SOCKETID socketID, const QString &address, quint16 port)
{
    qDebug() << "RTP::enetPeerConnected(...)";

    if(m_socketInfoHash.contains(socketID)) {
        Protocol ptl = m_socketInfoHash.value(socketID);
        if(ptl != ENET) {
            m_enetProtocol->disconnect(socketID);
        }
    } else {
        m_socketInfoHash.insert(socketID, ENET);
    }

}

#ifdef UDT_ENABLED
void RTP::udtPeerConnected(SOCKETID socketID, const QString &address, quint16 port)
{
    qDebug() << "RTP::udtPeerConnected(...)";

    if(m_socketInfoHash.contains(socketID)) {
        Protocol ptl = m_socketInfoHash.value(socketID);
        if(ptl != UDT) {
            m_udtProtocol->closeSocket(socketID);
        }
    } else {
        m_socketInfoHash.insert(socketID, UDT);
    }

}
#endif



} //namespace HEHUI
