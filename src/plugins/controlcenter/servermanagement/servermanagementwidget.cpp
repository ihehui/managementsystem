#include "servermanagementwidget.h"
#include "ui_servermanagementwidget.h"

#include <QMenu>
#include <QToolButton>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>



namespace HEHUI
{


ServerManagementWidget::ServerManagementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementWidget)
{
    ui->setupUi(this);

    initTabWidget();

    installEventFilter(this);
    ui->labelTop->installEventFilter(this);
    ui->groupBoxSystemInfo->installEventFilter(this);
    ui->groupBoxResources->installEventFilter(this);
    ui->groupBoxStatistics->installEventFilter(this);

    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(verifyPrivilege()));
    connect(ui->actionAdministrators, SIGNAL(triggered()), this, SLOT(manageAdmins()));
    connect(ui->actionAlarms, SIGNAL(triggered()), this, SLOT(manageAlarms()));
    connect(ui->actionAnnouncement, SIGNAL(triggered()), this, SLOT(manageAnnouncements()));


    m_myself = AdminUser::instance();
    connect(m_myself, SIGNAL(signalVerified()), this, SLOT(adminVerified()));
    //m_socketConnectedToServer = INVALID_SOCK_ID;

    m_adminsWidget = 0;
    m_alarmsWidget = 0;
    m_announcementManagementWidget = 0;
}

ServerManagementWidget::~ServerManagementWidget()
{
    delete ui;
}

bool ServerManagementWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu ) {
        QContextMenuEvent *contextMenuEvent = static_cast<QContextMenuEvent *> (event);

        showMenu(contextMenuEvent->globalPos());

        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}


void ServerManagementWidget::closeEvent(QCloseEvent *e)
{

    //Close all related TabPage
    int tabPages = ui->tabWidgetServer->count();
    for(int i = tabPages; i >= 0; --i) {
        ui->tabWidgetServer->removeTab(i);

        QWidget *widget = qobject_cast<QWidget *>(ui->tabWidgetServer->widget(i));
        if(!widget) {
            continue;
        }
        widget->close();

    }

    e->accept();
    //deleteLater();

}

void ServerManagementWidget::setAdminsData(const QByteArray &infoData)
{
    manageAdmins();
    m_adminsWidget->setData(infoData);
}

void ServerManagementWidget::setAlarmsData(const QByteArray &infoData)
{
    manageAlarms();
    m_alarmsWidget->setData(infoData);
}

void ServerManagementWidget::setAnnouncementsData(const QByteArray &infoData)
{
    manageAnnouncements();
    m_announcementManagementWidget->setAnnouncementsData(infoData);
}

void ServerManagementWidget::setAnnouncementTargetsData(const QString &extraInfo, const QByteArray &infoData)
{
    manageAnnouncements();
    m_announcementManagementWidget->setAnnouncementTargetsData(extraInfo, infoData);
}

void ServerManagementWidget::setAnnouncementReplies(const QByteArray &infoData)
{
    manageAnnouncements();
    m_announcementManagementWidget->setAnnouncementReplies(infoData);
}

void ServerManagementWidget::updateServerInfo(const QByteArray &infoData)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QJsonObject resLoadObj = object["ServerInfo"].toObject();
    if(resLoadObj.isEmpty()) {
        return;
    }

    ui->labelVersion->setText(resLoadObj.value("Version").toString());
    ui->labelOS->setText(resLoadObj.value("OS").toString());


    ui->labelCPUType->setText(resLoadObj.value("CPU").toString());

    quint64 memory = resLoadObj.value("Memory").toString().toULong();
    memory = memory / (1024 * 1024);
    ui->labelMemory->setText(QString("%1 MB").arg(memory));

    unsigned int startupUTCTime = resLoadObj.value("StartupUTCTime").toString().toUInt();
    QDateTime time = QDateTime::fromTime_t(startupUTCTime);
    ui->labelStartupTime->setText(time.toString("yyyy-MM-dd hh:mm:ss"));

    unsigned int curServerUTCTime = resLoadObj.value("CurrentServerUTCTime").toString().toUInt();
    unsigned int curDBUTCTime = resLoadObj.value("CurrentDBUTCTime").toString().toUInt();
    QDateTime curLocalTime = QDateTime::currentDateTime();
    unsigned int curLocalUTCTime = curLocalTime.toTime_t();
    if(abs((long)curServerUTCTime - (long)curLocalUTCTime) > 600
            || abs((long)curDBUTCTime - (long)curLocalUTCTime) > 600
      ) {
        QString message = tr("The server time and local time are too different!<br>Application Server Time: %1<br>Database Server Time: %2<br>Local Time: %3")
                          .arg(QDateTime::fromTime_t(startupUTCTime).toString("yyyy-MM-dd hh:mm"))
                          .arg(QDateTime::fromTime_t(curDBUTCTime).toString("yyyy-MM-dd hh:mm"))
                          .arg(curLocalTime.toString("yyyy-MM-dd hh:mm"))
                          ;
        QMessageBox::warning(0, tr("Warning"), message);
    }

    ui->labelDatabaseServerIP->setText(resLoadObj.value("DBServerIP").toString());

    QString driver = resLoadObj.value("DBDriver").toString().toUpper();
    if(driver.startsWith("Q")) {
        driver = driver.remove(0, 1);
        ui->labelDatabaseDriver->setText(driver);
    }


}

