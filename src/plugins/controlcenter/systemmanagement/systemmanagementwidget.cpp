
#include "systemmanagementwidget.h"

#include <QHostInfo>
#include <QHostAddress>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTime>
#include <QSettings>

#ifdef Q_OS_WIN32
    #include "HHSharedWindowsManagement/hwindowsmanagement.h"
    #include "HHSharedWindowsManagement/WinUtilities"
    //#include "../../sharedms/global_shared.h"
#endif

#include "constants.h"

#include "HHSharedGUI/hdatabaseconnecter.h"


namespace HEHUI {


SystemManagementWidget::SystemManagementWidget(RTP *rtp, ControlCenterPacketsParser *parser, const QString &adminName, ClientInfo *clientInfo,  QWidget *parent)
    : QWidget(parent), m_adminName(adminName)
{
    ui.setupUi(this);

    if(clientInfo){
        m_clientInfo = *clientInfo;
    }

    m_peerComputerName = m_clientInfo.getComputerName();
    ui.lineEditComputerName->setText(m_peerComputerName.toUpper());


    ui.lineEditIPAddress->setText("200.200.200.105");


    if(m_peerComputerName.toLower() == QHostInfo::localHostName().toLower()){
        localComputer = true;
    }else{
        localComputer = false;
    }

    if(localComputer){
        this->m_peerIPAddress = QHostAddress::LocalHost;
        ui.lineEditIPAddress->setText("127.0.0.1");
    }

    m_isJoinedToDomain = m_clientInfo.isJoinedToDomain();

    ui.comboBoxProtocol->addItem(tr("Auto"), quint8(RTP::AUTO));
    ui.comboBoxProtocol->addItem("TCP", quint8(RTP::TCP));
    ui.comboBoxProtocol->addItem("ENET", quint8(RTP::ENET));
    //ui.comboBoxProtocol->addItem("UDT", quint8(RTP::UDT));



    m_winDirPath = "";




#ifdef Q_OS_WIN32

    WindowsManagement wm;

    if(localComputer){
        ui.groupBoxAdministrationTools->show();
        ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabFileManagement));

        WinUtilities::getJoinInformation(&m_isJoinedToDomain);
        m_clientInfo.setIsJoinedToDomain(m_isJoinedToDomain);

    }else{
        int index = ui.tabWidget->indexOf(ui.tabLocalManagement);
        ui.tabWidget->removeTab(index);

        ui.tabLocalManagement->setEnabled(false);
        //            ui.groupBoxAdministrationTools->setEnabled(false);;
        //            ui.groupBoxAdministrationTools->hide();
    }

    m_winDirPath = wm.getEnvironmentVariable("windir");
    //m_winDirPath = QDir::rootPath() + "windows";


    if(m_isJoinedToDomain){
        ui.pushButtonAdminsManagement->hide();
        ui.pushButtonDomain->setText(tr("Unjoin The Domain"));
    }else{
        ui.pushButtonAdminsManagement->show();
        ui.pushButtonDomain->setText(tr("Join A Domain"));
    }



#else
    ui.pushButtonShowAdmin->setEnabled(false);
    ui.pushButtonShowAdmin->hide();
//    ui.pushButtonRenameComputer->setEnabled(false);
//    ui.pushButtonRenameComputer->hide();
//    ui.pushButtonDomain->setEnabled(false);
//    ui.pushButtonDomain->hide();

    int index = ui.tabWidget->indexOf(ui.tabLocalManagement);
    ui.tabWidget->removeTab(index);
    ui.tabLocalManagement->setEnabled(false);
    //        ui.groupBoxAdministrationTools->setEnabled(false);;
    //        ui.groupBoxAdministrationTools->hide();



#endif


    administratorsManagementMenu = 0;



    ui.groupBoxBasicSettings->setEnabled(false);
    ui.groupBoxOtherSettings->setEnabled(false);
    //ui.groupBoxSettings->hide();

    ui.horizontalLayoutCommand->setEnabled(false);


    //ui.tabSystemInfo->setEnabled(false);
    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    QHeaderView *view = ui.tableWidgetSoftware->horizontalHeader();
    view->resizeSection(0, 300);
    view->setVisible(true);


    ui.tabRemoteManagement->setEnabled(false);


    queryModel = 0;


    setRTP(rtp);
    m_peerSocket = INVALID_SOCK_ID;

    m_aboutToCloseSocket = false;


    setControlCenterPacketsParser(parser);

    clientResponseAdminConnectionResultPacketReceived = false;

    remoteConsoleRunning = false;


    //No editing possible.
    ui.tableWidgetSoftware->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    m_fileManager = 0;


    m_fileManagementWidget = qobject_cast<FileManagement *>(ui.tabFileManagement);
    Q_ASSERT(m_fileManagementWidget);
    if(!localComputer){
        m_fileManagementWidget->setPacketsParser(parser);
    }


//    connect(m_fileManagementWidget, SIGNAL(signalShowRemoteFiles(const QString &)), this, SLOT(requestFileSystemInfo(const QString &)));
//    connect(m_fileManagementWidget, SIGNAL(signalUploadFilesToRemote(const QStringList &, const QString &)), this, SLOT(requestUploadFilesToRemote(const QStringList &, const QString &)));
//    connect(m_fileManagementWidget, SIGNAL(signalDownloadFileFromRemote(const QStringList &, const QString &)), this, SLOT(requestDownloadFileFromRemote(const QStringList &, const QString &)));

    m_updateTemperaturesTimer = 0;

}

SystemManagementWidget::~SystemManagementWidget()
{

    qDebug()<<"~SystemManagementWidget()";

    if(m_updateTemperaturesTimer){
        m_updateTemperaturesTimer->stop();
        delete m_updateTemperaturesTimer;
    }

}

void SystemManagementWidget::closeEvent(QCloseEvent *event){
    qDebug()<<"--SystemManagementWidget::closeEvent(...)";

#ifdef Q_OS_WIN32
    if(!adminProcesses.isEmpty()){
        QString msg = tr("<p>Please make sure that these processes run as admin are closed! Or that raises the issue of security!</p>");
        msg += adminProcesses.join(", ");
        QMessageBox::warning(this, tr("Warning"), msg);
    }

#endif

    controlCenterPacketsParser->disconnect(this);

    if(queryModel){
        queryModel->clear();
        delete queryModel;
        queryModel = 0;
    }

    m_aboutToCloseSocket = true;
    m_rtp->closeSocket(m_peerSocket);
    m_peerSocket = INVALID_SOCK_ID;

    event->accept();

}

