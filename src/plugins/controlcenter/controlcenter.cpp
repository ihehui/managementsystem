


#include <QObject>
#include <QtSql>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QCloseEvent>

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>



#include "constants.h"
#include "controlcenter.h"
#include "servermanagement/serveraddressmanagerwindow.h"

#include "../../sharedms/settings.h"

#include "HHSharedCore/hglobal_core.h"
#include "HHSharedCore/hutilities.h"
#include "HHSharedGUI/hdatabaseconnecter.h"
#include "HHSharedGUI/hdataoutputdialog.h"
#include "HHSharedNetwork/hnetworkutilities.h"


namespace HEHUI {


bool ControlCenter::running = false;

ControlCenter::ControlCenter(const QString appName, QWidget *parent)
    : QMainWindow(parent), m_appName(appName)
{
    ui.setupUi(this);
    //setWindowFlags(Qt::Dialog);

    //setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle(m_appName);

    ui.statusbar->hide();

    ui.comboBoxOSVersion->addItem("All", QVariant(""));
    ui.comboBoxOSVersion->addItem("WIN_XP", QVariant("xp"));
    ui.comboBoxOSVersion->addItem("WIN_2003", QVariant("2003"));
    ui.comboBoxOSVersion->addItem("WIN_Vista", QVariant("vista"));
    ui.comboBoxOSVersion->addItem("WIN_7", QVariant("7"));
    ui.comboBoxOSVersion->addItem("WIN_2008", QVariant("2008"));
    ui.comboBoxOSVersion->addItem("WIN_8", QVariant(" 8"));
    ui.comboBoxOSVersion->addItem("WIN_2012", QVariant("2012"));
    ui.comboBoxOSVersion->addItem("WIN_8.1", QVariant("8.1"));
    ui.comboBoxOSVersion->addItem("WIN_10", QVariant("10"));
    ui.comboBoxOSVersion->setCurrentIndex(0);


    QMap<QString/*Short Name*/, QString/*Department*/> departments;
    departments.insert("", tr(""));
    departments.insert("it", tr("IT"));
    foreach (QString key, departments.keys()) {
        ui.comboBoxWorkgroup->addItem(departments.value(key), key);
    }
    ui.comboBoxWorkgroup->setCurrentIndex(0);

    ui.comboBoxUSBSD->addItem(tr("All"), -1);
    ui.comboBoxUSBSD->addItem(tr("ReadWrite"), quint8(MS::USBSTOR_ReadWrite));
    ui.comboBoxUSBSD->addItem(tr("ReadOnly"), quint8(MS::USBSTOR_ReadOnly));
    ui.comboBoxUSBSD->addItem(tr("Disabled"), quint8(MS::USBSTOR_Disabled));
    ui.comboBoxUSBSD->addItem(tr("Unknown"), quint8(MS::USBSTOR_Unknown));

    ui.comboBoxProcMon->addItem(tr("All"), QVariant(-1));
    ui.comboBoxProcMon->addItem(tr("Enabled"), QVariant(1));
    ui.comboBoxProcMon->addItem(tr("Disabled"), QVariant(0));
    ui.comboBoxProcMon->setCurrentIndex(0);

    searchClientsMenu = new QMenu();
    searchClientsMenu->addAction(ui.actionQueryDatabase);
    searchClientsMenu->addAction(ui.actionSearchNetwork);
    ui.toolButtonQuery->setMenu(searchClientsMenu);
    ui.toolButtonQuery->setDefaultAction(ui.actionQueryDatabase);

    ui.toolButtonfilter->setDefaultAction(ui.actionFilter);



    connect(ui.actionManage, SIGNAL(triggered()), this, SLOT(slotRemoteManagement()));
    connect(ui.actionQueryDatabase, SIGNAL(triggered()), this, SLOT(slotQueryDatabase()));
    connect(ui.actionSearchNetwork, SIGNAL(triggered()), this, SLOT(slotSearchNetwork()));
    connect(ui.actionFilter, SIGNAL(triggered()), this, SLOT(filter()));

    connect(ui.actionUpdatePassword, SIGNAL(triggered()), this, SLOT(slotUpdateUserLogonPassword()));
    connect(ui.actionInformNewPassword, SIGNAL(triggered()), this, SLOT(slotInformUserNewLogonPassword()));


    connect(ui.tableViewClientList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotShowClientInfo(const QModelIndex &)));
    //connect(ui.tableViewClientList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui.tableViewClientList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotRemoteManagement(const QModelIndex &)));

    connect(ui.tableViewClientList, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(slotShowCustomContextMenu(QPoint)));

    connect(ui.actionRemoteDesktop, SIGNAL(triggered()), this, SLOT(slotRemoteDesktop()));


    connect(ui.actionExport, SIGNAL(triggered()), this, SLOT(slotExportQueryResult()));
    connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(slotPrintQueryResult()));



    localComputerName = QHostInfo::localHostName().toLower();
    //localSystemManagementWidget = 0;

    databaseConnectionName = QString(DB_CONNECTION_NAME);
    query = 0;

    clientInfoModel = new ClientInfoModel(this);
    proxyModel = new ClientInfoSortFilterProxyModel(this);
    proxyModel->setSourceModel(clientInfoModel);
    proxyModel->setDynamicSortFilter(true);
    ui.tableViewClientList->setModel(proxyModel);


    this->installEventFilter(this);

    running = true;

    m_networkReady = false;
    resourcesManager = ResourcesManagerInstance::instance();
    controlCenterPacketsParser = 0;
    m_udpServer = 0;
    m_localUDPListeningPort = IP_MULTICAST_GROUP_PORT + 10;

    m_rtp = 0;
    m_localRTPListeningPort = RTP_LISTENING_PORT + 10;
    m_socketConnectedToServer = INVALID_SOCK_ID;
