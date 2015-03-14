#ifndef RTP_H
#define RTP_H

#include <QObject>

#include <limits>

#include "sharedmslib.h"
#include "udtprotocol.h"
#include "tcpserver.h"
#include "enetprotocol.h"


namespace HEHUI {


class SHAREDMSLIB_API RTP : public QObject
{
    Q_OBJECT
public:
    enum Protocol{AUTO = 0, TCP, ENET, UDT};

    explicit RTP(QObject *parent = 0);
    ~RTP();
    
    void startServers(const QHostAddress &localAddress = QHostAddress::Any, quint16 localPort = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    void stopServers();

    UDTProtocol * getUDTProtocol(){return m_udtProtocol;}
    UDTProtocol * startUDTProtocol(const QHostAddress &localAddress = QHostAddress::Any, quint16 localPort = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    quint16 getUDTServerPort();

    TCPServer * getTCPServer(){return m_tcpServer;}
    TCPServer * startTCPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    quint16 getTCPServerPort();

    ENETProtocol * getENETProtocol(){return m_enetProtocol;}
    ENETProtocol * startENETProtocol(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    quint16 getENETProtocolPort();

    int connectToHost( const QHostAddress &hostAddress, quint16 port, int waitMsecs = 0, QString *errorMessage = 0, Protocol protocol= AUTO);
    void closeSocket(int socketID);
    bool isSocketConnected(int socketID);
    bool getAddressInfoFromSocket(int socketID, QString *address, quint16 *port, bool getPeerInfo = true);
    bool isUDTSocket(int socketID);

    bool sendReliableData(int socketID, const QByteArray *byteArray);
    QString lastErrorString(){return m_lastErrorString;}

signals:
//    void connected (int socketID, const QString &peerAddress, quint16 peerPort);
    void disconnected (quint32 socketID/*, const QString &peerAddress, quint16 peerPort*/);

    
public slots:


private:

    QHash<int /*socketID*/, Protocol> m_socketInfoHash;

    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QString m_lastErrorString;
    
};

} //namespace HEHUI

#endif // RTP_H
