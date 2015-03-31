#ifndef ANNOUNCEMENT_H
#define ANNOUNCEMENT_H

#include <QWidget>
#include <QTableView>

#include "ui_announcement.h"

#include "networkmanager/controlcenterpacketsparser.h"



namespace HEHUI {

class Announcement : public QWidget
{
    Q_OBJECT

public:
    Announcement(QWidget *parent = 0);
    ~Announcement();

    void getMessageInfo(quint32 *messageID, QString *message, bool *confirmationRequired, int *validityPeriod);
    
signals:
    //void signalSendMessage(quint32 messageID, const QString &message, bool confirmationRequired, int validityPeriod);
    void signalCloseWidget();

private slots:
    void on_pushButtonSend_clicked();
    

    
    
private:
    Ui::AnnouncementClass ui;
    

    
};

} //namespace HEHUI 

#endif // ANNOUNCEMENT_H