//    m_serverAddress = "";
//    m_serverPort = 0;

//    Settings settings(SETTINGS_FILE_NAME, "./");
//    QStringList lastUsedAppServer = settings.getLastUsedAppServer().split(":");
//    if(lastUsedAppServer.size() == 2){
//        m_serverAddress = lastUsedAppServer.at(0);
//        m_serverPort = lastUsedAppServer.at(1).toUShort();
//    }

//    m_loginDlg = 0;


    startNetwork();

    vncProcess = 0;
    

    
    //    ui.toolButtonUpdatePassword->setEnabled(false);
    //    ui.toolButtonAnnouncement->setEnabled(false);
    
    
//    localUDPListeningPort = 0;
//    localRUDPListeningPort = 0;
    

//    m_serverInstanceID = 0;

    m_remoteDesktopMonitor = 0;

    //m_readonly = true;
    m_adminUser = AdminUser::instance();
    m_adminUser->init(m_rtp, controlCenterPacketsParser, this);
    connect(m_adminUser, SIGNAL(signalVerified()), this, SLOT(adminVerified()));
    


    slotInitTabWidget();

}

ControlCenter::~ControlCenter()
{
    qDebug()<<"--ControlCenter::~ControlCenter()";

    if(vncProcess){
        vncProcess->terminate();
        delete vncProcess;
        vncProcess = 0;
    }

    if(query){
        query->clear();
        delete query;
        query = 0;
    }

    QSqlDatabase db = QSqlDatabase::database(databaseConnectionName);
    if(db.isOpen()){
        db.close();
    }
    QSqlDatabase::removeDatabase(databaseConnectionName);

    delete controlCenterPacketsParser;
    controlCenterPacketsParser = 0;

//    resourcesManager->cleanInstance();
    delete resourcesManager;
    resourcesManager = 0;

    //TODO
    PacketHandlerBase::clean();


    delete proxyModel;
    proxyModel = 0;

    delete clientInfoModel;
    clientInfoModel = 0;

    if(m_remoteDesktopMonitor){
        delete m_remoteDesktopMonitor;
        m_remoteDesktopMonitor = 0;
    }

    delete m_adminUser;
    m_adminUser = 0;

    running = false;   

}

bool ControlCenter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyRelease ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down){
            //slotShowClientInfo(ui.tableViewClientList->currentIndex());
        }

        if(keyEvent->key() == Qt::Key_Escape){
            if(ui.lineEditComputerName->isReadOnly()){

                ui.lineEditAssetNO->setReadOnly(false);

                ui.lineEditComputerName->setReadOnly(false);
                ui.lineEditUserName->setReadOnly(false);

                //ui.comboBoxWorkgroup->setEnabled(true);
                ui.comboBoxUSBSD->setEnabled(true);

                ui.lineEditIPAddress->setReadOnly(false);

                ui.comboBoxOSVersion->setEnabled(true);
                ui.comboBoxProcMon->setEnabled(true);

                //ui.lineEditComputerName->setFocus();
                ui.lineEditUserName->setFocus();

            }else{
                ui.lineEditAssetNO->clear();
                ui.lineEditComputerName->clear();
                ui.lineEditUserName->clear();

                ui.comboBoxWorkgroup->setCurrentIndex(0);
                ui.comboBoxUSBSD->setCurrentIndex(0);

                ui.lineEditIPAddress->clear();

                ui.comboBoxOSVersion->setCurrentIndex(0);
                ui.comboBoxProcMon->setCurrentIndex(0);

                //ui.lineEditComputerName->setFocus();
                ui.lineEditUserName->setFocus();

            }

        }

        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O){
            slotExportQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P){
            slotPrintQueryResult();
        }

        return true;
    }else{
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

}

void ControlCenter::languageChange() {
    retranslateUi();
}

void ControlCenter::closeEvent(QCloseEvent *e) {
    qDebug()<<"--ControlCenter::closeEvent(...)";

    //Close all related TabPage
    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        //SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
        //if(!systemManagementWidget){continue;}
        //systemManagementWidget->close();

        QWidget *widget = qobject_cast<QWidget *>(ui.tabWidget->widget(i));
        if(!widget){continue;}
        widget->close();

        ui.tabWidget->removeTab(i);
        //systemManagementWidget->deleteLater();
    }


    if(controlCenterPacketsParser && m_socketConnectedToServer){
        controlCenterPacketsParser->sendAdminOnlineStatusChangedPacket(m_socketConnectedToServer, m_adminUser->getUserID(), false);
        m_rtp->closeSocket(m_socketConnectedToServer);
    }
    
    clientInfoModel->clear();
    
//    clientSocketsHash.clear();


    e->accept();
    //deleteLater();

}


void ControlCenter::retranslateUi() {

    ui.retranslateUi(this);

}

