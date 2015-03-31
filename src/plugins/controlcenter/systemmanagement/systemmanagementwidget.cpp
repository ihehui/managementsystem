
#include "systemmanagementwidget.h"

#include <QHostInfo>
#include <QHostAddress>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTime>
#include <QSettings>

#include "constants.h"
#include "shutdowndialog.h"
#include "../announcement/announcement.h"


#ifdef Q_OS_WIN32
#include "HHSharedWindowsManagement/hwindowsmanagement.h"
#include "HHSharedWindowsManagement/WinUtilities"
//#include "../../sharedms/global_shared.h"
#endif

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



    ui.horizontalLayoutCommand->setEnabled(false);


    //ui.tabSystemInfo->setEnabled(false);
    ui.toolButtonRequestHardwareInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);



    ui.tabRemoteConsole->setEnabled(false);


    queryModel = 0;


    setRTP(rtp);
    m_peerSocket = INVALID_SOCK_ID;

    m_aboutToCloseSocket = false;


    setControlCenterPacketsParser(parser);

    clientResponseAdminConnectionResultPacketReceived = false;

    remoteConsoleRunning = false;


    //No editing possible.
    //ui.tableWidgetSoftware->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui.tableWidgetServices->setEditTriggers(QAbstractItemView::NoEditTriggers);

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



