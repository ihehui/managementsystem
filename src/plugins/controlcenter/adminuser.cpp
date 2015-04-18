#include "adminuser.h"

#include <QDialog>
#include <QVBoxLayout>

#include "servermanagement/serveraddressmanagerwindow.h"
#include "../../sharedms/settings.h"
#include "constants.h"

namespace HEHUI {

AdminUser *AdminUser::adminUserInstance = 0;

AdminUser * AdminUser::instance(){
    if(adminUserInstance == 0){
        adminUserInstance = new AdminUser();
    }
    return adminUserInstance;
}

AdminUser::AdminUser()
    :UserBase()
{

    m_socketConnectedToServer = INVALID_SOCK_ID;
    m_serverAddress = "";
    m_serverPort = 0;
    Settings settings(SETTINGS_FILE_NAME, "./");
    QStringList lastUsedAppServer = settings.getLastUsedAppServer().split(":");
    if(lastUsedAppServer.size() == 2){
        m_serverAddress = lastUsedAppServer.at(0);
        m_serverPort = lastUsedAppServer.at(1).toUShort();
    }

    m_loginDlg = 0;

    m_readonly = true;

}

AdminUser::~AdminUser()
{

}

void AdminUser::init(RTP *rtp, ControlCenterPacketsParser *controlCenterPacketsParser, QObject *parent){
    Q_ASSERT(rtp);
    Q_ASSERT(controlCenterPacketsParser);

    m_rtp = rtp;
    m_controlCenterPacketsParser = controlCenterPacketsParser;
    setParent(parent);

    connect(m_rtp, SIGNAL(disconnected(SOCKETID)), this, SLOT(peerDisconnected(SOCKETID)));
    connect(m_controlCenterPacketsParser, SIGNAL(signalServerResponseAdminLoginResultPacketReceived(SOCKETID, const QString &, bool, const QString &, bool)), this, SLOT(processLoginResult(SOCKETID, const QString &, bool, const QString &, bool)));

}

bool AdminUser::isReadonly(){
    return m_readonly;
}

SOCKETID AdminUser::socketConnectedToServer(){
    return m_socketConnectedToServer;
}

bool AdminUser::isAdminVerified(){
    if(!isVerified()){
        verifyUser();
    }

    return isVerified();
}

void AdminUser::verifyUser(){

    if(!m_rtp || !m_controlCenterPacketsParser){
        return;
    }

    if(m_serverAddress.isEmpty() || (m_serverPort == 0) ){
        modifyServerSettings();
    }
    if(m_serverAddress.isEmpty() || (m_serverPort == 0) ){
        return;
    }

    if(!m_loginDlg){
        m_loginDlg = new  LoginDlg(this, APP_NAME, true);
        connect(m_loginDlg, SIGNAL(signalModifySettings()), this, SLOT(modifyServerSettings()));
        connect(m_loginDlg, SIGNAL(signalLogin()), this, SLOT(login()));
    }

    m_loginDlg->exec();

}

void AdminUser::modifyServerSettings(){

    QDialog dlg;
    QVBoxLayout vbl(&dlg);
    vbl.setContentsMargins(0, 0, 0, 0);

    ServerAddressManagerWindow smw(&dlg);
    connect(&smw, SIGNAL(signalLookForServer(const QString &, quint16 )), m_controlCenterPacketsParser, SLOT(sendClientLookForServerPacket(const QString &, quint16)));
    connect(m_controlCenterPacketsParser, SIGNAL(signalServerDeclarePacketReceived(const QString&, quint16, quint16, const QString&, const QString&, int)), &smw, SLOT(serverFound(const QString&, quint16, quint16, const QString&, const QString&, int)));
    connect(&smw, SIGNAL(signalServerSelected(const QString &, quint16)), this, SLOT(serverSelected(const QString &, quint16)));

    vbl.addWidget(&smw);
    dlg.setLayout(&vbl);
    dlg.updateGeometry();
    dlg.setWindowTitle(tr("Servers"));
    dlg.setWindowModality(Qt::WindowModal);
    dlg.exec();

}

void AdminUser::serverSelected(const QString &serverAddress, quint16 serverPort){
    if(m_serverAddress != serverAddress || (m_serverPort != serverPort)){
        m_serverAddress = serverAddress;
        m_serverPort = serverPort;
        if(m_socketConnectedToServer != INVALID_SOCK_ID){
            m_rtp->closeSocket(m_socketConnectedToServer);
            m_socketConnectedToServer = INVALID_SOCK_ID;
        }
    }

}

bool AdminUser::connectToServer(const QString &serverAddress, quint16 serverPort){

    QString errorMessage;
    m_socketConnectedToServer = m_rtp->connectToHost(QHostAddress(serverAddress), serverPort, 10000, &errorMessage);
    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        m_loginDlg->setErrorMessage(errorMessage);
        qCritical()<<tr("ERROR! Can not connect to server %1:%2 ! %3").arg(serverAddress).arg(serverPort).arg(errorMessage);
        return false;
    }
    m_serverAddress = serverAddress;
    m_serverPort = serverPort;

    Settings settings(SETTINGS_FILE_NAME, "./");
    settings.setLastUsedAppServer(m_serverAddress + ":" + QString::number(m_serverPort));

    qWarning()<<"Server Connected!"<<" Address:"<<serverAddress<<" Port:"<<m_serverPort;

    return true;
}

bool AdminUser::login(){

    if(m_socketConnectedToServer == INVALID_SOCK_ID){
        connectToServer(m_serverAddress, m_serverPort);
    }

    bool ok = m_controlCenterPacketsParser->sendAdminLoginPacket(m_socketConnectedToServer, getUserID(), getPassword());
    if(!ok){
        m_loginDlg->setErrorMessage(tr("Can not send data to server!"));
    }

    return ok;

}

void AdminUser::processLoginResult(SOCKETID socketID, const QString &serverName, bool result, const QString &message, bool readonly){
    Q_ASSERT(socketID == m_socketConnectedToServer);

    if(result){
        m_loginDlg->accept();
        delete m_loginDlg;
        m_loginDlg = 0;
        emit signalVerified();
    }else{
        m_loginDlg->setErrorMessage(message);
    }

    setVerified(result);
    m_readonly = readonly;
}

void AdminUser::peerDisconnected(SOCKETID socketID){
    qDebug()<<"--AdminUser::peerDisconnected(...) socketID:"<<socketID;

    if(socketID == m_socketConnectedToServer){
        m_socketConnectedToServer = INVALID_SOCK_ID;
        setVerified(false);
        m_readonly = true;
        return;
    }

}














} //namespace HEHUI