//void SystemManagementWidget::dragEnterEvent(QDragEnterEvent *event){

//    if (event->mimeData()->hasUrls()) {
//        event->acceptProposedAction();
//    }
//}

//void SystemManagementWidget::dragMoveEvent(QDragMoveEvent *event)
//{
//    // Accept file actions with all extensions.

////    if (event->mimeData()->hasUrls()) {
//        event->acceptProposedAction();
////    }

//}

//void SystemManagementWidget::dropEvent(QDropEvent *event)
//{

//    // Accept drops if the file exists.

//    const QMimeData *mimeData = event->mimeData();
//    QStringList files;
//    if (mimeData->hasUrls()) {
//        QList<QUrl> urlList = mimeData->urls();
//        foreach (QUrl url, urlList) {
//            if (url.isValid() && url.scheme().toLower() == "file" ){
//                QString fileName = url.path().remove(0, 1);
//                //QFileInfo fi(fileName);
//                //QMessageBox::information(this, fileName, fileName);
//                if (QFile::exists(fileName)){
//                    files.append(fileName);
//                }
//            }
//        }

//    }

//    event->acceptProposedAction();

//    if (!files.isEmpty()){
//        filesDropped(files);
//    }

//}


void SystemManagementWidget::setRTP(RTP *rtp){

    if(!rtp){
        ui.toolButtonVerify->setEnabled(false);
        return;
    }

    this->m_rtp = rtp;
    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)));
}

//void SystemManagementWidget::setUDTProtocol(UDTProtocol *udtProtocol){

//    if(!udtProtocol){
//        ui.toolButtonVerify->setEnabled(false);
//        return;
//    }

//    this->m_udtProtocol = udtProtocol;
//    connect(m_udtProtocol, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));

//}

//void SystemManagementWidget::setTCPServer(TCPServer *tcpServer){
//    if(!tcpServer){
//        ui.toolButtonVerify->setEnabled(false);
//        return;
//    }

//    this->m_tcpServer = tcpServer;
//    connect(m_tcpServer, SIGNAL(disconnected(int)), this, SLOT(peerDisconnected(int)));


//}

void SystemManagementWidget::setControlCenterPacketsParser(ControlCenterPacketsParser *parser){

    if(!parser){
        ui.toolButtonVerify->setEnabled(false);
        return;
    }

    this->controlCenterPacketsParser = parser;
    connect(controlCenterPacketsParser, SIGNAL(signalClientOnlineStatusChanged(SOCKETID, const QString&, bool)), this, SLOT(processClientOnlineStatusChangedPacket(SOCKETID, const QString&, bool)), Qt::QueuedConnection);
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseAdminConnectionResultPacketReceived(SOCKETID, const QString &, bool, const QString &)), this, SLOT(processClientResponseAdminConnectionResultPacket(SOCKETID, const QString &, bool, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientMessagePacketReceived(const QString &, const QString &, quint8)), this, SLOT(clientMessageReceived(const QString &, const QString &, quint8)));
    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientSummaryInfoPacketReceived(SOCKETID, const QByteArray &)), this, SLOT(clientResponseClientSummaryInfoPacketReceived(SOCKETID, const QByteArray &)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseClientDetailedInfoPacketReceived(const QString &, const QByteArray &)), this, SLOT(clientDetailedInfoPacketReceived(const QString &, const QByteArray &)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)), this, SLOT(clientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)));
    connect(controlCenterPacketsParser, SIGNAL(signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)), this, SLOT(remoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserOnlineStatusChanged(const QString&, const QString&,  bool)), this, SLOT(userOnlineStatusChangedPacketReceived(const QString&, const QString&, bool)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)), this, SLOT(userResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)));
    

    connect(controlCenterPacketsParser, SIGNAL(signalTemperaturesPacketReceived(const QString &, const QString &)), this, SLOT(updateTemperatures(const QString &, const QString &)));
    connect(controlCenterPacketsParser, SIGNAL(signalScreenshotPacketReceived(const QString &, const QByteArray &)), this, SLOT(updateScreenshot(const QString &, const QByteArray &)));


//    ////////////////////
//    connect(controlCenterPacketsParser, SIGNAL(signalFileSystemInfoReceived(int, const QString &, const QByteArray &)), this, SLOT(fileSystemInfoReceived(int, const QString &, const QByteArray &)));
//    //File TX
//    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestUploadFile(int, const QByteArray &, const QString &, quint64, const QString &)), this, SLOT(processPeerRequestUploadFilePacket(int, const QByteArray &, const QString &,quint64, const QString &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalAdminRequestDownloadFile(int,QString)), this, SLOT(processPeerRequestDownloadFilePacket(int,QString)), Qt::QueuedConnection);

//    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64)), this, SLOT(fileDownloadRequestAccepted(int, const QString &, const QByteArray &, quint64)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileDownloadRequestDenied(int , const QString &, const QString &)), this, SLOT(fileDownloadRequestDenied(int , const QString &, const QString &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), this, SLOT(fileUploadRequestResponsed(int, const QByteArray &, bool, const QString &)), Qt::QueuedConnection);

//    connect(controlCenterPacketsParser, SIGNAL(signalFileDataRequested(int, const QByteArray &, int, int )), this, SLOT(processFileDataRequestPacket(int,const QByteArray &, int, int )), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileDataReceived(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), this, SLOT(processFileDataReceivedPacket(int, const QByteArray &, int, const QByteArray &, const QByteArray &)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileTXStatusChanged(int, const QByteArray &, quint8)), this, SLOT(processFileTXStatusChangedPacket(int, const QByteArray &, quint8)), Qt::QueuedConnection);
//    connect(controlCenterPacketsParser, SIGNAL(signalFileTXError(int , const QByteArray &, quint8 , const QString &)), this, SLOT(processFileTXErrorFromPeer(int , const QByteArray &, quint8 , const QString &)), Qt::QueuedConnection);


    ui.toolButtonVerify->setEnabled(true);

}

