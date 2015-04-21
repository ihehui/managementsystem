#include "serverinfo.h"

namespace HEHUI {


ServerInfo::ServerInfo(const QString &ip, quint16 port)
{

    serverName = "";
    serverIP = ip;
    serverPort = port;
    version = "";

    currentState = NotTested;

}

ServerInfo::~ServerInfo()
{

}







} //namespace HEHUI
