#ifndef BULLETINBOARD_H
#define BULLETINBOARD_H

#include <QWidget>
#include <QCloseEvent>
#include <QMenu>
#include <QSettings>


#include "ui_bulletinboardwidget.h"

#include "../../sharedms/announcementinfo.h"



namespace HEHUI
{

class BulletinBoardWidget : public QWidget
{
    Q_OBJECT

public:
    BulletinBoardWidget(const QString &userName, QWidget *parent = 0);
    ~BulletinBoardWidget();

    void showAnnouncements(unsigned int announcementID);
    void showAnnouncements();
    bool processAnnouncementsInfo(const QByteArray &infoData);
    void processAnnouncementReplies(const QByteArray &infoData);

//    void deleteAnnouncementsInfo(unsigned int announcementID);

protected:
    void closeEvent(QCloseEvent *event);


signals:
    void sendReplyMessage(unsigned int originalMessageID, const QString &replyMessage);

private slots:
    void clearAnnouncements();
    void saveAnnouncementInfo(unsigned int announcementID);
    bool isAnnouncementInfoExists(unsigned int announcementID);
    AnnouncementInfo *getAnnouncementInfo(unsigned int announcementID);


    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();
    void on_pushButtonReply_clicked();
    void on_pushButtonACK_clicked();


private:
    Ui::SystemManagementWidgetClass ui;

    QString m_userName;

    int curAnnouncementIndex;
    unsigned int m_curAnnouncementID;

    QSettings *m_settings;

    QList<AnnouncementInfo *> infolist;

    QHash<unsigned int /*Announcement ID*/, int /*times*/> acknowledgedAnnouncements;


};

} //namespace HEHUI

#endif // BULLETINBOARD_H