void SystemManagementWidget::on_toolButtonVerify_clicked(){

    if(!verifyPrivilege()){
        return;
    }

    m_peerIPAddress = QHostAddress(ui.lineEditIPAddress->text().trimmed());
    if(localComputer){
        this->m_peerIPAddress = QHostAddress::LocalHost;
    }
    if(m_peerIPAddress.isNull()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid IP Address!"));
        ui.lineEditIPAddress->setFocus();
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = false;

    ui.toolButtonVerify->setEnabled(false);

    QString errorMessage;
    if(m_peerSocket == INVALID_SOCK_ID){
        m_peerSocket = m_rtp->connectToHost(m_peerIPAddress, UDT_LISTENING_PORT, 5000, &errorMessage, RTP::Protocol(ui.comboBoxProtocol->itemData(ui.comboBoxProtocol->currentIndex()).toUInt()));
    }
    if(m_peerSocket == INVALID_SOCK_ID){
        QMessageBox::critical(this, tr("Error"), tr("Can not connect to host!<br>%1").arg(errorMessage));
        ui.toolButtonVerify->setEnabled(true);
        return;
    }

    bool ok = controlCenterPacketsParser->sendAdminRequestConnectionToClientPacket(m_peerSocket, QHostInfo::localHostName(), m_adminName);
    if(!ok){
        QString err = m_rtp->lastErrorString();
        m_rtp->closeSocket(m_peerSocket);
        m_peerSocket = INVALID_SOCK_ID;
        QMessageBox::critical(this, tr("Error"), tr("Can not send connection request to host!<br>%1").arg(err));
        ui.toolButtonVerify->setEnabled(true);
        return;
    }

    QTimer::singleShot(60000, this, SLOT(requestConnectionToClientTimeout()));

}

void SystemManagementWidget::on_pushButtonUSBSD_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }


    MS::USBSTORStatus usbSTORStatus = m_clientInfo.getUsbSDStatus();

    QString text = tr("Do you really want to <font color = 'red'><b>%1</b></font> the USB SD on the computer?").arg((usbSTORStatus == MS::USBSTOR_ReadWrite)?tr("disable"):tr("enable"));
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    bool m_temporarilyAllowed = true;
    if(!usbSTORStatus){
        m_temporarilyAllowed = temporarilyAllowed();
    }

    bool ok = controlCenterPacketsParser->sendSetupUSBSDPacket(m_peerSocket, usbSTORStatus, m_temporarilyAllowed, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }
    ui.pushButtonUSBSD->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonPrograms_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool programesEnabled = m_clientInfo.getProgramsEnabled();

    QString text = tr("Do you really want to <font color = 'red'><b>%1</b></font> the Programes on the computer?").arg(programesEnabled?tr("disable"):tr("enable"));

    int ret = QMessageBox::question(this, tr("Question"), text,QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool m_temporarilyAllowed = true;
    if(!programesEnabled){
        m_temporarilyAllowed = temporarilyAllowed();
    }

    bool ok = controlCenterPacketsParser->sendSetupProgramesPacket(m_peerSocket, !programesEnabled, m_temporarilyAllowed, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }
    ui.pushButtonPrograms->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonShowAdmin_clicked(){
    QMessageBox::critical(this, tr("Error"), tr("Function Disabled!"));
    return;

    bool ok = controlCenterPacketsParser->sendShowAdminPacket(m_peerSocket, true);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonShowAdmin->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonRemoteAssistance_clicked(){
    
//    if(!verifyPrivilege()){
//        return;
//    }
    
    //ui.pushButtonRemoteAssistance->setEnabled(false);
//    emit requestRemoteAssistance();
    
//    bool ok = controlCenterPacketsParser->sendRemoteAssistancePacket(m_peerSocket, m_computerName, m_adminName);
//    if(!ok){
//        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
//        return;
//    }

}

void SystemManagementWidget::on_actionAddAdmin_triggered(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Select The User"), tr("User:"), m_clientInfo.getUsers().split(","), 0, false, &ok);
    if(!ok || item.isEmpty()){
        return;
    }

    QString text = tr("Do you really want to <font color = 'red'>add <b>%1</b></font> to local administrators group? ").arg(item);
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    ok = controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerSocket, m_peerComputerName, item, true, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonAdminsManagement->setEnabled(false);

}

void SystemManagementWidget::on_actionDeleteAdmin_triggered(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = false;
    QStringList admis = m_clientInfo.getAdministrators().split(",");
    QString item = QInputDialog::getItem(this, tr("Select The User"), tr("User:"), admis, 0, false, &ok);
    if(!ok || item.isEmpty()){
        return;
    }

    QString text = tr("Do you really want to <font color = 'red'>delete <b>%1</b></font> from local administrators group? ").arg(item);
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    //    QHostAddress address = QHostAddress(peerAddress);
    //    if(localComputer){
    //        address = QHostAddress(QHostAddress::LocalHost);
    //    }

    ok = controlCenterPacketsParser->sendModifyAdminGroupUserPacket(m_peerSocket, m_peerComputerName, item, false, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonAdminsManagement->setEnabled(false);

}

void SystemManagementWidget::getNewComputerName(){
    QString location = ui.comboBoxLocation->itemData(ui.comboBoxLocation->currentIndex()).toString();
    QString dept = ui.comboBoxDepartment->itemData(ui.comboBoxDepartment->currentIndex()).toString();
    QString sn = QString::number(ui.spinBoxSN->value()).rightJustified(5, '0');

    QString newName = location + dept + sn;
    ui.lineEditNewComputerName->setText(newName.toUpper());

    ui.pushButtonRenameComputer->setEnabled((newName.size() == 9) && (m_peerComputerName != newName));

}

void SystemManagementWidget::on_pushButtonRenameComputer_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString text = tr("Do you really want to <b><font color = 'red'>rename</font></b> the computer? ");
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    QString newComputerName = ui.lineEditNewComputerName->text();
//    do {
//        newComputerName = QInputDialog::getText(this, tr("Rename Computer"), tr("New Computer Name:"), QLineEdit::Normal, m_computerName, &ok).trimmed();
//        if (ok){
//            if(newComputerName.isEmpty()){
//                QMessageBox::critical(this, tr("Error"), tr("Incorrect Computer Name!"));
//            }else{
//                break;
//            }
//        }

//    } while (ok);

    if(newComputerName.isEmpty()){
        return;
    }

    QString domainAdminName = "",  domainAdminPassword = "";
    if(m_isJoinedToDomain){
        bool ok = false;
        domainAdminName = QInputDialog::getText(this, tr("Authentication Required"),
                                                tr("Domain Admin Name:"), QLineEdit::Normal,
                                                "", &ok);

        if(ok && !domainAdminName.isEmpty()){
            ok = false;
            do {
                domainAdminPassword = QInputDialog::getText(this, tr("Authentication Required"),
                                                  tr("Domain Admin Password:"), QLineEdit::Password,
                                                  "", &ok);
                if (!ok){
                    return;
                } if(domainAdminName.isEmpty()){
                    QMessageBox::critical(this, tr("Error"), tr("Domain admin password is required!"));
                }else{
                    break;
                }
            } while (ok);
        }

    }


    ok = controlCenterPacketsParser->sendRenameComputerPacket(m_peerSocket, m_peerComputerName, newComputerName, m_adminName, domainAdminName, domainAdminPassword);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonRenameComputer->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonDomain_clicked(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }

    QString text = tr("Do you really want to <b><font color = 'red'>join</font></b> the computer to a domain? ");
    if(m_isJoinedToDomain){
        text = tr("Do you really want to <b><font color = 'red'>unjoin</font></b> the computer from the domain? ");
    }
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    QString domainOrWorkgroupName = "";
    if(!m_isJoinedToDomain){
        QString joinType = m_isJoinedToDomain?tr("Workgroup"):tr("Domain");

        do {
            domainOrWorkgroupName = QInputDialog::getText(this, tr("Join To %1").arg(joinType), tr("%1 Name:").arg(joinType), QLineEdit::Normal, m_isJoinedToDomain?"WORKGROUP":DOMAIN_NAME, &ok).trimmed();
            if (ok){
                if(domainOrWorkgroupName.isEmpty()){
                    QMessageBox::critical(this, tr("Error"), tr("Incorrect %1 Name!").arg(joinType));
                }else{
                    break;
                }
            }

        } while (ok);
    }

    if(domainOrWorkgroupName.isEmpty()){
        return;
    }

    QString domainAdminName = "",  domainAdminPassword = "";
    if(m_isJoinedToDomain){
        bool ok = false;
        domainAdminName = QInputDialog::getText(this, tr("Authentication Required"),
                                                tr("Domain Admin Name:"), QLineEdit::Normal,
                                                "", &ok);

        if(ok && !domainAdminName.isEmpty()){
            ok = false;
            do {
                domainAdminPassword = QInputDialog::getText(this, tr("Authentication Required"),
                                                  tr("Domain Admin Password:"), QLineEdit::Password,
                                                  "", &ok);
                if (!ok){
                    return;
                } if(domainAdminName.isEmpty()){
                    QMessageBox::critical(this, tr("Error"), tr("Domain admin password is required!"));
                }else{
                    break;
                }
            } while (ok);
        }
    }

    ok = controlCenterPacketsParser->sendJoinOrUnjoinDomainPacket(m_peerSocket, m_peerComputerName, m_adminName, !m_isJoinedToDomain, domainOrWorkgroupName, domainAdminName, domainAdminPassword);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.pushButtonDomain->setEnabled(false);

}

void SystemManagementWidget::on_pushButtonMMC_clicked(){

    //runProgrameAsAdmin("mmc.exe");
    runProgrameAsAdmin(m_winDirPath + "/system32/mmc.exe", "compmgmt.msc");
}

void SystemManagementWidget::on_pushButtonCMD_clicked(){

    runProgrameAsAdmin("cmd.exe");

}

void SystemManagementWidget::on_pushButtonRegedit_clicked(){

    runProgrameAsAdmin(m_winDirPath + "/regedit.exe");
}

void SystemManagementWidget::on_pushButtonOtherEXE_clicked(){

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an EXE File to run as admin:"), "", tr("EXE (*.exe);;All(*.*)"));

    if (fileName.isEmpty()) {
        return;
    }

    runProgrameAsAdmin(fileName);

}

void SystemManagementWidget::on_toolButtonQuerySystemInfo_clicked(){

    resetSystemInfo();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    DatabaseConnecter dc(this);
    if(!dc.isDatabaseOpened(REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME,
                            REMOTE_SITOY_COMPUTERS_DB_DRIVER,
                            REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST,
                            REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT,
                            REMOTE_SITOY_COMPUTERS_DB_USER_NAME,
                            REMOTE_SITOY_COMPUTERS_DB_USER_PASSWORD,
                            REMOTE_SITOY_COMPUTERS_DB_NAME,
                            HEHUI::MYSQL
                            )){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), tr("Database Connection Failed! Query Failed!"));
        qCritical() << QString("Error: Database Connection Failed! Query Failed!");
        return ;
    }


    QSqlDatabase db;
    db = QSqlDatabase::database(REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME);


    if(!queryModel){
        queryModel = new QSqlQueryModel(this);
    }
    queryModel->clear();

    QString queryString = QString("SELECT s.OS, s.Workgroup, s.Users, d.* FROM summaryinfo s, detailedinfo d WHERE s.ComputerName = '%1' AND d.ComputerName = '%1' ").arg(m_peerComputerName);
    queryModel->setQuery(QSqlQuery(queryString, db));
    QApplication::restoreOverrideCursor();

    QSqlError error = queryModel->lastError();
    if (error.type() != QSqlError::NoError) {
        QString msg = QString("Can not query client info from database! %1 Error Type:%2 Error NO.:%3").arg(error.text()).arg(error.type()).arg(error.number());
        //QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fatal Error"), msg);

        //MySQL数据库重启，重新连接
        if(error.number() == 2006){
            db.close();
            QSqlDatabase::removeDatabase(REMOTE_SITOY_COMPUTERS_DB_CONNECTION_NAME);
            return;
        }

    }

    QSqlRecord record = queryModel->record(0);
    if(record.isEmpty()){
        QMessageBox::critical(this, tr("Fatal Error"), tr("No Record Found!"));
        return;
    }

    ui.osVersionLineEdit->setText(record.value("OS").toString());
    ui.installationDateLineEdit->setText(record.value("InstallationDate").toString());
    ui.computerNameLineEdit->setText(m_peerComputerName);
    ui.workgroupLineEdit->setText(record.value("Workgroup").toString());

    ui.osGroupBox->setEnabled(true);


    ui.cpuLineEdit->setText(record.value("CPU").toString());
    ui.motherboardLineEdit->setText(record.value("MotherboardName").toString());
    ui.memoryLineEdit->setText(record.value("Memory").toString());
    ui.videoCardLineEdit->setText(record.value("Video").toString());
    ui.monitorLineEdit->setText(record.value("Monitor").toString());
    ui.audioLineEdit->setText(record.value("Audio").toString());
    ui.lineEditNetworkAdapter->setText(record.value("NetworkAdapter").toString());

    ui.devicesInfoGroupBox->setEnabled(true);


    queryModel->clear();
    queryString = QString("SELECT SoftwareName, SoftwareVersion, InstallationDate, Publisher FROM installedsoftware WHERE ComputerName = '%1' ").arg(m_peerComputerName);
    queryModel->setQuery(QSqlQuery(queryString, db));
    while (queryModel->canFetchMore()){
        queryModel->fetchMore();
    }
    int rows = queryModel->rowCount();
    ui.tableWidgetSoftware->setRowCount(rows);
    for(int i=0; i<rows; i++){
        QSqlRecord record = queryModel->record(i);
        for(int j=0; j<4; j++){
            ui.tableWidgetSoftware->setItem(i, j, new QTableWidgetItem(record.value(j).toString()));
        }
    }
    queryModel->clear();

    //ui.tableWidgetSoftware->setModel(queryModel);
    ui.labelReportCreationTime->setText(tr("Last Update Time: %1").arg(record.value("UpdateTime").toString()));


    //QApplication::restoreOverrideCursor();

}

