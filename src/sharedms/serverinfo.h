#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>

#include "sharedmslib.h"


namespace HEHUI
{


class SHAREDMSLIB_API ServerInfo
{

public:
    enum State {NotTested, Testing, TestOK, TestFailed};

    explicit ServerInfo(const QString &ip, quint16 port);
    ~ServerInfo();

public:

    QString serverName;
    QString serverIP;
    quint16 serverPort;
    QString version;

    State currentState;

};

} //namespace HEHUI

#endif // SERVERINFO_H
