#include "adminsmanagementwidget.h"
#include "ui_adminsmanagementwidget.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QInputDialog>
#include <QMenu>

#include "admininfowidget.h"


#include "HHSharedGUI/hdataoutputdialog.h"


namespace HEHUI
{


AdminUserInfoModel::AdminUserInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub
}

AdminUserInfoModel::~AdminUserInfoModel()
{
    clearUsers();
}

void AdminUserInfoModel::clearUsers()
{

    if(usersList.isEmpty()) {
        return;
    }

    beginResetModel();

    foreach (AdminUserInfo *user, usersList) {
        delete user;
        user = 0;
    }
    usersList.clear();

    endResetModel();

}

void AdminUserInfoModel::setJsonData(const QByteArray &data)
{
    qDebug() << "--AdminUserInfoModel::setJsonData(...)";

    clearUsers();

    if(data.isEmpty()) {
        qCritical() << "ERROR! Empty user info data.";
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()) {
        return;
    }

    beginResetModel();

    QJsonArray infoArray = object["Administators"].toArray();
    if(infoArray.isEmpty()) {
        return;
    }

    for(int i = 0; i < infoArray.size(); i++) {
        QByteArray data = infoArray.at(i).toString().toUtf8();
        AdminUserInfo *info = new AdminUserInfo();
        info->setJsonData(data);
        usersList.append(info);
    }

    endResetModel();
}

AdminUserInfo *AdminUserInfoModel::getUser(const QModelIndex &index)
{
    if(!index.isValid()) {
        return 0;
    }

    return usersList.at(index.data(Qt::UserRole).toInt());
}

int AdminUserInfoModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return usersList.size();

}

int	 AdminUserInfoModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 9;

}

QVariant AdminUserInfoModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= usersList.size())) {
        return QVariant();
    }

    AdminUserInfo *info = static_cast<AdminUserInfo *> (usersList.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return info->getUserID();
            break;

        case 1:
            return info->getUserName();
            break;

        case 2:
            return info->businessAddress;
            break;

        case 3:
            return info->lastLoginIP;
            break;

        case 4:
            return info->lastLoginPC;
            break;

        case 5:
            return info->lastLoginTime;
            break;

        case 6:
            return info->readonly ? tr("Yes") : tr("No");
            break;

        case 7:
            return info->active ? tr("Yes") : tr("No");
            break;

        case 8:
            return info->remark;
            break;


        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::UserRole) {
        return row;
    }

    return QVariant();

}

QVariant AdminUserInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("User ID"));
            break;
        case 1:
            return QString(tr("User Name"));
            break;

        case 2:
            return QString(tr("Business Address"));
            break;

        case 3:
            return QString(tr("Last Login IP"));
            break;

        case 4:
            return QString(tr("Last Login PC"));
            break;

        case 5:
            return QString(tr("Last Login Time"));
            break;

        case 6:
            return QString(tr("Read-only"));
            break;

        case 7:
            return QString(tr("Active"));
            break;

        case 8:
            return QString(tr("Remark"));
            break;

        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}



/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
AdminsManagementWidget::AdminsManagementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminsManagementWidget)
{
    ui->setupUi(this);

    m_selectedAdmin = 0;

    m_userInfoModel = new AdminUserInfoModel(this);
    ui->tableView->setModel(m_userInfoModel);
    m_userInfoModel->setJsonData(QByteArray());

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedUser(const QModelIndex &)));
    //connect(ui.tableViewUsers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewAdminInfo(const QModelIndex &)));

    QHeaderView *view = ui->tableView->horizontalHeader();
    view->resizeSection(0, 100);
    view->resizeSection(1, 100);
    //view->resizeSection(3, 200);
    view->setVisible(true);

    m_myself = AdminUser::instance();



}

AdminsManagementWidget::~AdminsManagementWidget()
{
    qDebug() << "--AdminsManagementWidget::~AdminsManagementWidget()";


    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(0);
    delete selectionModel;

    delete m_userInfoModel;

    delete ui;
}

void AdminsManagementWidget::setData(const QByteArray &data)
{
    m_selectedAdmin = 0;
    m_userInfoModel->setJsonData(data);
}

void AdminsManagementWidget::on_actionExport_triggered()
{
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();
}

void AdminsManagementWidget::on_actionPrint_triggered()
{
#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif
}



void AdminsManagementWidget::on_actionProperties_triggered()
{
    slotViewAdminInfo(ui->tableView->currentIndex());
}

void AdminsManagementWidget::on_actionCreateNewAccount_triggered()
{
    if(!verifyPrivilege()) {
        return;
    }

    AdminUserInfo info;
    showUserInfoWidget(&info, false);
    QString adminID = info.getUserID();
    if(adminID.isEmpty()) {
        return;
    }

    m_myself->packetsParser()->sendRequestUpdateSysAdminInfoPacket(m_myself->socketConnectedToServer(), adminID, info.getJsonData(true), false);

    QTimer::singleShot(3000, this, SLOT(on_actionRefresh_triggered()));

}

void AdminsManagementWidget::on_actionModifyAccount_triggered()
{

    if(!verifyPrivilege()) {
        return;
    }

    showUserInfoWidget(m_selectedAdmin, false);

    m_myself->packetsParser()->sendRequestUpdateSysAdminInfoPacket(m_myself->socketConnectedToServer(), m_selectedAdmin->getUserID(), m_selectedAdmin->getJsonData(!m_selectedAdmin->getPassword().isEmpty()), false);

    QTimer::singleShot(3000, this, SLOT(on_actionRefresh_triggered()));

}