void SystemManagementWidget::on_toolButtonRequestSystemInfo_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    bool ok = controlCenterPacketsParser->sendRequestClientDetailedInfoPacket(m_peerSocket, m_peerComputerName, false);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

    QTimer::singleShot(60000, this, SLOT(requestClientInfoTimeout()));

    ui.toolButtonRequestSystemInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    ui.osGroupBox->setEnabled(false);
    ui.devicesInfoGroupBox->setEnabled(false);

}

void SystemManagementWidget::on_toolButtonSaveAs_clicked(){

    QString path = QFileDialog::getSaveFileName(this, tr("File Save Path:"), QDir::homePath() + "/" + m_peerComputerName, tr("INI (*.ini);;All(*.*)"));
    if(path.isEmpty()){
        return;
    }

    QFileInfo fileInfo = QFileInfo(path);
    if(fileInfo.exists()){
        QDir dir = fileInfo.dir();
        dir.remove(fileInfo.fileName());
    }

    QDir::setCurrent(QDir::tempPath());
    QString clientInfoFilePath = QString("./%1.ini").arg(m_peerComputerName);
    if(!QFile::copy(clientInfoFilePath, path)){
        QMessageBox::critical(this, tr("Error"), tr("Can not save file!"));
    }

}

void SystemManagementWidget::on_groupBoxTemperatures_clicked(bool checked){
    if(checked){
        if(!m_updateTemperaturesTimer){
            m_updateTemperaturesTimer = new QTimer(this);
            connect(m_updateTemperaturesTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTemperatures()));
            m_updateTemperaturesTimer->setSingleShot(false);
        }
        m_updateTemperaturesTimer->start(5000);

    }else{
        m_updateTemperaturesTimer->stop();
        delete m_updateTemperaturesTimer;
        m_updateTemperaturesTimer = 0;

        requestUpdateTemperatures();
    }
}

