#ifndef ANNOUNCEMENTMANAGEMENTWIDGET_H
#define ANNOUNCEMENTMANAGEMENTWIDGET_H

#include <QWidget>

#include "announcementinfomodel.h"
#include "announcementinfowidget.h"

#include "../adminuser.h"


namespace Ui {
class AnnouncementManagementWidget;
}


namespace HEHUI {


class AnnouncementManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnouncementManagementWidget(QWidget *parent = 0);
    ~AnnouncementManagementWidget();

    void setAnnouncementsData(const QByteArray &data);
    void setAnnouncementTargetsData(const QString &announcementID, const QByteArray &data);


private slots:
    void on_toolButtonQuery_clicked();
    void on_toolButtonFilter_clicked();
    void on_actionRefresh_triggered();
    void filter();

    void on_comboBoxPeriod_currentIndexChanged(int index);

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_actionCreate_triggered();
    void on_actionClone_triggered();
    void on_actionDisable_triggered();

    void requestAcknowledgeAlarms(bool deleteAlarms = false);

    void showAnnouncementInfo(const QModelIndex &index);
    void showAnnouncementInfoWidget(AnnouncementInfo *info, bool readonly = true);

    void slotShowCustomContextMenu(const QPoint & pos);
    void getSelectedInfo(const QModelIndex &index);

    bool verifyPrivilege();

private:
    QString validityString() const;
    QString targetString() const;
    void periodString(QString *startTime, QString *endTime);



private:
    Ui::AnnouncementManagementWidget *ui;

    enum Period{
        Period_Custom = 0,
        Period_Today,
        Period_Last_24_Hours,
        Period_Yesterday,
        Period_This_Week,
        Period_This_Month,
    };

    AnnouncementInfoModel *m_model;
    SortFilterProxyModel *m_proxyModel;
    AnnouncementInfoWidget *m_infoWidget;

    AdminUser *m_myself;

    QStringList m_selectedInfoList;






};

} //namespace HEHUI

#endif // ANNOUNCEMENTMANAGEMENTWIDGET_H