void ServerManagementWidget::updateRealtimeInfo(const QByteArray &infoData)
{
    //qDebug()<<"--ServerManagementWidget::updateRealtimeInfo(...)";

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QJsonObject realtimeInfoObj = object["Realtime"].toObject();
    if(!realtimeInfoObj.isEmpty()) {
        ui->progressBarCPUUsage->setValue(realtimeInfoObj.value("CPULoad").toString().toUInt());
        ui->progressBarMemoryUsage->setValue(realtimeInfoObj.value("MemLoad").toString().toUInt());

        if(realtimeInfoObj.contains("Disks")) {
            QString html = convertDisksInfoToHTML(realtimeInfoObj.value("Disks").toString());
            ui->labelDiskUsage->setText(html);
        }

        uint totalClients = realtimeInfoObj.value("TotalClients").toString().toUInt();
        uint onlineClients = realtimeInfoObj.value("OnlineClients").toString().toUInt();
        QString clientsInfo = tr("Total: %1, Online: %2").arg(totalClients).arg(onlineClients);
        ui->labelClients->setText(clientsInfo);

        uint totalAlarms = realtimeInfoObj.value("TotalAlarms").toString().toUInt();
        uint unacknowledgedAlarms = realtimeInfoObj.value("UnacknowledgedAlarms").toString().toUInt();
        QString alarmsInfo = tr("Total: %1, Unacknowledged: %2").arg(totalAlarms).arg(unacknowledgedAlarms);
        ui->labelAlarms->setText(alarmsInfo);

    }

}

void ServerManagementWidget::initTabWidget()
{

    connect(ui->tabWidgetServer, SIGNAL(currentChanged(int)), this, SLOT(slotTabPageChanged()));

    QTabBar *tabBar = ui->tabWidgetServer->tabBar();
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar->shape();
    }

    QToolButton *newTabButton = new QToolButton(this);
    ui->tabWidgetServer->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/icon/resources/images/menu.png"));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(showTabMenu()));
    newTabButton->setToolTip(tr("Menu"));
    //newTabButton->setEnabled(false);

    QToolButton *closeTabButton = new QToolButton(this);
    ui->tabWidgetServer->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton->setCursor(Qt::ArrowCursor);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setIcon(QIcon(":/icon/resources/images/closetab.png"));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(slotcloseTab()));
    closeTabButton->setToolTip(tr("Close Page"));
    closeTabButton->setEnabled(false);


}

void ServerManagementWidget::slotTabPageChanged()
{

    QWidget *currentWidget = ui->tabWidgetServer->currentWidget();
    if(currentWidget == ui->tabServerInfo) {
        ui->tabWidgetServer->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
    } else {
        ui->tabWidgetServer->cornerWidget(Qt::TopRightCorner)->setEnabled(true);
    }

}

void ServerManagementWidget::slotcloseTab()
{

    QWidget *currentWidget = ui->tabWidgetServer->currentWidget();
    if(currentWidget == ui->tabServerInfo) {
        return;
    }
    if(!currentWidget->close()) {
        return;
    }
    ui->tabWidgetServer->removeTab(ui->tabWidgetServer->currentIndex());

    if(currentWidget == m_adminsWidget) {
        delete m_adminsWidget;
        m_adminsWidget = 0;
    }
    if(currentWidget == m_alarmsWidget) {
        delete m_alarmsWidget;
        m_alarmsWidget = 0;
    }

    if(currentWidget == m_announcementManagementWidget) {
        delete m_announcementManagementWidget;
        m_announcementManagementWidget = 0;
    }

}

void ServerManagementWidget::showTabMenu()
{
    QWidget *cornerWidget = ui->tabWidgetServer->cornerWidget(Qt::TopLeftCorner);
    QPoint pos = cornerWidget->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() + cornerWidget->height());

    showMenu(pos);

}