//    m_shutdownMenu = new QMenu(this);
//    m_shutdownMenu->addAction(ui.actionReboot);
//    m_shutdownMenu->addAction(ui.actionShutdown);
//    ui.toolButtonShutdown->setMenu(m_shutdownMenu);
//    ui.toolButtonShutdown->setDefaultAction(ui.actionReboot);
//    connect(ui.actionReboot, SIGNAL(triggered()), this, SLOT(shutdownSystem()));
//    connect(ui.actionShutdown, SIGNAL(triggered()), this, SLOT(shutdownSystem()));


    m_joinWorkgroupMenu = new QMenu(this);
    m_joinWorkgroupMenu->addAction(ui.actionJoinWorkgroup);
    m_joinWorkgroupMenu->addAction(ui.actionJoinDomain);
    ui.toolButtonChangeWorkgroup->setMenu(m_joinWorkgroupMenu);
    ui.toolButtonChangeWorkgroup->setDefaultAction(ui.actionJoinWorkgroup);
    connect(ui.actionJoinWorkgroup, SIGNAL(triggered()), this, SLOT(changeWorkgroup()));
    connect(ui.actionJoinDomain, SIGNAL(triggered()), this, SLOT(changeWorkgroup()));


    connect(ui.tabSoftware, SIGNAL(signalGetSoftwaresInfo(quint8)), this, SLOT(requestClientInfo(quint8)));
    ui.tabSoftware->setEnabled(false);

    connect(ui.tabUsers, SIGNAL(signalGetUsersInfo(quint8)), this, SLOT(requestClientInfo(quint8)));
    connect(ui.tabUsers, SIGNAL(signalCreateOrModifyWinUser(const QByteArray &)), this, SLOT(requestCreateOrModifyWinUser(const QByteArray &)));
    connect(ui.tabUsers, SIGNAL(signalDeleteUser(const QString &)), this, SLOT(requestDeleteUser(const QString &)));
    connect(ui.tabUsers, SIGNAL(signalLockWindows(const QString &, bool)), this, SLOT(requestLockWindows(const QString &, bool)));
    connect(ui.tabUsers, SIGNAL(signalSendMessageToUser(QString)), this, SLOT(requestSendMessageToUser(const QString &)));
    ui.tabUsers->setEnabled(false);

    connect(ui.tabServices, SIGNAL(signalGetServicesInfo(quint8)), this, SLOT(requestClientInfo(quint8)));
    connect(ui.tabServices, SIGNAL(signalChangServiceConfig(const QString &, bool, quint64)), this, SLOT(changServiceConfig(const QString &, bool, quint64)));
    ui.tabServices->setEnabled(false);


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

    connect(controlCenterPacketsParser, SIGNAL(signalClientInfoPacketReceived(const QString &, const QByteArray &, quint8)), this, SLOT(clientInfoPacketReceived(const QString &, const QByteArray &, quint8)));

    connect(controlCenterPacketsParser, SIGNAL(signalClientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)), this, SLOT(clientResponseRemoteConsoleStatusPacketReceived(const QString &, bool, const QString &, quint8)));
    connect(controlCenterPacketsParser, SIGNAL(signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)), this, SLOT(remoteConsoleCMDResultFromClientPacketReceived(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserOnlineStatusChanged(const QString&, const QString&,  bool)), this, SLOT(userOnlineStatusChangedPacketReceived(const QString&, const QString&, bool)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)), this, SLOT(userResponseRemoteAssistancePacketReceived(const QString &, const QString &, bool)));
    

    connect(controlCenterPacketsParser, SIGNAL(signalTemperaturesPacketReceived(const QString &, const QString &)), this, SLOT(updateTemperatures(const QString &, const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalUserReplyMessagePacketReceived(const QString &, const QString &, quint32 , const QString &)), this, SLOT(replyMessageReceived(const QString &, const QString &, quint32 , const QString &)));

    connect(controlCenterPacketsParser, SIGNAL(signalServiceConfigChangedPacketReceived(QString,QString,quint64,quint64)), this, SLOT(serviceConfigChangedPacketReceived(QString,QString,quint64,quint64)));


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

void SystemManagementWidget::on_toolButtonShutdown_clicked(){
    //    if(!verifyPrivilege()){
    //        return;
    //    }


    ShutdownDialog dlg(this);
    if(dlg.exec() != QDialog::Accepted){
        return;
    }

    bool ok = controlCenterPacketsParser->sendRequestShutdownPacket(m_peerSocket, dlg.message(), dlg.timeout(), dlg.forceAppsClosed(), dlg.rebootAfterShutdown());
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

//    QAction *action = qobject_cast<QAction*>(sender());
//    if(!action){return;}

//    bool reboot = true;
//    QString text;
//    if(action == ui.actionReboot){
//        text = tr("reboot");
//        reboot = true;
//    }else{
//        text = tr("shutdown");
//        reboot = false;
//    }
//    text = tr("Do you really want to <b><font color = 'red'>%1</font></b> the computer? ").arg(text);


//    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
//    if(ret == QMessageBox::No){
//        return;
//    }

//    bool ok = controlCenterPacketsParser->sendRequestShutdownPacket(m_peerSocket, reboot, true, 10, "Shutdown.............");
//    if(!ok){
//        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
//        return;
//    }

//    ui.toolButtonShutdown->setEnabled(false);

}

void SystemManagementWidget::on_toolButtonRenameComputer_clicked(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }

    QString text = tr("Do you really want to <b><font color = 'red'>rename</font></b> the computer? ");
    int ret = QMessageBox::question(this, tr("Question"), text, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::No){
        return;
    }

    bool ok = false;
    //QString newComputerName = ui.lineEditNewComputerName->text();
    QString newComputerName = m_peerComputerName;
    do {
        newComputerName = QInputDialog::getText(this, tr("Rename Computer"), tr("New Computer Name:"), QLineEdit::Normal, newComputerName, &ok).trimmed();
        if (ok){
            if(newComputerName.isEmpty()){
                QMessageBox::critical(this, tr("Error"), tr("Incorrect Computer Name!"));
            }else{
                break;
            }
        }

    } while (ok);

    if(newComputerName.isEmpty() || (newComputerName.toLower() == m_peerComputerName.toLower())){
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

    ui.toolButtonRenameComputer->setEnabled(false);

}

void SystemManagementWidget::changeWorkgroup(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }

    QAction *action = qobject_cast<QAction*>(sender());
    if(!action){return;}

    bool joinWorkgroupAction = false;
    if(action == ui.actionJoinWorkgroup){
        joinWorkgroupAction = true;
    }


    QString text = tr("Do you really want to <b><font color = 'red'>join</font></b> the computer to a %1? ").arg(joinWorkgroupAction?tr("workgroup"):tr("domain"));
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
        QString joinType = joinWorkgroupAction?tr("Workgroup"):tr("Domain");

        do {
            domainOrWorkgroupName = QInputDialog::getText(this, tr("Join To %1").arg(joinType), tr("%1 Name:").arg(joinType), QLineEdit::Normal, joinWorkgroupAction?"WORKGROUP":"", &ok).trimmed();
            if (ok){
                if(domainOrWorkgroupName.isEmpty()){
                    QMessageBox::critical(this, tr("Error"), tr("Incorrect %1 Name!").arg(joinType));
                }else{
                    break;
                }
            }

        } while (ok);

        if(domainOrWorkgroupName.isEmpty()){
            return;
        }

    }


    QString domainAdminName = "",  domainAdminPassword = "";
    if(!joinWorkgroupAction){
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

    ok = controlCenterPacketsParser->sendJoinOrUnjoinDomainPacket(m_peerSocket, m_peerComputerName, m_adminName, !joinWorkgroupAction, domainOrWorkgroupName, domainAdminName, domainAdminPassword);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }

    ui.toolButtonChangeWorkgroup->setEnabled(false);

}