void SystemManagementWidget::requestUpdateTemperatures(){
    bool cpu = ui.checkBoxCPUTemperature->isChecked();
    bool harddisk = ui.checkBoxHarddiskTemperature->isChecked();
    //if(cpu || harddisk){
        controlCenterPacketsParser->sendRequestTemperaturesPacket(m_peerSocket, cpu, harddisk);
    //}
}

void SystemManagementWidget::on_toolButtonRunRemoteApplication_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }
    
    if(remoteConsoleRunning){
        int rep = QMessageBox::question(this, tr("Confirm"), tr("Do you really want to terminate the process?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(rep == QMessageBox::Yes){
            bool ok = controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerSocket, m_peerComputerName, "", m_adminName, false);
            if(!ok){
                QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
                return;
            }
            //ui.toolButtonRunRemoteApplication->setEnabled(false);
        }

    }else{
        if(!verifyPrivilege()){
            return;
        }

        QString remoteAPPPath = ui.comboBoxRemoteApplicationPath->currentText();
        if(!remoteAPPPath.trimmed().isEmpty()){
            bool ok = controlCenterPacketsParser->sendAdminRequestRemoteConsolePacket(m_peerSocket, m_peerComputerName, remoteAPPPath, m_adminName, true);
            if(!ok){
                QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
                return;
            }
        }

        ui.comboBoxRemoteApplicationPath->setEnabled(false);
        ui.toolButtonRunRemoteApplication->setEnabled(false);
        

        ui.toolButtonSendCommand->setEnabled(false);
    }


    

    //QTimer::singleShot(10000, this, SLOT(requestRemoteConsoleTimeout()));




}

void SystemManagementWidget::on_toolButtonSendCommand_clicked(){

//    if(!verifyPrivilege()){
//        return;
//    }

    QString cmd = ui.comboBoxCommand->currentText();
    bool ok = controlCenterPacketsParser->sendRemoteConsoleCMDFromAdminPacket(m_peerSocket, m_peerComputerName, cmd);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.comboBoxCommand->insertItem(0, cmd);

    ui.comboBoxCommand->setEditText("");
    ui.comboBoxCommand->setFocus();

    //ui.textBrowserRemoteApplicationOutput->append(cmd + "\n");

}

void SystemManagementWidget::on_pushButtonRefreshScreenshot_clicked(){

    if(m_onlineUsers.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("No user online!"));
        return;
    }

    QString userName = m_onlineUsers.first();
    bool ok = controlCenterPacketsParser->sendRequestScreenshotPacket(m_peerSocket, userName, true);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }

}

