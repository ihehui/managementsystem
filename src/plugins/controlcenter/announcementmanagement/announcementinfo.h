#ifndef ANNOUNCEMENTINFO_H
#define ANNOUNCEMENTINFO_H

#include <QObject>


namespace HEHUI {

class AnnouncementTarget : public QObject
{
    Q_OBJECT
public:
    explicit AnnouncementTarget(QObject *parent = 0);


public:
    QString ID;
    QString AnnouncementID;
    QString AssetNO;
    QString UserName;
    bool Acknowledged;
    QString ACKTime;

};



class AnnouncementInfo : public QObject
{
    Q_OBJECT
public:
    explicit AnnouncementInfo(QObject *parent = 0);
    ~AnnouncementInfo();


public:
    QString ID;
    quint8 Type;
    QString Content;
    bool ACKRequired;
    QString Admin;
    QString PublishDate;
    quint32 ValidityPeriod;
    quint8 TargetType;
    quint32 DisplayTimes;
    bool Active;





};

} //namespace HEHUI

#endif // ANNOUNCEMENTINFO_H
