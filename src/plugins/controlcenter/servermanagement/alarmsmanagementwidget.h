#ifndef ALARMSMANAGEMENTWIDGET_H
#define ALARMSMANAGEMENTWIDGET_H

#include <QWidget>

#include "alarminfomodel.h"
#include "../adminuser.h"


namespace Ui
{
class AlarmsManagementWidget;
}

namespace HEHUI
{


class AlarmsManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmsManagementWidget(QWidget *parent = 0);
    ~AlarmsManagementWidget();

    void setData(const QByteArray &data);

private slots:
    void on_toolButtonQuery_clicked();
    void on_toolButtonFilter_clicked();
    void on_actionRefresh_triggered();
    void filter();

    void on_comboBoxPeriod_currentIndexChanged(int index);

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_actionAcknowledge_triggered();
    void on_actionDelete_triggered();
    void requestAcknowledgeAlarms(bool deleteAlarms = false);


    void slotShowCustomContextMenu(const QPoint &pos);
    void getSelectedInfo(const QModelIndex &index);

    bool verifyPrivilege();

private:
    QString typeString() const;
    QString acknowledgedString() const;
    void periodString(QString *startTime, QString *endTime);




private:
    Ui::AlarmsManagementWidget *ui;

    AlarmInfoModel *m_model;
    AlarmInfoSortFilterProxyModel *m_proxyModel;

    AdminUser *m_myself;

    QStringList m_selectedInfoList;

    enum Period {
        Period_Custom = 0,
        Period_Today,
        Period_Last_24_Hours,
        Period_Yesterday,
        Period_This_Week,
        Period_This_Month,
    };

};

} //namespace HEHUI

#endif // ALARMSMANAGEMENTWIDGET_H
