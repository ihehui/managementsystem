#ifndef ANNOUNCEMENTINFOWIDGET_H
#define ANNOUNCEMENTINFOWIDGET_H

#include <QWidget>
#include <QTableView>

#include "ui_announcementinfowidget.h"
#include "../../sharedms/announcementinfo.h"
#include "announcementinfomodel.h"

#include "networkmanager/controlcenterpacketsparser.h"

#include "../adminuser.h"

#include "HHSharedCore/JobMonitor"



namespace HEHUI
{

class AnnouncementInfoWidget : public QWidget
{
    Q_OBJECT

public:
    enum AnnouncementJobType {CreateAnnouncement, ModifyAnnouncement};
    AnnouncementInfoWidget(bool readonly = true, QWidget *parent = 0);
    ~AnnouncementInfoWidget();

    void setAnnouncementInfo(const AnnouncementInfo *info);
    void getAnnouncementInfo(AnnouncementInfo *info);
    void setReadonly(bool readonly);
    bool isReadonly();

    void setAnnouncementTargetsData(const QString &announcementID, const QByteArray &data);

    void addComputerTargets(const QStringList &targets);

signals:
    //void signalSendMessage(quint32 messageID, const QString &message, bool confirmationRequired, int validityPeriod);
    void signalCloseWidget();
    void signalAnnouncementUpdated();

private slots:
    void on_pushButtonClone_clicked();
    void on_pushButtonEdit_clicked();
    void on_pushButtonSave_clicked();

    void on_actionRefresh_triggered();
    void on_actionExport_triggered();
    void on_actionPrint_triggered();
    void on_actionAddComputer_triggered();
    void on_actionAddUser_triggered();
    void on_actionDeleteTarget_triggered();

    void slotShowCustomContextMenu(const QPoint &pos);
    void getSelectedInfo(const QModelIndex &index);
    void linkClicked(const QUrl &url);
    void jobFinished(quint32 jobID);
    quint32 newJob(AnnouncementJobType jobType, const QString &jobTitle);


    bool verifyPrivilege();

private:
    Ui::AnnouncementInfoWidgetClass ui;

    AdminUser *m_myself;

    AnnouncementTargetModel *m_model;
    AnnouncementInfo m_info;

    unsigned int m_localTempID;
    bool m_readonly;
    bool m_targetsTouched;

    JobMonitor *m_jobMonitor;
    quint32 m_jobID;


};

} //namespace HEHUI

#endif // ANNOUNCEMENT_H
