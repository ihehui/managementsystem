#include "processmonitorrule.h"


namespace HEHUI {

ProcessMonitorRule::ProcessMonitorRule(QObject *parent)
    :QObject(parent)
{

    hashRule = true;
    blacklistRule = true;
    globalRule = true;

}

ProcessMonitorRule::~ProcessMonitorRule()
{

}

ProcessMonitorRule & ProcessMonitorRule::operator= (const ProcessMonitorRule &info){
    ruleString = info.ruleString;
    comment = info.comment;

    hashRule = info.hashRule;
    blacklistRule = info.blacklistRule;
    globalRule = info.globalRule;

    return *this;
}





} //namespace HEHUI
