/****************************************************************************
 * servermanagerwindow.cpp
 *
 * Created On: 2010-5-24
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2015-4-16
 * Last Modified By: 贺辉
 ***************************************************************************
 */



#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>


#include "serveraddressmanagerwindow.h"
#include "ui_serveraddressmanagerwindow.h"

#include "../constants.h"

#include "../../sharedms/global_shared.h"
#include "../../sharedms/settings.h"



namespace HEHUI {

ServerAddressManagerWindow::ServerAddressManagerWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ServerAddressManagerWindowClass)
{
    ui->setupUi(this);

    model = new ServerInfoModel(this);
    ui->tableViewServers->setModel(model);
    connect(ui->tableViewServers, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotServerSelected(const QModelIndex &)));

    QHeaderView *view = ui->tableViewServers->horizontalHeader();
    view->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //view->resizeSection(0, 100);
    //view->resizeSection(2, 100);
    view->setVisible(true);

    //    if(clientNetworkManager->getNetworkType() == ClientResourcesManager::LAN){
    ui->lineEditIP->setText(QString(IP_MULTICAST_GROUP_ADDRESS));
    ui->spinBoxPort->setValue(IP_MULTICAST_GROUP_PORT);
    ui->toolButtonSearchServer->show();
    ui->toolButtonSearchServer->setEnabled(true);
    ui->toolButtonSearchServer->setFocus();

    ui->toolButtonAddServer->hide();
    //    }else{
    //        ui->toolButtonSearchServer->hide();
    //        ui->toolButtonAddServer->show();
    //    }


    serversHash.clear();
    slotLoadServers();

    ui->lineEditIP->setFocus();

    //        clientPacketsParser = 0;


}

ServerAddressManagerWindow::~ServerAddressManagerWindow()
{
    slotSaveServers();
    
    delete model;
    model = 0;

    qDeleteAll(serversHash);
    serversHash.clear();
    
    this->disconnect();

    delete ui;

}

void ServerAddressManagerWindow::closeEvent(QCloseEvent *event){
    event->accept();

}

void ServerAddressManagerWindow::updateModel(){
    
    slotSaveServers();
    
    model->setServersList(serversHash.values());
    
    ui->tableViewServers->selectRow(0);
    slotUpdateUI();
    
    //emit signalServersUpdated();

}

bool ServerAddressManagerWindow::isIPAddressValid(){
    QRegExpValidator validator(this);
    // Regexp for IP: ^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$
    QRegExp rx("^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$");
    validator.setRegExp(rx);

    int pos = 0;
    QString ip = ui->lineEditIP->text();

    if(validator.validate(ip, pos) == QValidator::Acceptable){
        quint8 number = ip.section(".", 0, 0).toUShort();
        if((number >= 224 && number <= 239) || (number == 255)){
            ui->toolButtonSearchServer->setEnabled(true);
            ui->toolButtonSearchServer->show();
            ui->toolButtonAddServer->setEnabled(false);
            ui->toolButtonAddServer->hide();
        }else{
            ui->toolButtonAddServer->setEnabled(true);
            ui->toolButtonAddServer->show();
            ui->toolButtonSearchServer->setEnabled(false);
            ui->toolButtonSearchServer->hide();
        }
        return true;
    }else{
        ui->toolButtonAddServer->setEnabled(false);
        ui->toolButtonSearchServer->setEnabled(false);
        QMessageBox::critical(this, tr("Error"), tr("Invalid IP address!"));
        ui->lineEditIP->setFocus();
        ui->lineEditIP->end(false);
        return false;
    }

}

void ServerAddressManagerWindow::serverFound(const QString &serverAddress, quint16 serverRTPListeningPort, quint16 serverTCPListeningPort, const QString &serverName, const QString &version, int serverInstanceID){

    qWarning();
    qWarning()<<"Server Found!"<<" Address:"<<serverAddress<<" RTP Port:"<<serverRTPListeningPort<<" Name:"<<serverName<<" Version:"<<version;
    qWarning();

    ServerInfo *info;
    if(serversHash.contains(serverAddress)){
        info = serversHash.value(serverAddress);
        info->serverPort = serverRTPListeningPort;
    }else{
        info = new ServerInfo(serverAddress, serverRTPListeningPort);
    }

    info->serverName = serverName;
    info->version = version;

    info->currentState = ServerInfo::TestOK;
    serversHash[serverAddress] = info;

    updateModel();

}

