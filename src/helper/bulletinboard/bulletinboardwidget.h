#ifndef BULLETINBOARD_H
#define BULLETINBOARD_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QSettings>


#include "ui_bulletinboardwidget.h"

#include "../../sharedms/announcementinfo.h"



namespace HEHUI {

class BulletinBoardWidget : public QWidget
{
    Q_OBJECT

public:
    BulletinBoardWidget(const QString &userName, QWidget *parent = 0);
    ~BulletinBoardWidget();

    void processAnnouncementsInfo(const QByteArray &infoData);
    void showAnnouncements();

    void deleteAnnouncementsInfo(const QString &announcementID);

protected:
    void closeEvent(QCloseEvent *event);


signals:
    void sendReplyMessage(quint32 originalMessageID, const QString &replyMessage);

private slots:
    void clearAnnouncements();
    void saveAnnouncementInfo(const QString &announcementID);
    bool isAnnouncementInfoExists(const QString &announcementID);


    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();
    void on_pushButtonReply_clicked();
    void on_pushButtonACK_clicked();


private:
    Ui::SystemManagementWidgetClass ui;

    QString m_userName;
    
    int curAnnouncementIndex;
    QString m_curAnnouncementID;

    QSettings *m_settings;

    QList<AnnouncementInfo *> infolist;

    QHash<QString /*Announcement ID*/, int /*times*/> acknowledgedAnnouncements;


};

} //namespace HEHUI

#endif // BULLETINBOARD_H