void SystemManagementWidget::processClientOnlineStatusChangedPacket(SOCKETID socketID, const QString &computerName, bool online){
    qDebug()<<"--SystemManagementWidget::processClientOnlineStatusChangedPacket(...)";

    if(socketID != m_peerSocket || computerName != this->m_peerComputerName){
        return;
    }

//    QString ip = "";
//    quint16 port = 0;
//    if(!m_udtProtocol->getAddressInfoFromSocket(socketID, &ip, &port)){
//        qCritical()<<m_rtp->lastErrorString();
//        return;
//    }

    if(!online){
        peerDisconnected(true);
    }

    //qWarning()<<QString("Client %1 %2!").arg(clientName).arg(online?"Online":"Offline");

}

void SystemManagementWidget::processClientResponseAdminConnectionResultPacket(SOCKETID socketID, const QString &computerName, bool result, const QString &message){
    qDebug()<<"SystemManagementWidget::processClientResponseVerifyInfoResultPacket:"<<"computerName:"<<computerName<<" result:"<<result;

    if(socketID != m_peerSocket){
        return;
    }

    clientResponseAdminConnectionResultPacketReceived = true;

    if(result == true){
        ui.lineEditComputerName->setText(computerName);
        //ui.tabSystemInfo->setEnabled(true);
        ui.tabRemoteManagement->setEnabled(true);
        ui.groupBoxBasicSettings->setEnabled(true);
        ui.groupBoxOtherSettings->setEnabled(true);
        ui.groupBoxRemoteConsole->setEnabled(true);

        ui.toolButtonRequestSystemInfo->setEnabled(true);

        ui.groupBoxTemperatures->setEnabled(true);

        m_fileManagementWidget->setPeerSocket(m_peerSocket);
        ui.tabFileManagement->setEnabled(true);

        if(!message.trimmed().isEmpty()){
            QMessageBox::warning(this, tr("Warning"), message);
        }
    }else{
        //ui.tabSystemInfo->setEnabled(false);
        ui.tabRemoteManagement->setEnabled(false);
        ui.groupBoxBasicSettings->setEnabled(false);
        ui.groupBoxOtherSettings->setEnabled(false);
        ui.groupBoxRemoteConsole->setEnabled(false);

        ui.toolButtonRequestSystemInfo->setEnabled(false);

        ui.groupBoxTemperatures->setEnabled(false);

        ui.toolButtonVerify->setEnabled(true);

        //m_fileManagementWidget->setPeerSocket(m_peerSocket);
        ui.tabFileManagement->setEnabled(false);

        QMessageBox::critical(this, tr("Connection Error"), message);
    }

}

void SystemManagementWidget::requestConnectionToClientTimeout(){

    if(!clientResponseAdminConnectionResultPacketReceived){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response received from client!"));
        ui.toolButtonVerify->setEnabled(true);
    }

}

