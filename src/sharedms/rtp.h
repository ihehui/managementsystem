#ifndef RTP_H
#define RTP_H

#include <QObject>

#include <limits>

#include "sharedmslib.h"
#include "tcpserver.h"
#include "enetprotocol.h"

#ifdef UDT_ENABLED
#include "udtprotocol.h"
#endif

namespace HEHUI
{


class SHAREDMSLIB_API RTP : public QObject
{
    Q_OBJECT
public:
    enum Protocol {AUTO = 0, TCP, ENET, UDT};

    explicit RTP(QObject *parent = 0);
    ~RTP();

    void startServers(const QHostAddress &localAddress = QHostAddress::Any, quint16 localPort = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    void stopServers();

#ifdef UDT_ENABLED
    //    UDTProtocol * getUDTProtocol(){return m_udtProtocol;}
        UDTProtocol *startUDTProtocol(const QHostAddress &localAddress = QHostAddress::Any, quint16 localPort = 0, bool tryOtherPort = true, QString *errorMessage = 0);
        quint16 getUDTServerPort();
#endif

    TCPServer *getTCPServer()
    {
        return m_tcpServer;
    }
    TCPServer *startTCPServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    quint16 getTCPServerPort();

    ENETProtocol *getENETProtocol()
    {
        return m_enetProtocol;
    }
    ENETProtocol *startENETProtocol(const QHostAddress &address = QHostAddress::Any, quint16 port = 0, bool tryOtherPort = true, QString *errorMessage = 0);
    quint16 getENETProtocolPort();

    SOCKETID connectToHost( const QHostAddress &hostAddress, quint16 port, int waitMsecs = 0, QString *errorMessage = 0, Protocol protocol = AUTO);
    void closeSocket(SOCKETID socketID);
    bool isSocketConnected(SOCKETID socketID);
    bool getAddressInfoFromSocket(SOCKETID socketID, QString *address, quint16 *port, bool getPeerInfo = true);

    QString socketProtocolString(SOCKETID socketID);

    bool sendReliableData(SOCKETID socketID, const QByteArray *byteArray);
    QString lastErrorString()
    {
        return m_lastErrorString;
    }

signals:
//    void connected (int socketID, const QString &peerAddress, quint16 peerPort);
    void disconnected (SOCKETID socketID/*, const QString &peerAddress, quint16 peerPort*/);

public slots:


private slots:
    void tcpPeerConnected(SOCKETID socketID, const QString &address, quint16 port);
    void enetPeerConnected(SOCKETID socketID, const QString &address, quint16 port);

#ifdef UDT_ENABLED
    void udtPeerConnected(SOCKETID socketID, const QString &address, quint16 port);
#endif


private:

    QHash<SOCKETID /*socketID*/, Protocol> m_socketInfoHash;

#ifdef UDT_ENABLED
    UDTProtocol *m_udtProtocol;
#endif

    TCPServer *m_tcpServer;
    ENETProtocol *m_enetProtocol;

    QString m_lastErrorString;

};

} //namespace HEHUI

#endif // RTP_H
