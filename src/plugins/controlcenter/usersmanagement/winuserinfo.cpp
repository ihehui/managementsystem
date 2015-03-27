#include "winuserinfo.h"


namespace HEHUI {

WinUserInfo::WinUserInfo(QObject *parent)
    :QObject(parent)
{

    accountDisabled = false;
    cannotChangePassword = false;
    accountLocked = false;
    passwordNeverExpires = false;

    lastLogonTime_t = 0;
    lastLogoffTime_t = 0;

    mustChangePassword = false;

}

WinUserInfo::~WinUserInfo()
{

}

WinUserInfo & WinUserInfo::operator= (const WinUserInfo &info){
    userName = info.userName;
    homeDir = info.homeDir;
    comment = info.comment;

    accountDisabled = info.accountDisabled;
    cannotChangePassword = info.cannotChangePassword;
    accountLocked = info.accountLocked;
    passwordNeverExpires = info.passwordNeverExpires;

    fullName = info.fullName;

    lastLogonTime_t = info.lastLogonTime_t;
    lastLogoffTime_t = info.lastLogoffTime_t;

    loggedon = info.loggedon;
    sid = info.sid;
    profile = info.profile;
    mustChangePassword = info.mustChangePassword;

    groups = info.groups;

    return *this;
}





} //namespace HEHUI