void SystemManagementWidget::on_toolButtonSetupUSB_clicked(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }


    if(!ui.checkBoxUSBSDReadable->isEnabled()){
        ui.checkBoxUSBSDReadable->setEnabled(true);
        ui.checkBoxUSBSDWriteable->setEnabled(true);
        ui.toolButtonSetupUSB->setText(tr("Apply"));
        return ;
    }


    MS::USBSTORStatus usbSTORStatus;
    QString str = "";
    bool readable = ui.checkBoxUSBSDReadable->isChecked();
    bool writeable = ui.checkBoxUSBSDWriteable->isChecked();
    if(readable && writeable){
        usbSTORStatus = MS::USBSTOR_ReadWrite;
        str = tr("Read-Write");
    }else if(!readable){
        usbSTORStatus = MS::USBSTOR_Disabled;
        str = tr("Disabled");
    }else{
        usbSTORStatus = MS::USBSTOR_ReadOnly;
        str = tr("Read-Only");
    }

    if(usbSTORStatus == m_clientInfo.getUsbSDStatus()){
        ui.checkBoxUSBSDReadable->setEnabled(false);
        ui.checkBoxUSBSDWriteable->setEnabled(false);
        ui.toolButtonSetupUSB->setText(tr("Modify"));
        return;
    }


    QString text = QString("Do you really want to change USB Storage Device settings to <font color = 'red'><b>'%1'</b></font> ?").arg(str);
    int ret = QMessageBox::question(this, tr("Question"), text,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No
                                    );
    if(ret == QMessageBox::No){
        return;
    }

    bool m_temporarilyAllowed = false;
//    if(!usbSTORStatus){
//        m_temporarilyAllowed = temporarilyAllowed();
//    }

    bool ok = controlCenterPacketsParser->sendSetupUSBSDPacket(m_peerSocket, usbSTORStatus, m_temporarilyAllowed, m_adminName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
        return;
    }
    //ui.pushButtonUSBSD->setEnabled(false);

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
//    ui.tableWidgetSoftware->setRowCount(rows);
//    for(int i=0; i<rows; i++){
//        QSqlRecord record = queryModel->record(i);
//        for(int j=0; j<4; j++){
//            ui.tableWidgetSoftware->setItem(i, j, new QTableWidgetItem(record.value(j).toString()));
//        }
//    }
    queryModel->clear();

    //ui.tableWidgetSoftware->setModel(queryModel);
    ui.labelReportCreationTime->setText(tr("Last Update Time: %1").arg(record.value("UpdateTime").toString()));


    //QApplication::restoreOverrideCursor();

}

