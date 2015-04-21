#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QWidget>
#include <QEvent>

#include "../../sharedms/global_shared.h"
#include "../../sharedms/processmonitorrule.h"

#include "processmonitorrulemodel.h"


namespace Ui {
class ProcessMonitor;
}

namespace HEHUI {


class ProcessMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessMonitor(QWidget *parent = 0);
    ~ProcessMonitor();


protected:
    bool eventFilter(QObject *obj, QEvent *event);


signals:
    void signalGetProcessMonitorInfo(quint8 infoType = MS::SYSINFO_PROCESSMONITOR);
    void signalSetProcessMonitorInfo(const QByteArray &rulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO);


public slots:
    void setJsonData(const QByteArray &data);
    void rulesSaved();


private slots:
    void slotShowCustomContextMenu(const QPoint & pos);
    void getSelectedRule(const QModelIndex &index);
    void slotViewRuleInfo(const QModelIndex &index);

    bool verifyPrivilege();

    void on_actionRefresh_triggered();
    void on_actionAddFileHash_triggered();
    void on_actionAddFilePath_triggered();
    void on_actionDelete_triggered();
    void on_actionModify_triggered();

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_pushButtonApply_clicked();

    void slotExportQueryResult();
    void slotPrintQueryResult();
    bool showRuleInfo(bool hashMode, bool readonly, QString *ruleString, QString *ruleComment, bool *blacklistRule);


private:
    Ui::ProcessMonitor *ui;

    ProcessMonitorRule *m_selectedRule;

    ProcessMonitorRuleModel *m_processMonitorRuleModel;
    ProcessMonitorRuleSortFilterProxyModel *m_proxyModel;


    QString m_updatedDate;
    QString m_updatedBy;

    bool m_rulesModified;



};

} //namespace HEHUI

#endif // PROCESSMONITOR_H