void ServerAddressManagerWindow::slotRequestForLANServer(const QString &ip, quint16 port){
    ui->toolButtonSearchServer->setEnabled(false);
    ui->toolButtonTestServers->setEnabled(false);

    emit signalLookForServer(ip, port);
}

void ServerAddressManagerWindow::slotTestServers(){

    ui->toolButtonTestServers->setEnabled(false);
    foreach(ServerInfo *info, serversHash.values()){
        info->currentState = ServerInfo::Testing;
        emit signalLookForServer(info->serverIP, info->serverPort);
    }

    updateModel();

    QTimer::singleShot(5000, this, SLOT(slotTimeout()));

}

void ServerAddressManagerWindow::slotTimeout(){
    foreach(ServerInfo *info, serversHash.values()){
        if(info->currentState != ServerInfo::TestOK){
            info->currentState = ServerInfo::TestFailed;
        }
    }

    updateModel();

    ui->toolButtonTestServers->setEnabled(true);

}


void ServerAddressManagerWindow::slotSaveServers(){
    QStringList serverList;
    foreach(ServerInfo *info, serversHash.values()){
        serverList<<info->serverIP + ":" + QString::number(info->serverPort);
    }

    Settings settings(SETTINGS_FILE_NAME, "./");
    settings.setAppServers(serverList.join(";"));
    settings.sync();

}

void ServerAddressManagerWindow::slotLoadServers(){
    Settings settings(SETTINGS_FILE_NAME, "./");
    QStringList serverList = settings.getAppServers().split(";");

    foreach(QString server, serverList){
        QStringList values = server.split(":");
        if(values.size() != 2){
            continue;
        }
        ServerInfo *info = new ServerInfo(values.at(0), values.at(1).toUInt());
        serversHash.insert(values.at(0), info);
    }

    updateModel();

}

void ServerAddressManagerWindow::slotUpdateUI(){
    if(ui->tableViewServers->currentIndex().isValid()){
        ui->toolButtonDeleteServer->setEnabled(true);
    }else{
        ui->toolButtonDeleteServer->setEnabled(false);
    }

    if(ui->tableViewServers->model()->rowCount()){
        ui->toolButtonTestServers->setEnabled(true);
    }else{
        ui->toolButtonTestServers->setEnabled(false);
    }

}

void ServerAddressManagerWindow::on_toolButtonSearchServer_clicked(){
    if(!isIPAddressValid()){
        return;
    }

    slotRequestForLANServer(ui->lineEditIP->text(), ui->spinBoxPort->value());

}

void ServerAddressManagerWindow::on_toolButtonAddServer_clicked(){
    if(!isIPAddressValid()){
        return;
    }

    QString ip = ui->lineEditIP->text();
    quint16 port = ui->spinBoxPort->value();

    ServerInfo *info;
    if(serversHash.contains(ip)){
        info = serversHash.value(ip);
    }else{
        info = new ServerInfo(ip, port);
    }

    info->currentState = ServerInfo::NotTested;
    serversHash[ip] = info;

    updateModel();
    
    emit signalLookForServer(ip, port);
    
}

void ServerAddressManagerWindow::on_lineEditIP_editingFinished (){

    isIPAddressValid();
}

void ServerAddressManagerWindow::on_toolButtonDeleteServer_clicked(){

    QModelIndex index = ui->tableViewServers->currentIndex();
    if(!index.isValid()){
        return;
    }

    int row = index.row();
    QString ip = index.sibling(row, 0).data().toString();
    ServerInfo *info = serversHash.value(ip);
    serversHash.remove(ip);
    delete info;
    info = 0;
    updateModel();

}

void ServerAddressManagerWindow::on_toolButtonTestServers_clicked(){
    slotTestServers();
}

void ServerAddressManagerWindow::slotServerSelected(const QModelIndex &index){
    
    if(!index.isValid()){
        return;
    }
    
    slotSaveServers();
    
    QStringList server;

    int row = index.row();
    for(int i=0; i<4; i++){
        QModelIndex idx = index.sibling(row, i);
        server << idx.data().toString();
    }
    
    emit signalServerSelected(server.at(0), server.at(1).toUShort(), server.at(2), server.at(3));
    
    QDialog *dlg = qobject_cast<QDialog *> (parentWidget());
    if(dlg){
        dlg->accept();
    }
    
    
}

















} //namespace HEHUI