void ControlCenter::slotInitTabWidget(){

    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabPageChanged()));

    QTabBar *tabBar = ui.tabWidget->tabBar();
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar->shape();
    }

    // workaround for sgi style
    //QPalette pal = palette();
    // pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
    // pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
    // pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));

    QToolButton *newTabButton = new QToolButton(this);
    ui.tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/icon/resources/images/addtab.png"));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(slotNewTab()));
    newTabButton->setToolTip(tr("Add Page"));
    newTabButton->setEnabled(true);

    QToolButton *closeTabButton = new QToolButton(this);
    //closeTabButton->setPalette(pal);
    ui.tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton->setCursor(Qt::ArrowCursor);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setIcon(QIcon(":/icon/resources/images/closetab.png"));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(slotcloseTab()));
    closeTabButton->setToolTip(tr("Close Page"));
    closeTabButton->setEnabled(false);



//    ClientInfo localInfo("");
//    localInfo.setComputerName(localComputerName);
//    localSystemManagementWidget = new SystemManagementWidget(0, 0, m_adminUser, &localInfo);
//    localSystemManagementWidget->setParent(this);
//    ui.tabWidget->addTab(localSystemManagementWidget, tr("Local Computer"));

    ui.tabWidget->setCurrentIndex(0);

    SystemManagementWidget systemManagementWidget(0, 0);
    ui.tabServer->setMinimumSize(systemManagementWidget.minimumSizeHint());


//    slotNewTab();
//    ui.lineEditAssetNO->setFocus();
//    QTimer::singleShot(0, this, SLOT(slotcloseTab()));

}

void ControlCenter::slotTabPageChanged(){

    QWidget *currentWidget = ui.tabWidget->currentWidget();

    SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(currentWidget);
    if(systemManagementWidget){
//        if(systemManagementWidget == localSystemManagementWidget){
//            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
//        }else{
            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(true);
//        }
    }else{      
        ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
    }

    updateWindowTitle();

}

void ControlCenter::slotNewTab(ClientInfo *info){

    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_rtp, controlCenterPacketsParser, info, this);
    connect(systemManagementWidget, SIGNAL(updateTitle(SystemManagementWidget*)), this, SLOT(updateTabText(SystemManagementWidget*)));
    connect(systemManagementWidget, SIGNAL(signalSetProcessMonitorInfo(const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)), this, SLOT(changProcessMonitorInfo(const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)));

    ui.tabWidget->addTab(systemManagementWidget, tr("System Management"));
    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);
    ui.tabWidget->setCurrentWidget(systemManagementWidget);

    updateGeometry();

}

void ControlCenter::slotcloseTab(){

    //   if(ui.tabWidget->count()==1){
    //        return;
    //   }

    //QWidget *page = qobject_cast<QWidget *>(ui.tabWidget->currentWidget());
    //ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
    //delete page;

    SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->currentWidget());
    if(systemManagementWidget){
        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
        systemManagementWidget->close();
        //systemManagementWidget->deleteLater();
    }

    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);


}

void ControlCenter::updateWindowTitle(){
    QString title = ui.tabWidget->tabText(ui.tabWidget->currentIndex());
    setWindowTitle(tr("%1[%2]").arg(m_appName).arg(title));
}

void ControlCenter::updateTabText(SystemManagementWidget *wgt){
    if(!wgt){return;}
    QString title = wgt->peerAssetNO();
    ui.tabWidget->setTabText(ui.tabWidget->indexOf(wgt), title);

    updateWindowTitle();
}


void ControlCenter::slotRemoteManagement(const QModelIndex &index){

    QModelIndex idx = index;
    if(!idx.isValid()){
        QModelIndexList  indexList = ui.tableViewClientList->selectionModel()->selectedIndexes();
        if(indexList.isEmpty()){return;}
        idx = indexList.at(0);
    }
    if(!idx.isValid()){
        return;
    }

    ClientInfo *info = clientInfoModel->getClientInfo(idx);
    Q_ASSERT(info);

    QString assetNO = info->getAssetNO();

//    if(info->getComputerName() == localComputerName){
//        ui.tabWidget->setCurrentWidget(localSystemManagementWidget);
//        return;
//    }

    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        SystemManagementWidget *wgt = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
        if(!wgt){continue;}
        if(wgt->peerAssetNO() == assetNO){
            ui.tabWidget->setCurrentIndex(i);
            return;
        }
    }

    slotNewTab(info);

//    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_rtp, controlCenterPacketsParser, info, this);
//    connect(systemManagementWidget, SIGNAL(updateTitle(SystemManagementWidget*)), this, SLOT(updateTabText(SystemManagementWidget*)));
//    connect(systemManagementWidget, SIGNAL(signalSetProcessMonitorInfo(const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)), this, SLOT(changProcessMonitorInfo(const QByteArray &, const QByteArray &, bool, bool, bool, bool, bool, const QString &)));

//    ui.tabWidget->addTab(systemManagementWidget, assetNO);
//    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);
//    ui.tabWidget->setCurrentWidget(systemManagementWidget);

}

bool ControlCenter::openDatabase(bool reopen){

    if(query){
        query->clear();
        delete query;
        query = 0;
    }

    QSqlDatabase db;
    if(!openDatabase(&db, reopen)){
        return false;
    }

    if(!query){
        query = new QSqlQuery(db);
    }

    return true;

}

