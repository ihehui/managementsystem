#ifndef ANNOUNCEMENTINFOWIDGET_H
#define ANNOUNCEMENTINFOWIDGET_H

#include <QWidget>
#include <QTableView>

#include "ui_announcementinfowidget.h"
#include "announcementinfo.h"
#include "announcementinfomodel.h"

#include "networkmanager/controlcenterpacketsparser.h"

#include "../adminuser.h"



namespace HEHUI {

class AnnouncementInfoWidget : public QWidget
{
    Q_OBJECT

public:
    AnnouncementInfoWidget(bool readonly = true, QWidget *parent = 0);
    ~AnnouncementInfoWidget();

    void setAnnouncementInfo(const AnnouncementInfo *info);
    void getAnnouncementInfo(AnnouncementInfo *info);

    void setReadonly(bool readonly);

    void setComputerTargets(const QStringList &targets);



    void getMessageInfo(quint32 *messageID, QString *message, bool *confirmationRequired, int *validityPeriod);
    
signals:
    //void signalSendMessage(quint32 messageID, const QString &message, bool confirmationRequired, int validityPeriod);
    void signalCloseWidget();

private slots:
    void on_pushButtonSend_clicked();

    void on_actionRefresh_triggered();
    
    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void slotShowCustomContextMenu(const QPoint & pos);
    void getSelectedInfo(const QModelIndex &index);

    bool verifyPrivilege();
    
private:
    Ui::AnnouncementInfoWidgetClass ui;
    
    AdminUser *m_myself;

    AnnouncementTargetModel *m_model;
    QStringList m_selectedInfoList;

    unsigned int m_localTempID;


};

} //namespace HEHUI 

#endif // ANNOUNCEMENT_H
