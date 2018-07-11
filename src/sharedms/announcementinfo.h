#ifndef ANNOUNCEMENTINFO_H
#define ANNOUNCEMENTINFO_H

#include <QObject>

#define URLScheme_Reply "Reply"

#include "sharedmslib.h"

namespace HEHUI
{

class SHAREDMSLIB_API AnnouncementTarget
{

public:
    explicit AnnouncementTarget();

public:
    QString ID;
    unsigned int AnnouncementID;
    QString AssetNO;
    QString UserName;
    bool Acknowledged;
    QString ACKTime;

};
/////////////////////////////////////


class SHAREDMSLIB_API AnnouncementReply
{

public:
    explicit AnnouncementReply();

public:
    QString ID;
    unsigned int AnnouncementID;
    QString Sender;
    QString SendersAssetNO;
    QString Receiver;
    QString ReceiversAssetNO;
    QString Message;
    QString PublishTime;

};
/////////////////////////////////////


class SHAREDMSLIB_API AnnouncementInfo
{
public:
    explicit AnnouncementInfo();
    ~AnnouncementInfo();


public:
    unsigned int ID;
    quint8 Type;
    QString Content;
    bool ACKRequired;
    QString Admin;
    QString PublishDate;
    quint32 ValidityPeriod;
    quint8 TargetType;
    quint32 DisplayTimes;
    bool Active;
    QString Targets;

    unsigned int TempID;

    QString Replies;





};

} //namespace HEHUI

#endif // ANNOUNCEMENTINFO_H