bool ControlCenter::openDatabase(QSqlDatabase *database, bool reopen, QString *errorString){
    if(!database){
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(reopen){
        database->close();
        QSqlDatabase::removeDatabase(DB_CONNECTION_NAME);
    }

    Settings settings(SETTINGS_FILE_NAME, "./");

    QString driver = DB_DRIVER;
    QString host = "";
    quint16 port = DB_SERVER_PORT;
    QString user = DB_USER_NAME;
    QString passwd = "";
    QString databaseName = DB_NAME;
    HEHUI::DatabaseType databaseType = HEHUI::MYSQL ;

    host = settings.getDBServerHost();
    if(!host.isEmpty()){
        driver = settings.getDBDriver();
        port = settings.getDBServerPort();
        user = settings.getDBServerUserName();
        passwd = settings.getDBServerUserPassword();
        databaseName = settings.getDBName();
        databaseType = settings.getDBType();
    }

    DatabaseConnecter dc(this);
    if(!dc.isDatabaseOpened(DB_CONNECTION_NAME,
                            driver,
                            host,
                            port,
                            user,
                            passwd,
                            databaseName,
                            databaseType
                            )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Error"), tr("Database Connection Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Query Failed!");
        return false;
    }

    if(dc.settingsModified()){
        settings.setDBDriver(dc.dbDriver());
        settings.setDBServerHost(dc.dbServerHost());
        settings.setDBServerPort(dc.dbServerPort());
        settings.setDBServerUserName(dc.dbUser());
        settings.setDBServerUserPassword(dc.dbPasswd());
        settings.setDBName(dc.dbName());
        settings.setDBType(dc.dbType());
        settings.sync();
    }

    *database = QSqlDatabase::database(DB_CONNECTION_NAME);

    return true;
}



bool ControlCenter::execQuery(const QString &statement ){

    if(!query){
        if(!openDatabase()){
            return false;
        }
    }else{
        query->clear();
    }

    if(!query->exec(statement)){
        QSqlError error = query->lastError();
        QString msg = tr("Can not execute the SQL statement! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        QMessageBox::critical(this, tr("Error"), msg);

        qCritical()<<msg;
        qCritical()<<"statement:";
        qCritical()<<statement;
        qCritical()<<"";
        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            query->clear();
            openDatabase(true);
        }

        return false;
    }

    return true;

}

inline QString ControlCenter::assetNO() const {
    return ui.lineEditAssetNO->text().trimmed();
}

inline QString ControlCenter::computerName() const {
    return ui.lineEditComputerName->text().trimmed();
}

inline QString ControlCenter::osVersion() const{
    return ui.comboBoxOSVersion->currentData().toString();
}

inline QString ControlCenter::workgroup() const {
    return ui.comboBoxWorkgroup->currentText();
}

inline QString ControlCenter::userName() const {
    return ui.lineEditUserName->text().trimmed();
}

inline QString ControlCenter::ipAddress() const {
    return ui.lineEditIPAddress->text().trimmed();
}

int ControlCenter::usbsdStatus(){
    return ui.comboBoxUSBSD->currentData().toInt();
}

int ControlCenter::procMonEnabled() const{
    return ui.comboBoxProcMon->currentData().toInt();
}

void ControlCenter::updateActions() {

    bool selected = ui.tableViewClientList->currentIndex().isValid() && ui.tableViewClientList->selectionModel()->selectedIndexes().size();

    if(osVersion().contains("2000", Qt::CaseInsensitive)){
        ui.actionRemoteDesktop->setEnabled(false);
    }else{
        ui.actionRemoteDesktop->setEnabled(selected);
    }

    ui.actionExport->setEnabled(selected);
    ui.actionPrint->setEnabled(selected);

    ui.actionManage->setEnabled(selected);

}

void ControlCenter::slotQueryDatabase(){
    if(!m_adminUser->isAdminVerified()){
        return;
    }
    
    ui.toolButtonQuery->setDefaultAction(ui.actionQueryDatabase);  

    clientInfoModel->clear();
    proxyModel->cleanFilters();

    controlCenterPacketsParser->sendRequestClientInfoPacket(m_socketConnectedToServer, "", MS::SYSINFO_OS);
//    controlCenterPacketsParser->sendRequestClientInfoPacket(m_socketConnectedToServer, "", MS::SYSINFO_HARDWARE);


    //statusBar()->showMessage(tr("Matched In Database:%1").arg(QString::number(queryModel->rowCount())));
    
}

void ControlCenter::slotSearchNetwork() {
    if(!m_adminUser->isAdminVerified()){
        return;
    }

    ui.toolButtonQuery->setDefaultAction(ui.actionSearchNetwork);

    clientInfoModel->clear();
    proxyModel->cleanFilters();

    //Broadcast
    QList<QHostAddress> broadcastAddresses = NetworkUtilities::broadcastAddresses();
    foreach (QHostAddress address, broadcastAddresses) {
        controlCenterPacketsParser->sendAdminSearchClientPacket(address, computerName(), userName(), workgroup(), assetNO(), ipAddress(), osVersion(), m_adminUser->getUserID());
    }
    
    //statusBar()->showMessage(tr("Matched:%1 Total:%2").arg(QString::number(proxyModel->rowCount())).arg(clientInfoHash.size()));
    statusBar()->showMessage(tr("Matched In Network:%1").arg(QString::number(proxyModel->rowCount())));

}

void ControlCenter::filter(){

    QRegExp assetNORegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp computerNameRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp workgroupRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp osRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp userNameRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp ipRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp usbSDRegExp = QRegExp(".*", Qt::CaseInsensitive);
    QRegExp procMonRegExp = QRegExp(".*", Qt::CaseInsensitive);


    QString filterString = assetNO();
    if(!filterString.trimmed().isEmpty()){
        assetNORegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = computerName();
    if(!filterString.trimmed().isEmpty()){
        computerNameRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = osVersion();
    if(!filterString.trimmed().isEmpty()){
        osRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = workgroup();
    if(!filterString.trimmed().isEmpty()){
        workgroupRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = userName();
    if(!filterString.trimmed().isEmpty()){
        userNameRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = ipAddress();
    if(!filterString.trimmed().isEmpty()){
        ipRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = QString::number(usbsdStatus());
    if(filterString != "-1"){
        usbSDRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = QString::number(procMonEnabled());
    if(filterString != "-1"){
        procMonRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    proxyModel->setFilters(assetNORegExp, computerNameRegExp, osRegExp, workgroupRegExp, userNameRegExp, ipRegExp, usbSDRegExp, procMonRegExp );

    statusBar()->showMessage(tr("Matched:%1").arg(QString::number(proxyModel->rowCount())));

}

//void ControlCenter::slotUpdateButtonClicked() {


//}


void ControlCenter::slotShowClientInfo(const QModelIndex &index) {

//    if(!index.isValid()){
//        return;
//    }

//    int row = index.row();
//    QStringList list;


//    for(int i = 0; i < 13; i++){
//        QModelIndex idx =  index.sibling(row,i);
//        list << idx.data().toString();
//        //QMessageBox::information(this,QString(row),idx.data().toString());
//    }

//    int index = 0;
//    ui.lineEditAssetNO->setText(networkInfoList.at(index++).toLower());
//    ui.lineEditComputerName->setText(list.at(index++));
//    ui.comboBoxWorkgroup->setCurrentIndex( ui.comboBoxWorkgroup->findText(list.at(1),Qt::MatchStartsWith));

//    QStringList networkInfoList = list.at(2).split(",").at(0).split("/");
//    ui.lineEditIPAddress->setText(networkInfoList.at(0));

//    ui.lineEditUserName->setText(list.at(3));
//    ui.comboBoxOSVersion->setCurrentIndex(ui.comboBoxOSVersion->findText(list.at(4),Qt::MatchStartsWith));


//    ui.comboBoxUSBSD->setCurrentIndex( ui.comboBoxUSBSD->findData((list.at(5).toUShort())));
//    ui.comboBoxPrograms->setCurrentIndex( ui.comboBoxPrograms->findText((list.at(6) == "1"?tr("Enabled"):tr("Disabled")),Qt::MatchStartsWith));

//    m_administrators = list.at(7);


//    if(!ui.lineEditComputerName->isReadOnly()){
//        ui.lineEditComputerName->setReadOnly(true);
//        ui.lineEditUserName->setReadOnly(true);

//        ui.comboBoxWorkgroup->setEnabled(false);
//        ui.comboBoxUSBSD->setEnabled(false);

//        ui.lineEditMACAddress->setReadOnly(true);
//        ui.lineEditIPAddress->setReadOnly(true);

//        ui.comboBoxOSVersion->setEnabled(false);
//        ui.comboBoxPrograms->setEnabled(false);
//    }

//    ui.toolButtonManage->setEnabled(true);


}

void ControlCenter::slotExportQueryResult(){

    DataOutputDialog dlg(ui.tableViewClientList, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void ControlCenter::slotPrintQueryResult(){

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.tableViewClientList, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

void ControlCenter::slotRemoteDesktop(){

    //    QDesktopWidget *desktop = QApplication::desktop();
    //    QRect rect = desktop->screenGeometry();
    
    
    //    QProcess *rdpProcess = new QProcess(this);
    //    connect(rdpProcess, SIGNAL(finished(int)), rdpProcess, SLOT(deleteLater()));

    QString cmdString = ""; 

#ifdef Q_OS_WIN32

    cmdString = QString("mstsc.exe /v:%1 /f").arg(ipAddress());

#else

    cmdString = QString("rdesktop %1 -f").arg(ipAddress());

#endif

    //    rdpProcess->start(cmdString);

    if(!QProcess::startDetached(cmdString)){
        QMessageBox::critical(this, tr("Error"), tr("Can not start RDP client process!"));
    }

}

void ControlCenter::slotVNC(){
    QMessageBox::critical(this, tr("Error"), tr("Function Disabled!"));
    return;

    if(!vncProcess){
        vncProcess = new QProcess(this);
    }
    
    if(vncProcess->state() == QProcess::Running){
        return;
    }
    
    
    QString cmdString = ""; 

#ifdef Q_OS_WIN32

    //cmdString = QString("vncviewer.exe %1").arg(ipAddress());
    cmdString = QString("vncviewer.exe -listen");

#else

    cmdString = QString("vnc %1").arg(ipAddress());

#endif

    //    if(!QProcess::startDetached(cmdString)){
    //        QMessageBox::critical(this, tr("Error"), tr("Can not start VNC client process!"));
    //    }
    

    vncProcess->start(cmdString);
    if(!vncProcess->waitForStarted(5000)){
        QMessageBox::critical(this, tr("Error"), tr("Can not start VNC client process!"));
    }
    
    
    //TODO:
    
}


void ControlCenter::changProcessMonitorInfo(const QByteArray &localRulesData, const QByteArray &globalRulesData, bool enableProcMon, bool enablePassthrough, bool enableLogAllowedProcess, bool enableLogBlockedProcess, bool useGlobalRules, const QString &assetNO){

    if(INVALID_SOCK_ID == m_socketConnectedToServer){
        QMessageBox::critical(this, tr("Error"), tr("Server is offline! Can not sync process monitor data to server!"));
        return;
    }

    bool ok = controlCenterPacketsParser->sendRequestChangeProcessMonitorInfoPacket(m_socketConnectedToServer, localRulesData, globalRulesData, enableProcMon, enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess, useGlobalRules, assetNO);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to server!<br>%1").arg(m_rtp->lastErrorString()));
    }

}

void ControlCenter::slotUpdateUserLogonPassword(){
    QMessageBox::critical(this, tr("Error"), tr("Function Disabled!"));
    return;
    
    if(m_adminUser->isReadonly()){
        QMessageBox::critical(this, tr("Error"), tr("You dont have the access permissions!"));
        return;
    }

    if(ui.comboBoxWorkgroup->currentText().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the workgroup!"));
        ui.comboBoxWorkgroup->setFocus();
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to inform the clients to update the password?"), QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }
    
    
    controlCenterPacketsParser->sendUpdateMSUserPasswordPacket("", 0, workgroup(), m_adminUser->getUserID());
}

void ControlCenter::slotInformUserNewLogonPassword(){

    QMessageBox::critical(this, tr("Error"), tr("Function Disabled!"));
    return;
    
    if(m_adminUser->isReadonly()){
        QMessageBox::critical(this, tr("Error"), tr("You dont have the access permissions!"));
        return;
    }

    if(ui.comboBoxWorkgroup->currentText().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Please select the workgroup!"));
        ui.comboBoxWorkgroup->setFocus();
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to inform the clients to retrieve the new password?"), QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }
    
    
    controlCenterPacketsParser->sendInformUpdatePasswordPacket("", 0, workgroup(), m_adminUser->getUserID());
    
}

void ControlCenter::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *tableView = qobject_cast<QTableView*> (sender());

    if (!tableView){
        return;
    }

    updateActions();

    QMenu menu(this);

    menu.addAction(ui.actionManage);
    menu.addSeparator();
    menu.addAction(ui.actionRemoteDesktop);
    menu.addSeparator();

    menu.addAction(ui.actionExport);

#ifndef QT_NO_PRINTER

    menu.addSeparator();

    ui.actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui.actionPrint);

#endif

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}


void ControlCenter::startNetwork(){

    QString errorMessage = "";
    m_udpServer = resourcesManager->startUDPServer(QHostAddress::Any, m_localUDPListeningPort, true, &errorMessage);
    if(!m_udpServer){
        QMessageBox::critical(this, tr("Error"), tr("Can not start UDP listening on port %1! %2").arg(m_localUDPListeningPort).arg(errorMessage));
    }else{
        qWarning()<<QString("UDP listening on port %1!").arg(m_localUDPListeningPort);
    }

    m_rtp = resourcesManager->startRTP(QHostAddress::Any, m_localRTPListeningPort, true, &errorMessage);
    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)));


//    m_udtProtocol = m_rtp->getUDTProtocol();
////    if(!m_udtProtocol){
////        QString error = tr("Can not start UDT listening on port %1! %2").arg(m_localUDTListeningPort).arg(errorMessage);
////        QMessageBox::critical(this, tr("Error"), error);
////        close();
////        return;
////    }
//    m_localUDTListeningPort = m_udtProtocol->getUDTListeningPort();
//    //connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));
//    m_udtProtocol->startWaitingForIOInOneThread(1);
//    //m_udtProtocol->startWaitingForIOInSeparateThread();


    controlCenterPacketsParser = new ControlCenterPacketsParser(resourcesManager, this);

    connect(controlCenterPacketsParser, SIGNAL(signalJobFinished(quint32, quint8, const QVariant &)), JobMonitor::instance(), SLOT(finishJob(quint32, quint8, const QVariant &)), Qt::QueuedConnection);

    connect(controlCenterPacketsParser, SIGNAL(signalServerMessageReceived(const QString &, quint8)), this, SLOT(showServerMessage(const QString &, quint8)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientInfoPacketReceived(const QString &, const QByteArray &,quint8)), this, SLOT(updateOrSaveClientInfo(const QString &, const QByteArray &,quint8)));
    //connect(controlCenterPacketsParser, SIGNAL(signalClientOnlineStatusChanged(int, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(int, const QString&, bool)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalSystemInfoFromServerReceived(const QString &, const QByteArray &,quint8)), this, SLOT(processSystemInfoFromServer(const QString &, const QByteArray &,quint8)));

    connect(controlCenterPacketsParser, SIGNAL(signalAssetNOModifiedPacketReceived(const QString &, const QString &, bool, const QString &)), this, SLOT(processAssetNOModifiedPacket(const QString &, const QString &, bool, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalDesktopInfoPacketReceived(quint32, const QString &, int, int, int, int)), this, SLOT(processDesktopInfo(quint32, const QString &, int, int, int, int)));
    connect(controlCenterPacketsParser, SIGNAL(signalScreenshotPacketReceived(const QString &, QList<QPoint>, QList<QByteArray>)), this, SLOT(processScreenshot(const QString &, QList<QPoint>, QList<QByteArray>)));
    //connect(controlCenterPacketsParser, SIGNAL(signalServerResponseAdminLoginResultPacketReceived(SOCKETID, const QString &, bool, const QString &, bool)), this, SLOT(processLoginResult(SOCKETID, const QString &, bool, const QString &, bool)));



//    if(localSystemManagementWidget){
//        localSystemManagementWidget->setRTP(m_rtp);
//        localSystemManagementWidget->setControlCenterPacketsParser(controlCenterPacketsParser);
//    }

    m_networkReady = true;

    //controlCenterPacketsParser->sendClientLookForServerPacket("255.255.255.255");


}

void ControlCenter::showServerMessage(const QString &message, quint8 messageType){

    QString msg = QString(tr("<p>Message From Server <b>%1</b> :</p>").arg(m_adminUser->serverName()));
    msg += message;
    switch(messageType){
    case quint8(MS::MSG_Information):
        QMessageBox::information(this, tr("Message"), msg);
        break;
    case quint8(MS::MSG_Warning):
        QMessageBox::warning(this, tr("Warning"), msg);
        break;
    case quint8(MS::MSG_Critical):
        QMessageBox::critical(this, tr("Error"), msg);
        break;
    default:
        QMessageBox::information(this, tr("Message"), msg);
    }

}

void ControlCenter::updateOrSaveClientInfo(const QString &assetNO, const QByteArray &clientInfoData, quint8 infoType){
    //qDebug()<<"--ControlCenter::updateOrSaveClientInfo(...) "<< " Asset NO.:"<<assetNO;
    
    ClientInfo *info = clientInfoModel->getClientInfo(assetNO);
    if(!info){
        info = new ClientInfo(assetNO, this);
    }

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
    case quint8(MS::SYSINFO_HARDWARE):
        info->setJsonData(clientInfoData);
        break;

//    case quint8(MS::SYSINFO_SOFTWARE):
//        processSoftwareInfo(info, clientInfo);
//        break;
//    case quint8(MS::SYSINFO_SERVICES):
//        updateServicesInfo(object);
//        break;
    default:
        qCritical()<<"ERROR! Unknown client info type!";
        break;
    }

    clientInfoModel->addClientInfo(info);

    qApp->processEvents();

}

void ControlCenter::processSystemInfoFromServer(const QString &extraInfo, const QByteArray &infoData, quint8 infoType){
    //qDebug()<<"--ControlCenter::processSystemInfoFromServer(...) "<< " extraInfo:"<<extraInfo<<" infoType:"<<infoType;


    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
    case quint8(MS::SYSINFO_HARDWARE):
    {
        if(!extraInfo.isEmpty()){
            updateClientInfoFromServer(extraInfo, infoData, infoType);
            return;
        }
    }
        break;

    default:
        break;
    }

    updateSystemInfoFromServer(extraInfo, infoData, infoType);

    qApp->processEvents();

}

void ControlCenter::updateSystemInfoFromServer(const QString &extraInfo, const QByteArray &infoData, quint8 infoType){
    //qDebug()<<"--ControlCenter::updateSystemInfoFromServer(...)"<<" infoType:"<<infoType;

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
    {
        QList<ClientInfo*> clientsList;
        if(infoData.isEmpty()){return;}

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(infoData, &error);
        if(error.error != QJsonParseError::NoError){
            qCritical()<<error.errorString();
            return;
        }
        QJsonObject object = doc.object();
        if(object.isEmpty()){return;}

        QJsonArray infoArray = object["OS"].toArray();
        if(infoArray.isEmpty()){return;}

        for(int i=0;i<infoArray.size();i++){
            QByteArray data = infoArray.at(i).toString().toUtf8();
            ClientInfo *info = new ClientInfo("", this);
            info->setJsonData(data);
            clientsList.append(info);
        }

        clientInfoModel->setClientList(clientsList);

    }
        break;

    case quint8(MS::SYSINFO_SYSADMINS):
    {
        ui.tabServer->setAdminsData(infoData);
    }
        break;

    case quint8(MS::SYSINFO_SYSALARMS):
    {
        ui.tabServer->setAlarmsData(infoData);
    }
        break;

    case quint8(MS::SYSINFO_SERVER_INFO):
    {
        ui.tabServer->updateServerInfo(infoData);
    }
        break;

    case quint8(MS::SYSINFO_REALTIME_INFO):
    {
        ui.tabServer->updateRealtimeInfo(infoData);
    }
        break;

    case quint8(MS::SYSINFO_ANNOUNCEMENTS):
    {
        ui.tabServer->setAnnouncementsData(infoData);
    }
        break;

    case quint8(MS::SYSINFO_ANNOUNCEMENTTARGETS):
    {
        ui.tabServer->setAnnouncementTargetsData(extraInfo,infoData);
    }
        break;

    case quint8(MS::SYSINFO_ANNOUNCEMENTREPLIES):
    {
        ui.tabServer->setAnnouncementReplies(infoData);
    }
        break;

    default:
        qCritical()<<"ERROR! Invalid info from server!";
        break;
    }


}

void ControlCenter::updateClientInfoFromServer(const QString &assetNO, const QByteArray &infoData, quint8 infoType){
    qDebug()<<"--ControlCenter::updateClientInfoFromServer(...)"<<" assetNO:"<<assetNO<<" infoType:"<<infoType;

    ClientInfo *info = clientInfoModel->getClientInfo(assetNO);
    if(!info){
        info = new ClientInfo(assetNO, this);
        clientInfoModel->addClientInfo(info);
    }

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
    case quint8(MS::SYSINFO_HARDWARE):
        info->setJsonData(infoData);
        break;

//    case quint8(MS::SYSINFO_SOFTWARE):
//        processSoftwareInfo(info, clientInfo);
//        break;
//    case quint8(MS::SYSINFO_SERVICES):
//        updateServicesInfo(object);
//        break;
    default:
        qCritical()<<"ERROR! Unknown client info type!";
        break;
    }

    clientInfoModel->updateClientInfo(info);

}

void ControlCenter::processAssetNOModifiedPacket(const QString &newAssetNO, const QString &oldAssetNO, bool modified, const QString &message){
    if(!modified){return;}

    ClientInfo *info = clientInfoModel->getClientInfo(oldAssetNO);
    if(!info){
        return;
    }
    info->setAssetNO(newAssetNO);
    clientInfoModel->updateClientInfo(info);
}

void ControlCenter::processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &clientName, bool online){
    qDebug()<<"--ControlCenter::processClientOnlineStatusChangedPacket(...)";

//    QString ip = "";
//    quint16 port = 0;

//    if(!m_udtProtocol->getAddressInfoFromSocket(socketID, &ip, &port)){
//        qCritical()<<m_udtProtocol->getLastErrorMessage();
//        return;
//    }

//    qWarning()<<QString("Client %1 %2!").arg(clientName).arg(online?"Online":"Offline");

//    if(online){
//        clientSocketsHash.insert(socketID, QHostAddress(ip));
//    }else{

//        int tabPages = ui.tabWidget->count();
//        for(int i = tabPages; i >= 0; --i){
//            SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//            if(!widget){continue;}
//            if(widget->peerIPAddress() == clientSocketsHash.value(socketID)){
//                widget->peerDisconnected(true);
//            }

//        }

//        clientSocketsHash.remove(socketID);
//    }

}

void ControlCenter::processDesktopInfo(quint32 userSocketID, const QString &userID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){

    if(!m_remoteDesktopMonitor){
        m_remoteDesktopMonitor = new RemoteDesktopMonitor(this);
        connect(m_remoteDesktopMonitor, SIGNAL(closeUserSocket(quint32)), this, SLOT(closeUserSocket(quint32)));
        connect(m_remoteDesktopMonitor, SIGNAL(signalClose()), this, SLOT(closeRemoteDesktopMonitor()));

        ui.tabWidget->insertTab(1, m_remoteDesktopMonitor, tr("Remote Desktop"));
        ui.tabWidget->setCurrentWidget(m_remoteDesktopMonitor);
    }

    m_remoteDesktopMonitor->setDesktopInfo(userSocketID, userID, desktopWidth, desktopHeight, blockWidth, blockHeight);

}

void ControlCenter::processScreenshot(const QString &userID, QList<QPoint> locations, QList<QByteArray> images){
    if(!m_remoteDesktopMonitor){
        qCritical()<<QString("ERROR! Remote Desktop Monitor Not Initialized.");
        return;
    }

    m_remoteDesktopMonitor->updateScreenshot(userID, locations, images);
}

void ControlCenter::closeRemoteDesktopMonitor(){
    qDebug()<<"--ControlCenter::closeRemoteDesktopMonitor()";

    if(!m_remoteDesktopMonitor){
        return;
    }

    ui.tabWidget->removeTab(ui.tabWidget->indexOf(m_remoteDesktopMonitor));

    delete m_remoteDesktopMonitor;
    m_remoteDesktopMonitor = 0;

}

void ControlCenter::closeUserSocket(quint32 userSocketID){
    qDebug()<<"--ControlCenter::closeUserSocket(...) userSocketID:"<<userSocketID;
    m_rtp->closeSocket(userSocketID);
}

void ControlCenter::peerConnected(const QHostAddress &peerAddress, quint16 peerPort){
    qWarning()<<QString("Connected! "+peerAddress.toString()+":"+QString::number(peerPort));

}

void ControlCenter::signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort){
    qCritical()<<QString("Connecting Timeout! "+peerAddress.toString()+":"+QString::number(peerPort));
}

void ControlCenter::peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose){
    qDebug()<<QString("Disconnected! "+peerAddress.toString()+":"+QString::number(peerPort));

//    if(!normalClose){
//        qCritical()<<QString("ERROR! Peer %1:%2 Closed Unexpectedly!").arg(peerAddress.toString()).arg(peerPort);
//    }

//    int tabPages = ui.tabWidget->count();
//    for(int i = tabPages; i >= 0; --i){
//        SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//        if(!widget){continue;}
//        if(widget->peerIPAddress() == peerAddress){
//            widget->peerDisconnected(normalClose);
//        }

//    }

}

void ControlCenter::peerDisconnected(SOCKETID socketID){
    qDebug()<<"--ControlCenter::peerDisconnected(...) socketID:"<<socketID;

//    if(clientSocketsHash.contains(socketID)){
//        qCritical()<<QString("ERROR! Peer %1 Closed Unexpectedly!").arg(clientSocketsHash.value(socketID).toString());

//        int tabPages = ui.tabWidget->count();
//        for(int i = tabPages; i >= 0; --i){
//            SystemManagementWidget *widget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->widget(i));
//            if(!widget){continue;}
//            if(widget->peerIPAddress() == clientSocketsHash.value(socketID)){
//                widget->peerDisconnected(false);
//            }
//        }

//        clientSocketsHash.remove(socketID);
//    }


    if(socketID == m_socketConnectedToServer){
        m_socketConnectedToServer = INVALID_SOCK_ID;
//        m_adminUser->setVerified(false);
//        m_readonly = true;
        return;
    }

    if(m_remoteDesktopMonitor){
        m_remoteDesktopMonitor->peerDisconnected(socketID);
    }

}

void ControlCenter::adminVerified(){
    m_socketConnectedToServer = m_adminUser->socketConnectedToServer();
}












} //namespace HEHUI