void SystemManagementWidget::on_toolButtonRequestHardwareInfo_clicked(){

    //    if(!verifyPrivilege()){
    //        return;
    //    }

    bool ok = controlCenterPacketsParser->sendRequestClientInfoPacket(m_peerSocket, m_peerComputerName, quint8(MS::SYSINFO_HARDWARE));
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!\n%1").arg(m_rtp->lastErrorString()));
        return;
    }


    ui.toolButtonRequestHardwareInfo->setEnabled(false);
    ui.toolButtonSaveAs->setEnabled(false);

    //ui.osGroupBox->setEnabled(false);
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
        m_peerComputerName = computerName;
        ui.computerNameLineEdit->setText(computerName);

        ui.lineEditComputerName->setText(computerName);
        ui.lineEditComputerName->setReadOnly(true);

        ui.tabSystemInfo->setEnabled(true);
        ui.groupBoxTemperatures->setEnabled(true);
        ui.toolButtonRequestHardwareInfo->setEnabled(true);

        ui.tabRemoteConsole->setEnabled(true);
        ui.groupBoxRemoteConsole->setEnabled(true);


        ui.tabSoftware->setEnabled(true);
        ui.tabUsers->setEnabled(true);
        ui.tabServices->setEnabled(true);

        m_fileManagementWidget->setPeerSocket(m_peerSocket);
        ui.tabFileManagement->setEnabled(true);

        if(!message.trimmed().isEmpty()){
            QMessageBox::warning(this, tr("Warning"), message);
        }
    }else{
        //ui.tabSystemInfo->setEnabled(false);
        ui.groupBoxRemoteConsole->setEnabled(false);

        ui.toolButtonRequestHardwareInfo->setEnabled(false);
        ui.groupBoxTemperatures->setEnabled(false);

        ui.toolButtonVerify->setEnabled(true);


        ui.tabRemoteConsole->setEnabled(false);
        ui.tabSoftware->setEnabled(false);
        ui.tabUsers->setEnabled(false);
        ui.tabServices->setEnabled(false);

        ui.tabFileManagement->setEnabled(false);

        QMessageBox::critical(this, tr("Connection failed"), message);
    }

}

void SystemManagementWidget::requestConnectionToClientTimeout(){

    if(!clientResponseAdminConnectionResultPacketReceived){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response received from client!"));
        ui.toolButtonVerify->setEnabled(true);
    }

}

