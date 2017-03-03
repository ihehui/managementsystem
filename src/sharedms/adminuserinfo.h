#ifndef ADMINUSERINFO_H
#define ADMINUSERINFO_H

#include <QObject>

#include "sharedmslib.h"

#include "HHSharedCore/huserbase.h"



namespace HEHUI
{


class SHAREDMSLIB_API AdminUserInfo : public UserBase
{
public:
    AdminUserInfo(const QString &adminUserID = "", QObject *parent = 0);
    ~AdminUserInfo();

    void setJsonData(const QByteArray &data);
    QByteArray getJsonData(bool withPassword = false);

public:
    QString businessAddress;
    QString lastLoginIP;
    QString lastLoginPC;
    QString lastLoginTime;
    bool readonly;
    bool active;
    QString remark;

    unsigned int socketID;


};

} //namespace HEHUI

#endif // ADMINUSERINFO_H
