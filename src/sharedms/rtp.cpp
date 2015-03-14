
#include <QDebug>
#include "rtp.h"

#include "global_shared.h"

namespace HEHUI {


RTP::RTP(QObject *parent) :
    QObject(parent)
{

//    m_udtProtocol = 0;
    m_udtProtocol = new UDTProtocol(true, 0, this);
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SIGNAL(disconnected(int)));

    m_tcpServer = 0;
//    m_tcpServer = new TCPServer(this);
//    connect(m_tcpServer, SIGNAL(disconnected(int)), this, SIGNAL(disconnected(int)));

    m_enetProtocol = 0;
//    m_enetProtocol = new ENETProtocol(this);
//    connect(m_enetProtocol, SIGNAL(connected(quint32, const QString &, quint16)), this, SLOT(connected(quint32, const QString &, quint16)));
//    connect(m_enetProtocol, SIGNAL(disconnected(int, const QString &, quint16)), this, SIGNAL(disconnected(int, const QString &, quint16)));


}

RTP::~RTP(){

    if(m_udtProtocol){
        //m_udtProtocol->close();
        delete m_udtProtocol;
        m_udtProtocol = 0;
    }

    if(m_tcpServer){
        m_tcpServer->closeServer();
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    if(m_enetProtocol){
        m_enetProtocol->close();
        delete m_enetProtocol;
        m_enetProtocol = 0;
    }

}

void RTP::startServers(const QHostAddress &localAddress, quint16 localPort, bool tryOtherPort, QString *errorMessage){

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

    if(errorMessage){
        *errorMessage = err;
    }


}

void RTP::stopServers(){
    if(m_udtProtocol){
        m_udtProtocol->close();
    }

    if(m_tcpServer){
        m_tcpServer->closeServer();
    }

}

UDTProtocol * RTP::startUDTProtocol(const QHostAddress &localAddress, quint16 localPort, bool tryOtherPort, QString *errorMessage){

    if(!m_udtProtocol){
        m_udtProtocol = new UDTProtocol(true, 0, this);
    }

    UDTSOCKET socket = m_udtProtocol->listen(localPort, localAddress);
    if(socket == UDTProtocolBase::INVALID_UDT_SOCK && tryOtherPort){
        socket = m_udtProtocol->listen();
    }

    if(socket == UDTProtocolBase::INVALID_UDT_SOCK){
        if(errorMessage){
            *errorMessage = m_udtProtocol->getLastErrorMessage();
        }
        delete m_udtProtocol;
        m_udtProtocol = 0;
        return 0;
    }

    return m_udtProtocol;

}

quint16 RTP::getUDTServerPort(){
    return m_udtProtocol->getUDTListeningPort();
}

TCPServer * RTP::startTCPServer(const QHostAddress &address, quint16 port, bool tryOtherPort, QString *errorMessage){

    if(!m_tcpServer){
        m_tcpServer = new TCPServer(this);
        connect(m_tcpServer, SIGNAL(disconnected(int)), this, SIGNAL(disconnected(int)));
    }

    if( (!m_tcpServer->listen(address, port)) && tryOtherPort){
        m_tcpServer->listen();
    }

    if(!m_tcpServer->isListening()){
        if(errorMessage){
            *errorMessage = m_tcpServer->errorString();
            qCritical()<<*errorMessage;
        }
        delete m_tcpServer;
        m_tcpServer = 0;
    }

    qDebug()<<"TCP listening on port "<<port;

    return m_tcpServer;

}

quint16 RTP::getTCPServerPort(){
    quint16 port;
    m_tcpServer->serverAddressInfo(0, &port);
    return port;
}

ENETProtocol * RTP::startENETProtocol(const QHostAddress &address, quint16 port, bool tryOtherPort, QString *errorMessage){

    if(!m_enetProtocol){
        m_enetProtocol = new ENETProtocol(this);
        connect(m_enetProtocol, SIGNAL(disconnected(quint32, const QString &, quint16)), this, SIGNAL(disconnected(quint32, const QString &, quint16)));
    }

    if( (!m_enetProtocol->listen(port, address)) && tryOtherPort){
        m_enetProtocol->listen(0);
    }

    if(!m_enetProtocol->isListening()){
        if(errorMessage){
            *errorMessage = m_enetProtocol->errorString();
            qCritical()<<*errorMessage;
        }
        delete m_enetProtocol;
        m_enetProtocol = 0;
    }

    m_enetProtocol->startWaitingForIOInAnotherThread(50);


    qDebug()<<"ENET listening on port "<<port;
    return m_enetProtocol;

}

quint16 RTP::getENETProtocolPort(){
    quint16 port;
    m_enetProtocol->getLocalListeningAddressInfo(0, &port);
    return port;
}


int RTP::connectToHost( const QHostAddress & hostAddress, quint16 port, int waitMsecs, QString *errorMessage, Protocol protocol){
    qDebug()<<"-------------------Protocol:"<<protocol;
    quint32 socketID = INVALID_SOCK_ID;
    QString err;
    bool connected = false;

    switch (protocol) {
    case AUTO:
    {
        socketID = connectToHost(hostAddress, port, waitMsecs, &err, ENET);
        if(INVALID_SOCK_ID == socketID){
            socketID = connectToHost(hostAddress, port, waitMsecs, &err, TCP);
        }
        if(INVALID_SOCK_ID != socketID){
            connected = true;
            return socketID;
        }
    }
        break;
    case TCP:
    {
        if(!m_tcpServer){
            err = tr("TCP server not running.");
            break;
        }
        socketID = m_tcpServer->connectToHost(hostAddress, port, waitMsecs);
        if(!m_tcpServer->isConnected(socketID) ){
            err += tr("\nCan not connect to host %1:%2 via TCP! %3").arg(hostAddress.toString()).arg(port).arg(m_tcpServer->socketErrorString(socketID));
            qCritical()<<err;
            m_tcpServer->abort(socketID);
            socketID = INVALID_SOCK_ID;
        }else{
            connected = true;
        }
    }
        break;
    case ENET:
    {
        if(!m_enetProtocol){
            err = tr("ENET not running.");
            break;
        }
        if(!m_enetProtocol->connectToHost(hostAddress, port, &socketID, waitMsecs)){
            err += tr("\nCan not connect to host %1:%2 via ENET! %3").arg(hostAddress.toString()).arg(port).arg(m_enetProtocol->errorString());
            qCritical()<<err;
        }else{
            connected = true;
        }

    }
        break;
    case UDT:
    {
        if(!m_udtProtocol){
            err = tr("UDT not running.");
            break;
        }
        socketID = m_udtProtocol->connectToHost(hostAddress, port, 0, true, waitMsecs);
        if( (socketID == INVALID_SOCK_ID) || (!m_udtProtocol->isSocketConnected(socketID)) ){
            err += tr("Can not connect to host %1:%2 via UDT! %3").arg(hostAddress.toString()).arg(port).arg(m_udtProtocol->getLastErrorMessage());
            qCritical()<<err;
        }else{
            connected = true;
            qDebug()<<QString("Peer %1:%2 connected via UDT! ").arg(hostAddress.toString()).arg(port);
        }
    }
        break;
    default:
        break;
    }

    if(errorMessage){
        *errorMessage = err;
    }

    if(connected){
        m_socketInfoHash.insert(socketID, protocol);
    }

    return socketID;

}

void RTP::closeSocket(int socketID){
    qDebug()<<"--RTP::closeSocket(...)";

    m_udtProtocol->closeSocket(socketID);
    m_tcpServer->disconnectFromHost(socketID);
    m_enetProtocol->close();

}

bool RTP::isSocketConnected(int socketID){

    bool connected = false;
    if(m_udtProtocol->isSocketExist(socketID)){
        connected = m_udtProtocol->isSocketConnected(socketID);
    }else{
        connected = m_tcpServer->isConnected(socketID);
    }

    return connected;

}

bool RTP::getAddressInfoFromSocket(int socketID, QString *address, quint16 *port, bool getPeerInfo){

    if(m_udtProtocol->isSocketExist(socketID)){
        return m_udtProtocol->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
    }else{
        return m_tcpServer->getAddressInfoFromSocket(socketID, address, port, getPeerInfo);
    }

}

bool RTP::isUDTSocket(int socketID){
    return m_udtProtocol->isSocketExist(socketID);
}


bool RTP::sendReliableData(int socketID, const QByteArray *byteArray){

    bool ok = false;
    Protocol ptl = m_socketInfoHash.value(socketID);
    switch (ptl) {
    case TCP:
        ok = m_tcpServer->sendData(socketID, byteArray);
        m_lastErrorString = m_tcpServer->socketErrorString(socketID);
        break;
    case ENET:
        ok = m_enetProtocol->sendData(socketID, byteArray);
        m_lastErrorString = m_enetProtocol->errorString();
        break;
    case UDT:
        ok = m_udtProtocol->sendData(socketID, byteArray);
        m_lastErrorString = m_udtProtocol->getLastErrorMessage();
        break;
    default:
        break;
    }

    if(m_udtProtocol->isSocketExist(socketID)){
        ok = m_udtProtocol->sendData(socketID, byteArray);
        m_lastErrorString = m_udtProtocol->getLastErrorMessage();
    }else{
        ok = m_tcpServer->sendData(socketID, byteArray);
        m_lastErrorString = m_tcpServer->socketErrorString(socketID);
    }

    return ok;

}




} //namespace HEHUI
