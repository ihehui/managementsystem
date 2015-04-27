#include "servermanagementwidget.h"
#include "ui_servermanagementwidget.h"

#include <QMenu>
#include <QToolButton>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>



namespace HEHUI {


ServerManagementWidget::ServerManagementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerManagementWidget)
{
    ui->setupUi(this);

    initTabWidget();

    installEventFilter(this);
    ui->groupBoxTop->installEventFilter(this);
    ui->groupBoxSystemInfo->installEventFilter(this);
    ui->groupBoxResources->installEventFilter(this);
    ui->groupBoxStatistics->installEventFilter(this);

    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(verifyPrivilege()));
    connect(ui->actionAdministrators, SIGNAL(triggered()), this, SLOT(manageAdmins()));
    connect(ui->actionAlarms, SIGNAL(triggered()), this, SLOT(manageAlarms()));


    m_myself = AdminUser::instance();
    connect(m_myself, SIGNAL(signalVerified()), this, SLOT(adminVerified()));
    //m_socketConnectedToServer = INVALID_SOCK_ID;

    m_adminsWidget = 0;
    m_alarmsWidget = 0;
}

ServerManagementWidget::~ServerManagementWidget()
{
    delete ui;
}

bool ServerManagementWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ContextMenu ) {
        QContextMenuEvent *contextMenuEvent = static_cast<QContextMenuEvent *> (event);

        showMenu(contextMenuEvent->globalPos());

        return true;
    }else{
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}


void ServerManagementWidget::closeEvent(QCloseEvent *e) {

    //Close all related TabPage
    int tabPages = ui->tabWidgetServer->count();
    for(int i = tabPages; i >= 0; --i){
        ui->tabWidgetServer->removeTab(i);

        QWidget *widget = qobject_cast<QWidget *>(ui->tabWidgetServer->widget(i));
        if(!widget){continue;}
        widget->close();

    }

    e->accept();
    //deleteLater();

}

void ServerManagementWidget::setAdminsData(const QByteArray &infoData){
    manageAdmins();
    m_adminsWidget->setData(infoData);
}

void ServerManagementWidget::setAlarmsData(const QByteArray &infoData){
    manageAlarms();
    m_alarmsWidget->setData(infoData);
}

void ServerManagementWidget::updateServerInfo(const QByteArray &infoData){
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QJsonObject resLoadObj = object["ServerInfo"].toObject();
    if(resLoadObj.isEmpty()){
        return;
    }

    ui->labelVersion->setText(resLoadObj.value("Version").toString());
    ui->labelOS->setText(resLoadObj.value("OS").toString());


    ui->labelCPUType->setText(resLoadObj.value("CPU").toString());

    quint64 memory = resLoadObj.value("Memory").toString().toULong();
    memory = memory/(1024*1024);
    ui->labelMemory->setText(QString("%1 MB").arg(memory));

    unsigned int startupUTCTime = resLoadObj.value("StartupUTCTime").toString().toUInt();
    QDateTime time = QDateTime::fromTime_t(startupUTCTime);
    ui->labelStartupTime->setText(time.toString("yyyy-MM-dd hh:mm:ss"));

}

void ServerManagementWidget::updateRealtimeInfo(const QByteArray &infoData){
    //qDebug()<<"--ServerManagementWidget::updateRealtimeInfo(...)";

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QJsonObject resLoadObj = object["Realtime"].toObject();
    if(!resLoadObj.isEmpty()){
        ui->progressBarCPUUsage->setValue(resLoadObj.value("CPULoad").toString().toUInt());
        ui->progressBarMemoryUsage->setValue(resLoadObj.value("MemLoad").toString().toUInt());
    }

}

void ServerManagementWidget::initTabWidget(){

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

void ServerManagementWidget::slotTabPageChanged(){

    QWidget *currentWidget = ui->tabWidgetServer->currentWidget();
    if(currentWidget == ui->tabServerInfo){
        ui->tabWidgetServer->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
    }else{
        ui->tabWidgetServer->cornerWidget(Qt::TopRightCorner)->setEnabled(true);
    }

}

void ServerManagementWidget::slotcloseTab(){

    QWidget *currentWidget = ui->tabWidgetServer->currentWidget();
    if(currentWidget == ui->tabServerInfo){return;}
    if(!currentWidget->close()){return;}
    ui->tabWidgetServer->removeTab(ui->tabWidgetServer->currentIndex());

    if(currentWidget == m_adminsWidget){
        delete m_adminsWidget;
        m_adminsWidget = 0;
    }
    if(currentWidget == m_alarmsWidget){
        delete m_alarmsWidget;
        m_alarmsWidget = 0;
    }

}

void ServerManagementWidget::showTabMenu(){
    QWidget *cornerWidget = ui->tabWidgetServer->cornerWidget(Qt::TopLeftCorner);
    QPoint pos = cornerWidget->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() + cornerWidget->height());

    showMenu(pos);

}

void ServerManagementWidget::showMenu(const QPoint & pos){

    QMenu menu;

    if(!m_myself->isVerified()){
        menu.addAction(ui->actionLogin);
    }else{
        menu.addAction(ui->actionAdministrators);
        menu.addAction(ui->actionAlarms);
    }

    menu.exec(pos);

}

void ServerManagementWidget::manageAdmins(){
    if(!verifyPrivilege()){
        return;
    }

    if(!m_adminsWidget){
        m_adminsWidget = new AdminsManagementWidget(this);
        ui->tabWidgetServer->addTab(m_adminsWidget, tr("Administrators"));
    }

    ui->tabWidgetServer->setCurrentIndex(ui->tabWidgetServer->indexOf(m_adminsWidget));

}

void ServerManagementWidget::manageAlarms(){
    if(!m_alarmsWidget){
        m_alarmsWidget = new AlarmsManagementWidget(this);
        ui->tabWidgetServer->addTab(m_alarmsWidget, tr("Alarms"));
    }

    ui->tabWidgetServer->setCurrentIndex(ui->tabWidgetServer->indexOf(m_alarmsWidget));

}

bool ServerManagementWidget::verifyPrivilege(){

    if(!m_myself->isAdminVerified()){
        return false;
    }
    if(m_myself->isReadonly()){
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;
}

void ServerManagementWidget::adminVerified(){
    //m_socketConnectedToServer = m_myself->socketConnectedToServer();

    ui->labelServerIP->setText(m_myself->serverAddress());
    ui->labelServerPort->setText(QString::number(m_myself->serverPort()));
    ui->labelServerName->setText(m_myself->serverName());

}



} //namespace HEHUI
