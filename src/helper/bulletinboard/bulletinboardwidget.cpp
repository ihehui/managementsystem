
#include "bulletinboardwidget.h"

#include <QDesktopWidget>
#include <QTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QInputDialog>



namespace HEHUI
{


BulletinBoardWidget::BulletinBoardWidget(const QString &userName, QWidget *parent)
    : QWidget(parent), m_userName(userName)
{
    ui.setupUi(this);

    setWindowFlags(Qt::WindowStaysOnTopHint);

    // Get the size of screen
    QDesktopWidget *desktop = QApplication::desktop();
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


    curAnnouncementIndex = -1;
    m_curAnnouncementID = 0;

    m_settings = new QSettings("HKEY_CURRENT_USER\\Software\\HEHUI\\MS", QSettings::NativeFormat, this);
    m_settings->beginGroup("AcknowledgedAnnouncements");
    QStringList announcementIDs = m_settings->allKeys();
    foreach (QString id, announcementIDs) {
        int times = m_settings->value(id, 1).toInt();
        acknowledgedAnnouncements.insert(id.toUInt(), times);
    }
    m_settings->endGroup();

}

BulletinBoardWidget::~BulletinBoardWidget()
{

    //qWarning()<<"BulletinBoardWidget::~BulletinBoardWidget()";

    clearAnnouncements();

    delete m_settings;

}

void BulletinBoardWidget::closeEvent(QCloseEvent *event)
{

    foreach (AnnouncementInfo *info, infolist) {
        if(acknowledgedAnnouncements.contains(info->ID)) {}
        delete info;
        infolist.removeAll(info);
    }

    curAnnouncementIndex = 0;
    if(infolist.size()) {
        showAnnouncements();
    }

    event->accept();

}

void BulletinBoardWidget::showAnnouncements(unsigned int announcementID)
{
    qDebug() << "--BulletinBoardWidget::showAnnouncements(...)";

    for(int i = 0; i < infolist.size(); i++) {
        AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
        if(!info) {
            continue;
        }
        if(info->ID == announcementID) {
            curAnnouncementIndex = i;
            showAnnouncements();
            return;
        }
    }

}

void BulletinBoardWidget::showAnnouncements()
{
    int totalCount = infolist.size();

    //if(index >= totalCount ){return;}
    AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
    if(!info) {
        return;
    }
    m_curAnnouncementID = info->ID;

    //ui.groupBoxMessage->setTitle(QString("%1(%2 %3)").arg(info->ID).arg(info->Admin).arg(info->PublishDate) );

    QString colorString = "style=\"background-color: #ffffff;\" ";
    QString html = "<html><head><meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"><title>Announcement</title>"
                   "<style type=\"text/css\">"
                   "table{ background-color: #b2b2b2; margin-top: 1px; margin-bottom: 1px; margin-left: 1px; margin-right: 1px; width: 100%; font-size: 16px;}"
                   "table tr{background-color: #f3f8fb;}"
                   "</style>"
                   "</head><body>"
                   ;
    html += "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"1\"  >";
    //html += QString("<tr %1><td align=\"center\" colspan=\"2\">%2</td></tr>").arg(colorString).arg(tr("Announcement"));
    //html += QString("<tr %1><td align=\"center\" colspan=\"2\">ID:%2 Admin:%3 Time:%4</td></tr>").arg(colorString).arg(info->ID).arg(info->Admin).arg(info->PublishDate);
    html += QString("<tr %1><td height=\"100%\" align=\"center\" colspan=\"2\">%2</td></tr>").arg(colorString).arg(info->Content);
    html += QString("<tr %1><td align=\"right\" colspan=\"2\">ID:%2 Admin:%3 Time:%4</td></tr>").arg(colorString).arg(info->ID).arg(info->Admin).arg(info->PublishDate);

    if(!info->Replies.isEmpty()) {
        html += QString("<tr><td align=\"center\" colspan=\"2\">%1</td></tr>").arg(tr("Replies"));
        html += info->Replies;
    }
    html += "</table></body></html>";
    ui.textBrowser->setText(html);
    qDebug() << "html:" << html;

    ui.labelCount->setText(QString::number(curAnnouncementIndex + 1) + "/" + QString::number(totalCount));

    ui.toolButtonPrevious->setEnabled(curAnnouncementIndex > 0);
    ui.toolButtonNext->setEnabled(curAnnouncementIndex < (totalCount - 1));

}

void BulletinBoardWidget::processAnnouncementReplies(const QByteArray &infoData)
{

    if(infoData.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    unsigned int announcementID = object["AnnouncementID"].toInt();
    if(isAnnouncementInfoExists(announcementID)) {
        return;
    }
    AnnouncementInfo *info = getAnnouncementInfo(announcementID);
    if(!info) {
        return;
    }

    QJsonArray jsonArray = object["AnnouncementReplies"].toArray();
    for(int i = 0; i < jsonArray.size(); i++) {
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10) {
            qCritical() << "ERROR! Invalid JSON array.";
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

        QString html = QString("<tr><td valign=\"middle\">%1<p>%2</p></td><td>%3</td></tr>").arg(replyinfo.Sender).arg(replyinfo.PublishTime).arg(replyinfo.Message);
        info->Replies += html;

    }

    showAnnouncements(announcementID);

}

bool BulletinBoardWidget::processAnnouncementsInfo(const QByteArray &infoData)
{
    qDebug() << "--BulletinBoardWidget::processAnnouncementsInfo(...)";

    if(infoData.isEmpty()) {
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return false;
    }
    QJsonObject object = doc.object();
    QJsonArray jsonArray = object["Announcements"].toArray();

    for(int i = 0; i < jsonArray.size(); i++) {
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }

        int index = 0;
        unsigned int id = infoArray.at(index++).toInt();
        if(isAnnouncementInfoExists(id)) {
            continue;
        }

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

        if(acknowledgedAnnouncements.contains(id)) {
            if(info->DisplayTimes > 0 && (info->DisplayTimes <= acknowledgedAnnouncements.value(id, 1)) ) {
                delete info;
                continue;
            }
        } else {
            acknowledgedAnnouncements.insert(id, 0);
        }

        if(!info->ACKRequired) {
            saveAnnouncementInfo(id);
        }

        infolist.append(info);
    }

    if(!infolist.size()) {
        return false;
    }

    curAnnouncementIndex = infolist.size() - 1;
    showAnnouncements();

    return true;
}

void BulletinBoardWidget::clearAnnouncements()
{
    foreach (AnnouncementInfo *info, infolist) {
        delete info;
    }
    infolist.clear();
}

void BulletinBoardWidget::saveAnnouncementInfo(unsigned int announcementID)
{

    m_settings->beginGroup("AcknowledgedAnnouncements");
    if(!acknowledgedAnnouncements.contains(announcementID)) {
        return;
    }
    m_settings->setValue(QString::number(announcementID), (acknowledgedAnnouncements.value(announcementID) + 1) );
    m_settings->endGroup();

}

bool BulletinBoardWidget::isAnnouncementInfoExists(unsigned int announcementID)
{
    foreach (AnnouncementInfo *info, infolist) {
        if(info->ID == announcementID) {
            return true;
        }
    }

    return false;
}

AnnouncementInfo *BulletinBoardWidget::getAnnouncementInfo(unsigned int announcementID)
{
    foreach (AnnouncementInfo *info, infolist) {
        if(info->ID == announcementID) {
            return info;
        }
    }

    return 0;
}

void BulletinBoardWidget::on_toolButtonPrevious_clicked()
{
    if(curAnnouncementIndex == 0) {
        return;
    }
    curAnnouncementIndex--;
    showAnnouncements();
}

void BulletinBoardWidget::on_toolButtonNext_clicked()
{
    if(curAnnouncementIndex == (infolist.size() - 1 )) {
        return;
    }
    curAnnouncementIndex++;
    showAnnouncements();
}

void BulletinBoardWidget::on_pushButtonReply_clicked()
{

    bool ok;
    QString reply = QInputDialog::getMultiLineText(this, tr("Reply"),
                    tr("Message:"), "", &ok);
    if (!ok || reply.isEmpty()) {
        return;
    }

    emit sendReplyMessage(m_curAnnouncementID, reply);

    AnnouncementInfo *info = infolist.at(curAnnouncementIndex);
    if(!info) {
        return;
    }

    //QString remark = QString(" <p align=\"center\"><span style=\" font-size:9pt;color:#068ec8;\">-- Reply message sent at %1 --</span></p> ").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    //QString remark = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(m_userName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    //info->Replies = remark + reply;

    QString html = QString("<tr><td valign=\"middle\">%1<p>%2</p></td><td>%3</td></tr>").arg(m_userName).arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")).arg(reply);
    info->Replies += html;


    showAnnouncements();

}

void BulletinBoardWidget::on_pushButtonACK_clicked()
{
    saveAnnouncementInfo(m_curAnnouncementID);
}















} //namespace HEHUI


