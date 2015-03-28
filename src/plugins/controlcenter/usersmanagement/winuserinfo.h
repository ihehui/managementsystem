#ifndef WINUSERINFO_H_
#define WINUSERINFO_H_


#include <QObject>
#include <QString>

namespace HEHUI {


class WinUserInfo : public QObject
{

public:
    WinUserInfo(QObject *parent = 0);
    ~WinUserInfo();

    WinUserInfo & operator= (const WinUserInfo &info);


public:
    QString userName;
    QString homeDir;
    QString comment;

    bool accountDisabled;
    bool cannotChangePassword;
    bool accountLocked;
    bool passwordNeverExpires;

    QString fullName;

    unsigned long lastLogonTime_t;
    unsigned long lastLogoffTime_t;

    QString sid;
    QString profile;
    bool mustChangePassword;

    QString groups;
    bool loggedon;






};

} //namespace HEHUI

#endif // WINUSERINFO_H_
