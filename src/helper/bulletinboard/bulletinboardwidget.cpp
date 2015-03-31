
#include "bulletinboardwidget.h"

#include <QDesktopWidget>
#include <QTime>
#include <QDebug>



namespace HEHUI {


BulletinBoardWidget::BulletinBoardWidget(const QString &userName, QWidget *parent)
    : QWidget(parent), m_userName(userName)
{
    ui.setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    // Get the size of screen
    QDesktopWidget* desktop = QApplication::desktop();
    QRect rect = desktop->availableGeometry(this);
    int desktopWidth = rect.width();
    int desktopHeight = rect.height();
    int windowWidth = frameGeometry().width();
    int windowHeight = frameGeometry().height();
    //move the window
    move((desktopWidth - windowWidth) / 2, (desktopHeight - windowHeight) / 2);
    //        move((desktopWidth - windowWidth), (desktopHeight - windowHeight));

    //qWarning()<<"dw:"<<desktopWidth<<" dh:"<<desktopHeight;
    //qWarning()<<"ww:"<<windowWidth<<" wh:"<<windowHeight;
    //qWarning()<<"gw:"<<geometry().width()<<" gh:"<<geometry().height();
    //qWarning()<<"fsw:"<<frameSize().width()<<" fsh:"<<frameSize().height();


    ui.groupBoxReply->setVisible(false);

    curAnnouncementIndex = -1;
    totalCount = 0;

    m_curMessageID = 0;


}

BulletinBoardWidget::~BulletinBoardWidget()
{

    //qWarning()<<"BulletinBoardWidget::~BulletinBoardWidget()";

}

void BulletinBoardWidget::closeEvent(QCloseEvent *event){

    event->accept();
    
}

void BulletinBoardWidget::showServerAnnouncement(const QString &adminName, quint32 announcementID, const QString &serverAnnouncement){

    if(announcements.contains(announcementID)){
        return;
    }

    m_curMessageID = announcementID;

    
    curAnnouncementIndex = totalCount;
    totalCount++;
    
    //    ui.textBrowser->append(QString("-------- %1 --------").arg(adminName + "  " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")));
    //    ui.textBrowser->append(QString(" <p align=\"center\"><span style=\" font-size:12pt;color:#068ec8;\">-------- %1 --------</span></p> ").arg(adminName + "  " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")));
    //        ui.textBrowser->append(QString(" <p align=\"center\" style=\" font-size:12pt;color:#068ec8;\">-------- %1 --------</p> ").arg(adminName + "  " + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")));

    //    ui.textBrowser->append(serverAnnouncement);
    //    ui.textBrowser->append("\n");
    
    //    ui.labelCount->setText(QString("%1 %2").arg(QString::number(count)).arg(count>1?tr("Messages"):tr("Message")));

    
    //QString remark = QString(" <p align=\"center\"><span style=\" font-size:9pt;color:#068ec8;\">-- Message sent by %1. Received at %2 --</span></p> ").arg(adminName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(adminName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    QString msg = remark + serverAnnouncement;
    
    ui.textBrowser->setText(msg);
    ui.labelCount->setText(QString::number(curAnnouncementIndex+1)+"/"+QString::number(totalCount));
    
    announcements.insert(announcementID, msg);
    
    
    ui.toolButtonPrevious->setEnabled(curAnnouncementIndex>0);
    ui.toolButtonNext->setEnabled(false);
    
    
}

void BulletinBoardWidget::on_toolButtonPrevious_clicked(){
    curAnnouncementIndex--;
    
    //if(curAnnouncementIndex ==0){
    ui.toolButtonPrevious->setEnabled(curAnnouncementIndex>0);
    ui.toolButtonNext->setEnabled(curAnnouncementIndex<(totalCount-1));
    //}
    
    QList<quint32> keys = announcements.keys();
    qSort(keys);

    m_curMessageID = keys.at(curAnnouncementIndex);
    ui.textBrowser->setText(announcements.value(m_curMessageID));
    ui.labelCount->setText(QString::number(curAnnouncementIndex+1)+"/"+QString::number(totalCount));

}

void BulletinBoardWidget::on_toolButtonNext_clicked(){

    curAnnouncementIndex++;
    
    ui.toolButtonPrevious->setEnabled(curAnnouncementIndex>0);
    ui.toolButtonNext->setEnabled(curAnnouncementIndex<(totalCount-1));
    
    QList<quint32> keys = announcements.keys();
    qSort(keys);
    m_curMessageID = keys.at(curAnnouncementIndex);
    ui.textBrowser->setText(announcements.value(m_curMessageID));
    ui.labelCount->setText(QString::number(curAnnouncementIndex+1)+"/"+QString::number(totalCount));
    
}

void BulletinBoardWidget::on_pushButtonReply_clicked(){
    if(!ui.groupBoxReply->isVisible()){
        ui.groupBoxReply->setVisible(true);
        return;
    }else{
        QString reply = ui.textEditReply->toPlainText();
        emit sendReplyMessage(m_curMessageID, reply);

        //QString remark = QString(" <p align=\"center\"><span style=\" font-size:9pt;color:#068ec8;\">-- Reply message sent at %1 --</span></p> ").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(m_userName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        QString msg = announcements.value(m_curMessageID) + reply + remark;
        announcements[m_curMessageID] = msg;
        ui.textBrowser->setText(msg);
        ui.textEditReply->clear();

    }



}















} //namespace HEHUI


