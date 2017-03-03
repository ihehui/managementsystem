#ifndef ALARMINFO_H
#define ALARMINFO_H

#include <QObject>

#include "sharedmslib.h"

namespace HEHUI
{


class SHAREDMSLIB_API AlarmInfo
{
public:
    AlarmInfo();
    ~AlarmInfo();


public:
    QString ID;
    QString AssetNO;
    quint8 AlarmType;
    QString Message;
    QString UpdateTime;
    quint8 Acknowledged;
    QString AcknowledgedBy;
    QString AcknowledgedTime;



};

} //namespace HEHUI

#endif // ALARMINFO_H