void SystemManagementWidget::clientMessageReceived(const QString &computerName, const QString &message, quint8 clientMessageType){
    qDebug()<<"--SystemManagementWidget::clientMessageReceived(...)"<<" computerName:"<<computerName;

    if(computerName.toLower() != this->m_peerComputerName.toLower()){
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

void SystemManagementWidget::requestClientInfo(quint8 infoType){

    bool ok = controlCenterPacketsParser->sendRequestClientInfoPacket(m_peerSocket, m_peerComputerName, infoType);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }

}

void SystemManagementWidget::clientInfoPacketReceived(const QString &computerName, const QByteArray &data, quint8 infoType){

    qDebug()<<"--SystemManagementWidget::clientInfoPacketReceived(...)"<<"  computerName:"<<computerName<<"  m_peerComputerName:"<<m_peerComputerName<<"  Type:"<<infoType;

    if(this->m_peerComputerName.toLower() != computerName.toLower()){
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){
        qCritical()<<"ERROR! Client info is empty.";
        return;
    }

    switch (infoType) {
    case quint8(MS::SYSINFO_OS):
        m_clientInfo.setJsonData(data);
        updateOSInfo();
        break;

    case quint8(MS::SYSINFO_HARDWARE):
        m_clientInfo.setJsonData(data);
        updateHardwareInfo();
        break;

    case quint8(MS::SYSINFO_SOFTWARE):
        ui.tabSoftware->setData(data);
        break;

    case quint8(MS::SYSINFO_SERVICES):
        ui.tabServices->setData(data);
        break;

    case quint8(MS::SYSINFO_USERS):
        ui.tabUsers->setData(data);
        break;

    default:
        break;
    }

    //    QJsonObject osObj = object["OS"].toObject();
    //    if(!osObj.isEmpty()){
    //        updateOSInfo(osObj);
    //    }

    //    QJsonObject obj = object["Hardware"].toObject();
    //    if(!obj.isEmpty()){
    //        updateSystemInfo(obj);
    //    }

    //    QJsonArray softwareArray = object["Software"].toArray();
    //    if(!softwareArray.isEmpty()){
    //        updateSoftwareInfo(softwareArray);
    //    }

    //    QJsonArray serviceArray = object["Service"].toArray();
    //    if(!serviceArray.isEmpty()){
    //        updateServicesInfo(serviceArray);
    //    }

}

void SystemManagementWidget::updateOSInfo(){

    //OS
    ui.osVersionLineEdit->setText(m_clientInfo.getOs());
    ui.installationDateLineEdit->setText(m_clientInfo.getInstallationDate());
    ui.lineEditOSKey->setText(m_clientInfo.getOsKey());
    ui.computerNameLineEdit->setText(m_clientInfo.getComputerName());
    ui.workgroupLineEdit->setText(m_clientInfo.getWorkgroup());
    ui.lineEditUsers->setText(m_clientInfo.getUsers());
    ui.osGroupBox->setEnabled(true);

    m_peerComputerName = m_clientInfo.getComputerName();
    Q_ASSERT(!m_peerComputerName.trimmed().isEmpty());

    m_isJoinedToDomain = m_clientInfo.isJoinedToDomain();
    if(m_isJoinedToDomain){
        ui.actionJoinWorkgroup->setEnabled(false);
        ui.actionJoinDomain->setText(tr("Unjoin the domain"));
        ui.toolButtonChangeWorkgroup->setDefaultAction(ui.actionJoinDomain);

    }else{
        ui.actionJoinWorkgroup->setEnabled(true);
        ui.actionJoinDomain->setText(tr("Join a domain"));
        ui.toolButtonChangeWorkgroup->setDefaultAction(ui.actionJoinWorkgroup);
    }

    m_onlineUsers = m_clientInfo.getOnlineUsers().split(",");

    return;

}

void SystemManagementWidget::updateHardwareInfo(){

    //Hardware
    ui.cpuLineEdit->setText(m_clientInfo.getCpu());
    ui.motherboardLineEdit->setText(m_clientInfo.getMotherboardName());
    ui.memoryLineEdit->setText(m_clientInfo.getMemory());
    ui.lineEditDiskDrives->setText(m_clientInfo.getStorage());
    ui.monitorLineEdit->setText(m_clientInfo.getMonitor());
    ui.videoCardLineEdit->setText(m_clientInfo.getVideo());
    ui.audioLineEdit->setText(m_clientInfo.getAudio());
    ui.lineEditNetworkAdapter->setText(m_clientInfo.getNetwork());
    ui.devicesInfoGroupBox->setEnabled(true);

    MS::USBSTORStatus status = MS::USBSTOR_Unknown;
    status = m_clientInfo.getUsbSDStatus();
    switch (status) {
    case MS::USBSTOR_ReadWrite:
        ui.checkBoxUSBSDReadable->setChecked(true);
        ui.checkBoxUSBSDWriteable->setChecked(true);
        break;
    case MS::USBSTOR_Disabled:
        ui.checkBoxUSBSDReadable->setChecked(false);
        ui.checkBoxUSBSDWriteable->setChecked(false);
        break;
    case MS::USBSTOR_ReadOnly:
        ui.checkBoxUSBSDReadable->setChecked(true);
        ui.checkBoxUSBSDWriteable->setChecked(false);
        break;
    default:
        break;
    }

    ui.checkBoxUSBSDReadable->setEnabled(false);
    ui.checkBoxUSBSDWriteable->setEnabled(false);

    ui.toolButtonRequestHardwareInfo->setEnabled(true);
}

void SystemManagementWidget::changServiceConfig(const QString &serviceName, bool startService, quint64 startupType){
    bool ok = controlCenterPacketsParser->sendRequestChangeServiceConfigPacket(m_peerSocket, serviceName, startService, startupType);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }
}

void SystemManagementWidget::requestCreateOrModifyWinUser(const QByteArray &userData){
    bool ok = controlCenterPacketsParser->sendRequestCreateOrModifyWinUserPacket(m_peerSocket, userData);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }
}

void SystemManagementWidget::requestDeleteUser(const QString &userName){
    bool ok = controlCenterPacketsParser->sendRequestDeleteUserPacket(m_peerSocket, userName);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }
}


