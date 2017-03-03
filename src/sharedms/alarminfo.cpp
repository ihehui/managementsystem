#include "alarminfo.h"
#include "global_shared.h"

namespace HEHUI
{


AlarmInfo::AlarmInfo()
{

    ID = "0";
    AssetNO = "";
    AlarmType = quint8(MS::ALARM_UNKNOWN);
    Message = "";
    UpdateTime = "";
    Acknowledged = 0;
    AcknowledgedBy = "";
    AcknowledgedTime = "";

}

AlarmInfo::~AlarmInfo()
{

}






} //namespace HEHUI
