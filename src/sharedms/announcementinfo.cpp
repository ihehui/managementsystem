#include "announcementinfo.h"
#include "global_shared.h"

namespace HEHUI {

AnnouncementTarget::AnnouncementTarget()
{

    ID = "";
    AnnouncementID = "";
    AssetNO = "";
    UserName = "";
    Acknowledged = false;

}
/////////////////////////////////////


AnnouncementReply::AnnouncementReply()
{

    ID = "";
    AnnouncementID = "";
    Sender = "";
    Receiver = "";
    Message = "";
    PublishTime = "";

}
/////////////////////////////////////


AnnouncementInfo::AnnouncementInfo()
{

    ID = "";
    Type = quint8(MS::ANNOUNCEMENT_NORMAL);
    Content = "";
    ACKRequired = true;
    Admin = "";
    PublishDate = "";
    ValidityPeriod = 30;
    TargetType = quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
    DisplayTimes = 1;
    Active = 1;
    Targets = "";

    TempID = 0;

}

AnnouncementInfo::~AnnouncementInfo()
{

}




} //namespace HEHUI