void ServerManagementWidget::showMenu(const QPoint &pos)
{

    QMenu menu;

    if(!m_myself->isVerified()) {
        menu.addAction(ui->actionLogin);
    } else {
        menu.addAction(ui->actionAdministrators);
        menu.addAction(ui->actionAlarms);
        menu.addAction(ui->actionAnnouncement);
    }

    menu.exec(pos);

}

void ServerManagementWidget::manageAdmins()
{
    if(!verifyPrivilege()) {
        return;
    }

    if(!m_adminsWidget) {
        m_adminsWidget = new AdminsManagementWidget(this);
        ui->tabWidgetServer->addTab(m_adminsWidget, tr("Administrators"));
    }

    ui->tabWidgetServer->setCurrentIndex(ui->tabWidgetServer->indexOf(m_adminsWidget));

}

void ServerManagementWidget::manageAlarms()
{
    if(!m_alarmsWidget) {
        m_alarmsWidget = new AlarmsManagementWidget(this);
        ui->tabWidgetServer->addTab(m_alarmsWidget, tr("Alarms"));
    }

    ui->tabWidgetServer->setCurrentIndex(ui->tabWidgetServer->indexOf(m_alarmsWidget));

}

void ServerManagementWidget::manageAnnouncements()
{
    if(!m_announcementManagementWidget) {
        m_announcementManagementWidget = new AnnouncementManagementWidget(this);
        ui->tabWidgetServer->addTab(m_announcementManagementWidget, tr("Announcements"));
    }

    ui->tabWidgetServer->setCurrentIndex(ui->tabWidgetServer->indexOf(m_announcementManagementWidget));

}

bool ServerManagementWidget::verifyPrivilege()
{

    if(!m_myself->isAdminVerified()) {
        return false;
    }
    if(m_myself->isReadonly()) {
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;
}

void ServerManagementWidget::adminVerified()
{
    //m_socketConnectedToServer = m_myself->socketConnectedToServer();

    ui->labelServerIP->setText(m_myself->serverAddress());
    ui->labelServerPort->setText(QString::number(m_myself->serverPort()));
    ui->labelServerName->setText(m_myself->serverName());

}

QString ServerManagementWidget::convertDisksInfoToHTML(const QString &disksInfo)
{

    //// disksInfo Sample:
    /// WIN
    // Partion	Type	Size	Available	Usage%
    // C:\	NTFS	30.08GB	3.25GB	89.16
    // D:\	NTFS	150.1GB	4.94GB	96.7
    // E:\	NTFS	150.1GB	0.46GB	99.68
    // F:\	NTFS	90.41GB	12.83GB	85.8
    // G:\	NTFS	463.75GB    119.8GB	74.16
    // H:\           0           0	0
    // J:\	CDFS	0.01GB	0	100
    /// LINUX
    // Filesystem     Type   Size  Used Avail Use% Mounted on
    // /dev/sda1      ext3    15G  5.1G  9.0G  37% /
    // udev           tmpfs  1.5G  144K  1.5G   1% /dev
    // tmpfs          tmpfs  1.5G     0  1.5G   0% /dev/shm
    // /dev/sda5      ext3   9.9G  152M  9.2G   2% /home
    // /dev/sdb1      ext3    50G   42G  5.1G  90% /opt
    // /dev/sda6      ext3    20G   16G  3.0G  85% /mnt/temp


    QString html = "<html><head><meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"><title>Disks</title>"
                   "<style type=\"text/css\">"
                   "table{ background-color: #b2b2b2; margin-top: 1px; margin-bottom: 1px; margin-left: 1px; margin-right: 1px; width: 100%; font-size: 16px;}"
                   "table tr{background-color: #FFFFFF;}"
                   "</style>"
                   "</head><body>"
                   ;

    html += "<table  border=\"0\" cellpadding=\"5\" cellspacing=\"1\"  >";

    QStringList list = disksInfo.split("\n");
    foreach (QString row, list) {
        if(row.isEmpty()) {
            continue;
        }
        html += "<tr>";
        QString sep = "\t";
        if(row.contains(" ")) {
            sep = " ";
        }
        QStringList infoList = row.split(sep);
        infoList.removeAll(" ");
        //if(infoList.size() != 5){continue;}
        foreach (QString item, infoList) {
            html += "<td>";
            html += item;
            html += "</td>";
        }
        html += "</tr>";
    }

    html += "</table></body></html>";

    return html;

}



} //namespace HEHUI