void AdminsManagementWidget::on_actionDeleteAccount_triggered()
{

    QModelIndex index = ui->tableView->currentIndex();
    if(!index.isValid()) {
        return;
    }

    getSelectedUser(index);
    slotDeleteAdmin();
}


void AdminsManagementWidget::on_actionRefresh_triggered()
{

    if(!verifyPrivilege()) {
        return;
    }

    m_myself->packetsParser()->sendRequestClientInfoPacket(m_myself->socketConnectedToServer(), "", MS::SYSINFO_SYSADMINS);

}

void AdminsManagementWidget::slotViewAdminInfo(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    getSelectedUser(index);
    showUserInfoWidget(m_selectedAdmin, true);

}

void AdminsManagementWidget::slotDeleteAdmin()
{

    if(!verifyPrivilege()) {
        return;
    }

    if(!m_selectedAdmin) {
        return;
    }

    QString adminID = m_selectedAdmin->getUserID();
    if(adminID.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid admin ID"));
        return;
    }

    int ret = QMessageBox::warning(this, tr("Warning"),
                                   tr("<font color=red><b> Deletion is not reversible! <p>Do you want to delete admin '%1'?<p> </b></font>").arg(adminID),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No
                                  );
    if(ret == QMessageBox::No) {
        return;
    }

    m_myself->packetsParser()->sendRequestUpdateSysAdminInfoPacket(m_myself->socketConnectedToServer(), adminID, QByteArray(), true);

    QTimer::singleShot(3000, this, SLOT(on_actionRefresh_triggered()));

}


void AdminsManagementWidget::showUserInfoWidget(AdminUserInfo *adminUser, bool readonly)
{
    qDebug() << "--AdminsManagementWidget::showADUserInfoWidget(...)";

    if(readonly && (!verifyPrivilege()) ) {
        return;
    }

    QDialog dlg(this);
    QVBoxLayout vbl(&dlg);
    vbl.setContentsMargins(1, 1, 1, 1);

    AdminInfoWidget wgt(adminUser, readonly, &dlg);
    connect(&wgt, SIGNAL(signalOK()), &dlg, SLOT(accept()));
    connect(&wgt, SIGNAL(signalCancel()), &dlg, SLOT(reject()));

    vbl.addWidget(&wgt);
    dlg.setLayout(&vbl);
    dlg.updateGeometry();
    if(readonly) {
        dlg.setWindowTitle(tr("Admin Info"));
    } else if(adminUser->getUserID().isEmpty()) {
        dlg.setWindowTitle(tr("Create New Admin"));
    } else {
        dlg.setWindowTitle(tr("Modify Admin Info"));
    }
    dlg.exec();

}

void AdminsManagementWidget::slotShowCustomContextMenu(const QPoint &pos)
{

    QTableView *tableView = qobject_cast<QTableView *> (sender());
    if (!tableView) {
        return;
    }


    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
    if(!m_selectedAdmin) {
        menu.exec(tableView->viewport()->mapToGlobal(pos));
        return;
    }

    menu.addSeparator();
    menu.addAction(ui->actionExport);

#ifndef QT_NO_PRINTER

    ui->actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui->actionPrint);

#endif

    menu.addSeparator();

    QMenu accountMenu(tr("Account"), this);
    accountMenu.addAction(ui->actionProperties);
    accountMenu.addSeparator();
    accountMenu.addAction(ui->actionCreateNewAccount);
    accountMenu.addAction(ui->actionModifyAccount);
    accountMenu.addAction(ui->actionDeleteAccount);
    menu.addMenu(&accountMenu);

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void AdminsManagementWidget::getSelectedUser(const QModelIndex &index)
{


    if(!index.isValid()) {
        m_selectedAdmin = 0;
        return;
    }

    m_selectedAdmin = m_userInfoModel->getUser(index);


    //bool enableExp = ui.tableViewUsers->currentIndex().isValid() && ui.tableViewUsers->selectionModel()->selectedIndexes().size();
    bool enableExp = m_userInfoModel->rowCount();
    ui->actionExport->setEnabled(enableExp);
    ui->actionPrint->setEnabled(enableExp);

    bool enableModify = true;
    bool userSelected = false;
    if(m_selectedAdmin) {
        if(m_selectedAdmin->getUserID().toLower() == "root") {
            enableModify = false;
        }
        userSelected = true;
    }

    ui->actionProperties->setEnabled(userSelected);
    ui->actionCreateNewAccount->setEnabled(true);
    ui->actionDeleteAccount->setEnabled(enableModify);

}

bool AdminsManagementWidget::verifyPrivilege()
{

    if(!m_myself->isAdminVerified()) {
        return false;
    }
    if(m_myself->isReadonly()) {
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;

//    if(m_verified){
//        return true;
//    }

//    bool ok = false;
//    do {
//        QString text = QInputDialog::getText(this, tr("Authentication Required"),
//                                             tr("Authorization Number:"), QLineEdit::NoEcho,
//                                             "", &ok);
//        if (ok && !text.isEmpty()){
//            QString accessCodeString = "";
//            accessCodeString.append(QTime::currentTime().toString("hhmm"));
//            if(text.toLower() == accessCodeString){
//                m_verified = true;
//                return true;
//            }
//        }

//        QMessageBox::critical(this, tr("Error"), tr("Incorrect Authorization Number!"));

//    } while (ok);

//    return false;

}







} //namespace HEHUI