void SystemManagementWidget::requestSendMessageToUser(const QString &userName){

    QDialog dlg(this);
    QVBoxLayout layout(&dlg);
    layout.setContentsMargins(1, 1, 1, 1);
    layout.setSizeConstraint(QLayout::SetFixedSize);

    Announcement wgt(&dlg);
    //connect(&wgt, SIGNAL(signalSendMessage(quint32, const QString &, bool, int)), this, SLOT(sendMessageToUser(quint32, const QString &, bool, int)));
    connect(&wgt, SIGNAL(signalCloseWidget()), &dlg, SLOT(accept()));

    layout.addWidget(&wgt);
    dlg.setLayout(&layout);
    dlg.updateGeometry();
    dlg.setWindowTitle(tr("Announcement"));

    if(dlg.exec() != QDialog::Accepted){
        return;
    }

    quint32 messageID;
    QString message;
    bool confirmationRequired;
    int validityPeriod;
    wgt.getMessageInfo(&messageID, &message, &confirmationRequired, &validityPeriod);

    bool ok = controlCenterPacketsParser->sendAnnouncementPacket(m_peerSocket, m_peerComputerName, userName, m_adminName, messageID, message, confirmationRequired, validityPeriod);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }

}

//void SystemManagementWidget::sendMessageToUser(quint32 messageID, const QString &message, bool confirmationRequired, int validityPeriod){

//    bool ok = controlCenterPacketsParser->sendAnnouncementPacket(m_peerSocket, m_peerComputerName, userName, m_adminName, messageID, message, confirmationRequired, validityPeriod);
//    if(!ok){
//        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
//    }
//}

void SystemManagementWidget::requestLockWindows(const QString &userName, bool logoff){
    bool ok = controlCenterPacketsParser->sendRequestLockWindowsPacket(m_peerSocket, userName, logoff);
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("Can not send data to peer!<br>%1").arg(m_rtp->lastErrorString()));
    }
}



void SystemManagementWidget::requestClientInfoTimeout(){

    if(!ui.toolButtonRequestHardwareInfo->isEnabled()){
        QMessageBox::critical(this, tr("Error"), tr("Timeout! No response from client!"));
        ui.toolButtonRequestHardwareInfo->setEnabled(true);
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

    if(ui.tabUsers->isActive()){
        requestClientInfo(MS::SYSINFO_USERS);
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

void SystemManagementWidget::replyMessageReceived(const QString &computerName, const QString &userName, quint32 originalMessageID, const QString &replyMessage){
    if(computerName != this->m_peerComputerName){
        return;
    }

    QMessageBox::information(this, tr("Message"), QString("Message received from '%1':\r\n%2").arg(userName).arg(replyMessage));

}

void SystemManagementWidget::serviceConfigChangedPacketReceived(const QString &computerName, const QString &serviceName, quint64 processID, quint64 startupType){
    if(computerName != this->m_peerComputerName){
        return;
    }

    ui.tabServices->serviceConfigChanged(serviceName, processID, startupType);

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
    ui.toolButtonRequestHardwareInfo->setEnabled(false);
    if(ui.osVersionLineEdit->text().trimmed().isEmpty()){
        ui.toolButtonSaveAs->setEnabled(false);
    }

    ui.toolButtonVerify->setEnabled(true);

    ui.tabRemoteConsole->setEnabled(false);
    ui.tabSoftware->setEnabled(false);
    ui.tabUsers->setEnabled(false);
    ui.tabServices->setEnabled(false);

    m_fileManagementWidget->peerDisconnected(normalClose);
    m_fileManagementWidget->setPeerSocket(INVALID_SOCK_ID);
    ui.tabFileManagement->setEnabled(false);

    if(!normalClose){
        QMessageBox::critical(this, tr("Error"), QString("ERROR! Peer %1 Closed Unexpectedly!").arg(m_peerIPAddress.toString()));
    }else{
        QMessageBox::warning(this, tr("Warning"), QString("Warning! Peer %1 Closed!").arg(m_peerIPAddress.toString()));
    }

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
    bool ok = WinUtilities::runAs("administrator", ".", QString(WIN_ADMIN_PASSWORD), exeFilePath, parameters, show);
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

    ui.labelReportCreationTime->clear();
    ui.toolButtonSaveAs->setEnabled(false);


}














} //namespace HEHUI


