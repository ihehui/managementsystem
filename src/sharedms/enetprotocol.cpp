
#include <QDebug>

#include "enetprotocol.h"

#include "HHSharedNetwork/hpackethandlerbase.h"

namespace HEHUI {


ENETProtocol::ENETProtocol(QObject *parent) :
    ENETProtocolBase(parent)
{


    //注册自定义类型，必须重载“<<”和“>>”
    //qRegisterMetaTypeStreamOperators<HEHUI::Packet>("HEHUI::Packet");
    Packet::registerMetaTypeStreamOperators();

}




void ENETProtocol::processReceivedData(quint32 peerID, QByteArray data){
    //qDebug()<<"--ENETProtocolTest::processReceivedData(...) "<<"peerID:"<<peerID;

    convertDataToPacket(peerID, &data);

}

inline void ENETProtocol::convertDataToPacket(quint32 peerID, QByteArray *data){

    QString ip = "";
    quint16 port = 0;
    getPeerAddressInfo(peerID, &ip, &port);

    QDataStream in(data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QVariant v;
    in >> v;
    if (v.canConvert<Packet>()){
        Packet *packet = PacketHandlerBase::getPacket();
        *packet = v.value<Packet>();
        packet->setTransmissionProtocol(TP_UDT);
        packet->setSocketID(peerID);


        packet->setPeerHostAddress(QHostAddress(ip));
        packet->setPeerHostPort(port);
//        packet->setLocalHostAddress(m_udpSocket->localAddress());
//        packet->setLocalHostPort(m_udpSocket->localPort());

//        m_packetHandlerBase->appendIncomingPacket(packet);

        emit packetReceived(packet);
    }

}














} //namespace HEHUI