void SystemManagementWidget::clientMessageReceived(const QString &computerName, const QString &message, quint8 clientMessageType){

    if(computerName != this->m_peerComputerName){
        return;
    }

    QString msg = QString(tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName));
    msg += message;
    switch(clientMessageType){
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


void SystemManagementWidget::clientResponseClientSummaryInfoPacketReceived(SOCKETID socketID, const QByteArray &clientInfo){
//const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, quint8 usbSTORStatus, bool programesEnabled, const QString &admins, bool isJoinedToDomain, const QString &clientVersion, const QString &onlineUsers

    if(socketID != m_peerSocket){
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(clientInfo, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject obj = doc.object();

    QString computerName = obj["computerName"].toString();
    QString workgroupName = obj["workgroupName"].toString();
    QString networkInfo = obj["networkInfo"].toString();
    QString usersInfo = obj["usersInfo"].toString();
    QString osInfo = obj["osInfo"].toString();
    quint8 usbSTORStatus = obj["usbSTORStatus"].toInt();
    bool programesEnabled = obj["programesEnabled"].toBool();
    QString admins = obj["admins"].toString();
    bool isJoinedToDomain = obj["isJoinedToDomain"].toBool();
    QString clientVersion = obj["clientVersion"].toString();
    QString onlineUsers = obj["onlineUsers"].toString();


    m_peerComputerName = computerName;
    Q_ASSERT(!m_peerComputerName.trimmed().isEmpty());

    m_clientInfo.setComputerName(computerName);
    m_clientInfo.setWorkgroup(workgroupName);
    m_clientInfo.setNetwork(networkInfo);
    m_clientInfo.setUsers(usersInfo);
    m_clientInfo.setOs(osInfo);
    m_clientInfo.setUsbSDStatus(usbSTORStatus);
    m_clientInfo.setProgramsEnabled(programesEnabled);
    m_clientInfo.setAdministrators(admins);
    m_clientInfo.setClientVersion(clientVersion);
    m_clientInfo.setLastOnlineTime(QDateTime::currentDateTime());
    m_clientInfo.setIsJoinedToDomain(isJoinedToDomain);
    m_clientInfo.setOnlineUsers(onlineUsers);

    //QStringList networkInfoList = networkInfo.split(",").at(0).split("/");
    //    m_peerIPAddress = QHostAddress(networkInfoList.at(0));
    //    if(localComputer){
    //        this->m_peerIPAddress = QHostAddress(QHostAddress::LocalHost);
    //    }

    m_isJoinedToDomain = isJoinedToDomain;

    m_onlineUsers = onlineUsers.split(",");
    ui.pushButtonRefreshScreenshot->setEnabled(!m_onlineUsers.isEmpty());
    qDebug()<<"------------m_onlineUsers:"<<m_onlineUsers;


    //ui.toolButtonRequestSystemInfo->setEnabled(true);

    if(usbSTORStatus == quint8(MS::USBSTOR_ReadWrite)){
        ui.pushButtonUSBSD->setText(tr("Disable USB SD"));
    }else{
        ui.pushButtonUSBSD->setText(tr("Enable USB SD"));
    }
    ui.pushButtonUSBSD->setEnabled(true);

    if(programesEnabled){
        ui.pushButtonPrograms->setText(tr("Disable Programs"));
    }else{
        ui.pushButtonPrograms->setText(tr("Enable Programs"));
    }
    ui.pushButtonPrograms->setEnabled(true);


    if(!administratorsManagementMenu){
        administratorsManagementMenu = new QMenu(this);
        administratorsManagementMenu->addAction(ui.actionAddAdmin);
        administratorsManagementMenu->addAction(ui.actionDeleteAdmin);
    }
    ui.actionAddAdmin->setEnabled(!usersInfo.isEmpty());
    ui.actionDeleteAdmin->setEnabled(!admins.isEmpty());

    ui.pushButtonAdminsManagement->setMenu(administratorsManagementMenu);
    ui.pushButtonAdminsManagement->setEnabled(true);

    //ui.pushButtonRenameComputer->setEnabled(true);
    ui.pushButtonDomain->setEnabled(true);
    m_isJoinedToDomain = isJoinedToDomain;
    if(m_isJoinedToDomain){
        ui.pushButtonAdminsManagement->hide();
        ui.pushButtonDomain->setText(tr("Unjoin The Domain"));
    }else{
        ui.pushButtonAdminsManagement->show();
        ui.pushButtonDomain->setText(tr("Join A Domain"));
    }


    //ui.tabSystemInfo->setEnabled(true);
    ui.toolButtonRequestSystemInfo->setEnabled(true);
    //ui.toolButtonSaveAs->setEnabled(false);

    ui.tabRemoteManagement->setEnabled(true);

    ui.groupBoxBasicSettings->setEnabled(true);
    ui.groupBoxOtherSettings->setEnabled(true);
    //ui.groupBoxSettings->show();

    ui.groupBoxRemoteConsole->setEnabled(true);


}


void SystemManagementWidget::clientDetailedInfoPacketReceived(const QString &computerName, const QByteArray &clientInfo){


    if(this->m_peerComputerName != computerName){
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(clientInfo, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    QJsonObject obj = object["System"].toObject();
    if(!obj.isEmpty()){
        updateSystemInfo(obj);
    }

    QJsonArray array = object["Software"].toArray();
    if(!array.isEmpty()){
        updateSoftwareInfo(array);
    }


}

void SystemManagementWidget::updateSystemInfo(const QJsonObject &obj){

    resetSystemInfo();

    QString os = obj["OS"].toString();
    QString installationDate = obj["InstallDate"].toString();
    QString osKey = obj["Key"].toString();
    QString users = obj["Users"].toString();
    QString computerName = obj["ComputerName"].toString();
    QString workgroup = obj["Workgroup"].toString();

    ui.osVersionLineEdit->setText(os);
    ui.installationDateLineEdit->setText(installationDate);
    ui.lineEditOSKey->setText(osKey);
    ui.computerNameLineEdit->setText(computerName);
    ui.workgroupLineEdit->setText(workgroup);
    ui.lineEditUsers->setText(users);
    ui.osGroupBox->setEnabled(true);


    QString cpu = obj["Processor"].toString();
    QString memory = obj.value("PhysicalMemory").toString();
    QString motherboardName = obj.value("BaseBoard").toString();
    QString video = obj.value("VideoController").toString();
    QString monitor = obj.value("Monitor").toString();
    QString audio = obj.value("SoundDevice").toString();
    QString diskDrive = obj.value("DiskDrive").toString();
    QString networkAdapter = obj.value("NetworkAdapter").toString();


    ui.cpuLineEdit->setText(cpu);
    ui.motherboardLineEdit->setText(motherboardName);
    ui.memoryLineEdit->setText(memory);
    ui.lineEditDiskDrives->setText(diskDrive);
    ui.monitorLineEdit->setText(monitor);
    ui.videoCardLineEdit->setText(video);
    ui.audioLineEdit->setText(audio);
    ui.lineEditNetworkAdapter->setText(networkAdapter);
    ui.devicesInfoGroupBox->setEnabled(true);



    ui.toolButtonRequestSystemInfo->setEnabled(true);
    ui.toolButtonSaveAs->setEnabled(true);


    //slotResetStatusBar(false);
    //statusBar()->showMessage(tr("Done. Press 'Ctrl+S' to upload the data to server!"));

}

void SystemManagementWidget::updateSoftwareInfo(const QJsonArray &array){

    ui.tableWidgetSoftware->clearContents();
    int softwareCount = array.size();
    ui.tableWidgetSoftware->setRowCount(softwareCount);

    for(int i=0;i<softwareCount;i++){
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 4){continue;}
//        qDebug()<<infoArray.at(0).toString();
//        qDebug()<<infoArray.at(1).toString();
//        qDebug()<<infoArray.at(2).toString();
//        qDebug()<<infoArray.at(3).toString();

        for(int j=0; j<4; j++){
            ui.tableWidgetSoftware->setItem(i, j, new QTableWidgetItem(infoArray.at(j).toString()));
        }
    }



}

void SystemManagementWidget::requestClientInfoTimeout(){

    if(!ui.toolButtonRequestSystemInfo->isEnabled()){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));
        ui.toolButtonRequestSystemInfo->setEnabled(true);
    }

}

void SystemManagementWidget::clientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool running, const QString &extraMessage, quint8 messageType){


    if(computerName != this->m_peerComputerName){
        return;
    }

    ui.textBrowserRemoteApplicationOutput->append(extraMessage);

    if(running){
        remoteConsoleRunning = true;
        //        ui.horizontalLayoutRemoteApplication->setEnabled(false);
        //        ui.horizontalLayoutCommand->setEnabled(true);
        ui.comboBoxRemoteApplicationPath->setEnabled(false);
        ui.toolButtonRunRemoteApplication->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setIcon(QIcon(":/icon/resources/images/stop.png"));

        ui.comboBoxCommand->setFocus();
        ui.toolButtonSendCommand->setEnabled(true);

    }else{
        remoteConsoleRunning = false;
        //        ui.horizontalLayoutRemoteApplication->setEnabled(true);
        //        ui.horizontalLayoutCommand->setEnabled(false);
        ui.comboBoxRemoteApplicationPath->setEnabled(true);
        ui.comboBoxRemoteApplicationPath->setFocus();
        ui.toolButtonRunRemoteApplication->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setIcon(QIcon(":/icon/resources/images/start.png"));
        
        ui.toolButtonSendCommand->setEnabled(false);

        //QMessageBox::critical(this, tr("Error"), tr("<p>Message From Computer <b>%1</b> :</p>").arg(computerName) + extraMessage);

    }

    if(!extraMessage.trimmed().isEmpty()){
        clientMessageReceived(computerName, extraMessage, messageType);
    }

}

void SystemManagementWidget::remoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result){


    if(computerName != this->m_peerComputerName){
        return;
    }

    ui.textBrowserRemoteApplicationOutput->append(result);


}

