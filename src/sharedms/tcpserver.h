#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "sharedmslib.h"

#include "HHSharedNetwork/htcpbase.h"
#include "HHSharedNetwork/PacketBase"

namespace HEHUI {

class SHAREDMSLIB_API TCPServer : public TCPBase
{
    Q_OBJECT
public:
    explicit TCPServer(QObject *parent = 0);
    ~TCPServer();

    quint16 getTCPServerListeningPort();

signals:
    void packetReceived(const PacketBase &packet);



private:
    void processData(SOCKETID socketID, QByteArray *data);



    
};

}

#endif // TCPSERVER_H
