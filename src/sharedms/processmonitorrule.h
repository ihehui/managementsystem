#ifndef PROCESSMONITORRULE_H_
#define PROCESSMONITORRULE_H_


#include <QObject>
#include <QString>

#include "sharedmslib.h"



namespace HEHUI {


class SHAREDMSLIB_API ProcessMonitorRule : public QObject
{

public:
    ProcessMonitorRule(QObject *parent = 0);
    ~ProcessMonitorRule();

    ProcessMonitorRule & operator= (const ProcessMonitorRule &info);


public:
    QString ruleString;
    QString comment;

    bool hashRule;
    bool blacklistRule;

    bool globalRule;


};

} //namespace HEHUI

#endif // PROCESSMONITORRULE_H_
