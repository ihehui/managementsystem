
#include "bulletinboardwidget.h"

#include <QDesktopWidget>
#include <QTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>



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
    m_curAnnouncementID = "";

    m_settings = new QSettings("HKEY_CURRENT_USER\\Software\\HEHUI\\MS", QSettings::NativeFormat, this);
    m_settings->beginGroup("AcknowledgedAnnouncements");
    QStringList announcementIDs = m_settings->allKeys();
    foreach (QString id, announcementIDs) {
        int times = m_settings->value(id, 1).toInt();
        acknowledgedAnnouncements.insert(id, times);
    }
    m_settings->endGroup();

}

BulletinBoardWidget::~BulletinBoardWidget()
{

    //qWarning()<<"BulletinBoardWidget::~BulletinBoardWidget()";

    clearAnnouncements();

    delete m_settings;

}

void BulletinBoardWidget::closeEvent(QCloseEvent *event){

    foreach (AnnouncementInfo *info, infolist) {
        if(acknowledgedAnnouncements.contains(info->ID)){}
        delete info;
        infolist.removeAll(info);
    }

    curAnnouncementIndex = 0;
    if(infolist.size()){
        showAnnouncements();
    }

    event->accept();
    
}

void BulletinBoardWidget::showAnnouncements(const QString &announcementID){

    for(int i=0; i<infolist.size(); i++){
        AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
        if(!info){continue;}
        if(info->ID == announcementID){
            curAnnouncementIndex = i;
            showAnnouncements();
            return;
        }
    }

}

void BulletinBoardWidget::showAnnouncements(){
    int totalCount = infolist.size();

    //if(index >= totalCount ){return;}
    AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
    if(!info){return;}
    m_curAnnouncementID = info->ID;

    QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(info->Admin).arg(info->PublishDate);
    QString msg = remark + info->Content;
    if(!info->Replies.isEmpty()){
        msg += QString(" <p align=\"center\"><span style=\" font-size:9pt;color:#068ec8;\">---- Replies ----</span></p> ");
        msg += info->Replies;
    }

    ui.textBrowser->setText(msg);
    ui.labelCount->setText(QString::number(curAnnouncementIndex+1)+"/"+QString::number(totalCount));

    ui.toolButtonPrevious->setEnabled(curAnnouncementIndex>0);
    ui.toolButtonNext->setEnabled(curAnnouncementIndex<(totalCount-1));

}

void BulletinBoardWidget::processAnnouncementReplies(const QByteArray &infoData){

    if(infoData.isEmpty()){return;}

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QString announcementID = object["AnnouncementID"].toString();
    if(isAnnouncementInfoExists(announcementID)){return;}
    AnnouncementInfo *info = getAnnouncementInfo(announcementID);
    if(!info){return;}

    QJsonArray jsonArray = object["AnnouncementReplies"].toArray();
    for(int i=0;i<jsonArray.size(); i++){
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10){
            qCritical()<<"ERROR! Invalid JSON array.";
            continue;
        }

        int index = 0;
        QString id = infoArray.at(index++).toString();
        AnnouncementReply replyinfo;
        replyinfo.ID = id;
        replyinfo.AnnouncementID = infoArray.at(index++).toString().toUShort();
        replyinfo.Sender = infoArray.at(index++).toString();
        replyinfo.SendersAssetNO = infoArray.at(index++).toString();
        replyinfo.Receiver = infoArray.at(index++).toString();
        replyinfo.ReceiversAssetNO = infoArray.at(index++).toString();
        replyinfo.Message = infoArray.at(index++).toString();
        replyinfo.PublishTime = infoArray.at(index++).toString();

        QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(replyinfo.Sender).arg(replyinfo.PublishTime);
        info->Replies += remark + replyinfo.Message;

    }

    showAnnouncements(announcementID);

}

void BulletinBoardWidget::processAnnouncementsInfo(const QByteArray &infoData){
    if(infoData.isEmpty()){return;}

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    QJsonArray jsonArray = object["Announcements"].toArray();

    for(int i=0;i<jsonArray.size(); i++){
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10){
            qCritical()<<"ERROR! Invalid JSON array.";
            continue;
        }

        int index = 0;
        QString id = infoArray.at(index++).toString();
        if(isAnnouncementInfoExists(id)){continue;}

        AnnouncementInfo *info = new AnnouncementInfo();
        info->ID = id;
        info->Type = infoArray.at(index++).toString().toUShort();
        info->Content = infoArray.at(index++).toString();
        info->ACKRequired = infoArray.at(index++).toString().toUShort();
        info->Admin = infoArray.at(index++).toString();
        info->PublishDate = infoArray.at(index++).toString();
        info->ValidityPeriod = infoArray.at(index++).toString().toUInt();
        info->TargetType = infoArray.at(index++).toString().toUShort();
        info->DisplayTimes = infoArray.at(index++).toString().toUInt();
        info->Active = infoArray.at(index++).toString().toUInt();


        if(acknowledgedAnnouncements.contains(id)){
            if(info->DisplayTimes > 0 && (info->DisplayTimes <= acknowledgedAnnouncements.value(id, 1)) ){
                delete info;
                continue;
            }
        }else{
            acknowledgedAnnouncements.insert(id, 0);
        }

        if(!info->ACKRequired){
            saveAnnouncementInfo(id);
        }

        infolist.append(info);

    }

    if(!infolist.size()){
        return;
    }

    curAnnouncementIndex = infolist.size() -1;
    showAnnouncements();

}

void BulletinBoardWidget::clearAnnouncements(){
    foreach (AnnouncementInfo *info, infolist) {
        delete info;
    }
    infolist.clear();
}

void BulletinBoardWidget::saveAnnouncementInfo(const QString &announcementID){

    m_settings->beginGroup("AcknowledgedAnnouncements");
    if(!acknowledgedAnnouncements.contains(announcementID)){return;}
    m_settings->setValue(announcementID, (acknowledgedAnnouncements.value(announcementID) + 1) );
    m_settings->endGroup();

}

bool BulletinBoardWidget::isAnnouncementInfoExists(const QString &announcementID){
    foreach (AnnouncementInfo *info, infolist) {
        if(info->ID == announcementID){return true;}
    }

    return false;
}

AnnouncementInfo * BulletinBoardWidget::getAnnouncementInfo(const QString &announcementID){
    foreach (AnnouncementInfo *info, infolist) {
        if(info->ID == announcementID){return info;}
    }

    return 0;
}

void BulletinBoardWidget::on_toolButtonPrevious_clicked(){
    if(curAnnouncementIndex == 0){return;}
    curAnnouncementIndex--;
    showAnnouncements();
}

void BulletinBoardWidget::on_toolButtonNext_clicked(){
    if(curAnnouncementIndex == (infolist.size() -1 )){return;}
    curAnnouncementIndex++;
    showAnnouncements();
}

void BulletinBoardWidget::on_pushButtonReply_clicked(){
    if(!ui.groupBoxReply->isVisible()){
        ui.groupBoxReply->setVisible(true);
        return;
    }else{
        QString reply = ui.textEditReply->toPlainText();
        emit sendReplyMessage(m_curAnnouncementID.toUInt(), reply);

        AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
        if(!info){return;}

        //QString remark = QString(" <p align=\"center\"><span style=\" font-size:9pt;color:#068ec8;\">-- Reply message sent at %1 --</span></p> ").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(m_userName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        info->Replies = remark + reply;
        showAnnouncements();
    }

}

void BulletinBoardWidget::on_pushButtonACK_clicked(){
    saveAnnouncementInfo(m_curAnnouncementID);
}















} //namespace HEHUI


