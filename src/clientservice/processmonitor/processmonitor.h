#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../../sharedms/processmonitorrule.h"


namespace HEHUI {



class ProcessMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ProcessMonitor(QObject *parent = 0);
    ~ProcessMonitor();

    unsigned long lastErrorCode() const;

signals:


public slots:
    bool init();
    void setRulesData(const QByteArray &jsonData);
    QJsonArray getLocalRules();

    void setBasicInfo(bool useGlobalRules, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess);

    void cleanRules(bool globalRules);

private slots:
    void monitor();



private:
    bool setupDriver();
    void cleanupDriver();




private:
    unsigned long m_errorCode;

    bool m_useGlobalRules;
    bool m_passthroughEnabled;
    bool m_logAllowedProcessEnabled;
    bool m_logBlockedProcessEnabled;

    char outputbuff[256];


    QString msg;

    QStringList md5WhiteList;
    QStringList md5BlackList;
    QStringList filePathWhiteList;
    QStringList filePathBlackList;

    QHash<QString /*Rule*/, ProcessMonitorRule*> hashRulesHash;
    QHash<QString /*Rule*/, ProcessMonitorRule*> pathRulesHash;




};

} //namespace HEHUI

#endif // PROCESSMONITOR_H
