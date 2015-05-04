#include "announcementinfo.h"
#include "../../sharedms/global_shared.h"

namespace HEHUI {

AnnouncementTarget::AnnouncementTarget(QObject *parent) : QObject(parent)
{

    ID = "";
    AnnouncementID = "";
    AssetNO = "";
    UserName = "";
    Acknowledged = false;

}


AnnouncementInfo::AnnouncementInfo(QObject *parent) : QObject(parent)
{

    ID = "";
    Type = quint8(MS::ANNOUNCEMENT_NORMAL);
    Content = "";
    ACKRequired = true;
    Admin = "";
    PublishDate = "";
    ValidityPeriod = 0;
    TargetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
    DisplayTimes = 1;
    Active = 1;

}

AnnouncementInfo::~AnnouncementInfo()
{

}


} //namespace HEHUI