void SystemManagementWidget::requestRemoteConsoleTimeout(){

    if(!remoteConsoleRunning){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));

        ui.comboBoxRemoteApplicationPath->setEnabled(true);
        ui.toolButtonRunRemoteApplication->setEnabled(true);
    }

}

void SystemManagementWidget::userResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept){

    if(!accept){
        QMessageBox::critical(this, tr("Error"), tr("Remote Assistance Rejected By %1!").arg(userName+"@"+computerName));
    }
}

void SystemManagementWidget::userOnlineStatusChangedPacketReceived(const QString &userName, const QString &computerName, bool online){
    if(computerName != this->m_peerComputerName){
        return;
    }

    if(online){
        if(!m_onlineUsers.contains(userName)){
            m_onlineUsers.append(userName);
        }
    }else{
        m_onlineUsers.removeAll(userName);
    }

    ui.pushButtonRefreshScreenshot->setEnabled(!m_onlineUsers.isEmpty());

}

void SystemManagementWidget::updateTemperatures(const QString &cpuTemperature, const QString &harddiskTemperature){

    QStringList temperatures;

    if(!cpuTemperature.trimmed().isEmpty()){
        foreach (QString temperature, cpuTemperature.split(",")) {
            temperatures.append(temperature + "\342\204\203");
        }
    }
    ui.labelCPUTemperature->setText(temperatures.join(" "));
    temperatures.clear();

    if(!harddiskTemperature.trimmed().isEmpty()){
        foreach (QString temperature, harddiskTemperature.split(",")) {
            temperatures.append(temperature + "\342\204\203");
        }
    }
    ui.labelHarddiskTemperature->setText(temperatures.join(" "));
}

void SystemManagementWidget::updateScreenshot(const QString &userName, const QByteArray &screenshot){
    qDebug()<<"--SystemManagementWidget::updateScreenshot(...)";

    QImage image;
    image.loadFromData(screenshot);
    //pixmap.save(QString("%1.jpg").arg(QDateTime::currentDateTime().toTime_t()));
    //qDebug()<<"pixmap.isNull():"<<pixmap.isNull()<<"  screenshot.size():"<<screenshot.size();
    ui.labelScreenshot->setPixmap(QPixmap::fromImage(image));


}

void SystemManagementWidget::peerDisconnected(SOCKETID socketID){

    if(socketID != m_peerSocket){
        return;
    }

    peerDisconnected(m_aboutToCloseSocket);

}

void SystemManagementWidget::peerDisconnected(bool normalClose){
    qDebug()<<"--SystemManagementWidget::peerDisconnected(...) "<<" normalClose:"<<normalClose;

    m_peerSocket = INVALID_SOCK_ID;

    //ui.tabSystemInfo->setEnabled(false);
    ui.toolButtonRequestSystemInfo->setEnabled(false);
    if(ui.osVersionLineEdit->text().trimmed().isEmpty()){
        ui.toolButtonSaveAs->setEnabled(false);
    }

    ui.tabRemoteManagement->setEnabled(false);
    ui.toolButtonVerify->setEnabled(true);

    m_fileManagementWidget->peerDisconnected(normalClose);
    m_fileManagementWidget->setPeerSocket(INVALID_SOCK_ID);
    ui.tabFileManagement->setEnabled(false);

    if(!normalClose){
        QMessageBox::critical(this, tr("Error"), QString("ERROR! Peer %1 Closed Unexpectedly!").arg(m_peerIPAddress.toString()));
    }else{
        QMessageBox::warning(this, tr("Warning"), QString("Warning! Peer %1 Closed!").arg(m_peerIPAddress.toString()));
    }



//    foreach (QByteArray fileMD5, filesList) {
//        m_fileManager->closeFile(fileMD5);
//    }
//    fileTXRequestList.clear();
//    filesList.clear();


}


bool SystemManagementWidget::verifyPrivilege(){

    return true;

    if(m_adminName == "hehui"){
        return true;
    }
    
    bool ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Privilege Required"),
                                             tr("Access Code:"), QLineEdit::NoEcho,
                                             "", &ok);
        if (ok && !text.isEmpty()){
            QString accessCodeString = "hehui";
            accessCodeString.append(QTime::currentTime().toString("hhmm"));
            if(text.toLower() == accessCodeString){
                return true;
            }
        }

        QMessageBox::critical(this, tr("Error"), tr("Incorrect Access Code!"));

    } while (ok);

    return false;

}

bool SystemManagementWidget::temporarilyAllowed(){

    QStringList items;
    items << tr("Provisional Licence") << tr("Perpetual License");

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Select Licence Type"),
                                         tr("Licence Type:"), items, 0, false, &ok);
    if (!ok || item.isEmpty()){
        return true;
    }

    if(item == tr("Provisional Licence")){
        return true;
    }else{
        return false;
    }

}

void SystemManagementWidget::runProgrameAsAdmin(const QString &exeFilePath, const QString &parameters, bool show){

#ifdef Q_OS_WIN32
    if(!verifyPrivilege()){
        return;
    }
    WindowsManagement wm;
    bool ok = wm.runAs("administrator", ".", QString(WIN_ADMIN_PASSWORD), exeFilePath, parameters, show);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), wm.lastError());
        return;
    }

    adminProcesses.append(QFileInfo(exeFilePath).fileName());
#endif

}

void SystemManagementWidget::resetSystemInfo(){


    ui.osVersionLineEdit->clear();
    ui.installationDateLineEdit->clear();
    ui.lineEditOSKey->clear();
    ui.computerNameLineEdit->clear();
    ui.workgroupLineEdit->clear();
    ui.lineEditUsers->clear();
    ui.osGroupBox->setEnabled(false);


    ui.cpuLineEdit->clear();
    ui.motherboardLineEdit->clear();
    ui.memoryLineEdit->clear();
    ui.lineEditDiskDrives->clear();
    ui.monitorLineEdit->clear();
    ui.videoCardLineEdit->clear();
    ui.audioLineEdit->clear();
    ui.lineEditNetworkAdapter->clear();

    ui.devicesInfoGroupBox->setEnabled(false);

    ui.tableWidgetSoftware->clearContents();


    ui.labelReportCreationTime->clear();
    ui.toolButtonSaveAs->setEnabled(false);


}














} //namespace HEHUI


