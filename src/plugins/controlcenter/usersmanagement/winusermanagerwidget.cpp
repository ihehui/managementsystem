/*
 ****************************************************************************
 * WinUserManagerWidget.cpp
 *
 * Created on: 2012-10-19
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
 * Last Modified on: 2012-10-19
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QInputDialog>
#include <QTime>

#include "winusermanagerwidget.h"
#include "winuserinfowidget.h"
#include "../adminuser.h"


#include "HHSharedGUI/DataOutputDialog"




namespace HEHUI
{

WinUserManagerWidget::WinUserManagerWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    m_selectedWinUser = 0;

    m_userInfoModel = new WinUserInfoModel(this);
    m_sortFilterProxyModel = new WinUserInfoSortFilterProxyModel(this);
    m_sortFilterProxyModel->setSourceModel(m_userInfoModel);
    ui.tableViewUsers->setModel(m_sortFilterProxyModel);
    m_userInfoModel->setJsonData(QByteArray());


    activityTimer = new QTimer(this);
    activityTimer->setSingleShot(false);
    activityTimer->setInterval(120000); //2minutes
    connect(activityTimer, SIGNAL(timeout()), this, SLOT(activityTimeout()));
    activityTimer->start();

    m_verified = true;

    this->installEventFilter(this);
    ui.tableViewUsers->installEventFilter(this);

    connect(ui.tableViewUsers, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui.tableViewUsers, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedUser(const QModelIndex &)));
    //connect(ui.tableViewUsers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui.tableViewUsers, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewWinUserInfo(const QModelIndex &)));

    QHeaderView *view = ui.tableViewUsers->horizontalHeader();
    view->resizeSection(0, 100);
    view->resizeSection(1, 100);
    //view->resizeSection(3, 200);
    view->setVisible(true);



}

WinUserManagerWidget::~WinUserManagerWidget()
{
    qDebug() << "--WinUserManagerWidget::~WinUserManagerWidget()";

    activityTimer->stop();
    delete activityTimer;
    activityTimer = 0;

}

bool WinUserManagerWidget::eventFilter(QObject *obj, QEvent *event)
{

    switch(event->type()) {
    case QEvent::KeyRelease: {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);

        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
            getSelectedUser(ui.tableViewUsers->currentIndex());
        }

        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O) {
            slotExportQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P) {
            slotPrintQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_E) {
            //getSelectedADUser(ui.tableViewUsers->currentIndex());
            slotViewWinUserInfo(ui.tableViewUsers->currentIndex());
        }
//        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Return){
//            on_toolButtonQueryAD_clicked();
//        }

        activityTimer->start();
        return true;
    }
    break;
    case QEvent::MouseButtonPress:
    case QEvent::Leave: {
        activityTimer->start();
        //return QObject::eventFilter(obj, event);
    }
    break;
    //    case QEvent::ToolTip:
    //    {
    //        if(obj == ui.userPSWDLineEdit){
    //            QString pwd = ui.userPSWDLineEdit->text();
    //            if(pwd.isEmpty()){pwd = tr("Password");}
    //            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    //            QString tip = QString("<b><h1>%1</h1></b>").arg(pwd);
    //            QToolTip::showText(helpEvent->globalPos(), tip);
    //            return true;
    //        }

    //    }
    //        break;
    default:
        break;
        //return QObject::eventFilter(obj, event);


    }

    return QObject::eventFilter(obj, event);

}

void WinUserManagerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}

void WinUserManagerWidget::setData(const QByteArray &data)
{
    m_selectedWinUser = 0;
    m_userInfoModel->setJsonData(data);
}

bool WinUserManagerWidget::isActive()
{
    return m_userInfoModel->rowCount();
}

void WinUserManagerWidget::on_actionExport_triggered()
{
    slotExportQueryResult();
}

void WinUserManagerWidget::on_actionPrint_triggered()
{
    slotPrintQueryResult();
}



void WinUserManagerWidget::on_actionProperties_triggered()
{
    slotViewWinUserInfo(ui.tableViewUsers->currentIndex());
}

void WinUserManagerWidget::on_actionCreateNewAccount_triggered()
{
    slotCreateUser(0);
}

void WinUserManagerWidget::on_actionDeleteAccount_triggered()
{

    QModelIndex index = ui.tableViewUsers->currentIndex();
    if(!index.isValid()) {
        return;
    }

    getSelectedUser(index);
    slotDeleteUser();
}

void WinUserManagerWidget::on_actionSendMessage_triggered()
{
    emit signalSendMessageToUser(m_selectedWinUser->userName);
}

void WinUserManagerWidget::on_actionMonitorDesktop_triggered()
{
    emit signalMonitorUserDesktop(m_selectedWinUser->userName);
}

void WinUserManagerWidget::on_actionLogoff_triggered()
{
    emit signalLockWindows(m_selectedWinUser->userName, true);
}

void WinUserManagerWidget::on_actionLockDesktop_triggered()
{
    emit signalLockWindows(m_selectedWinUser->userName, false);
}

void WinUserManagerWidget::on_actionRefresh_triggered()
{
    slotRefresh();
}

void WinUserManagerWidget::slotExportQueryResult()
{

    DataOutputDialog dlg(ui.tableViewUsers, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void WinUserManagerWidget::slotPrintQueryResult()
{

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.tableViewUsers, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

void WinUserManagerWidget::slotViewWinUserInfo(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    getSelectedUser(index);
    showUserInfoWidget(m_selectedWinUser);

}

void WinUserManagerWidget::slotCreateUser(WinUserInfo *adUser)
{
    showUserInfoWidget(adUser, true);
}

void WinUserManagerWidget::slotDeleteUser()
{

    if(!verifyPrivilege()) {
        return;
    }

    if(!m_selectedWinUser) {
        return;
    }

    QString sAMAccountName = m_selectedWinUser->userName;
    if(sAMAccountName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to find SAM AccountName"));
        return;
    }

    int ret = QMessageBox::warning(this, tr("Warning"),
                                   tr("<font color=red><b> Deletion is not reversible! <p>Do you want to delete user '%1'?<p> </b></font>").arg(sAMAccountName),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No
                                  );
    if(ret == QMessageBox::No) {
        return;
    }

    if(m_selectedWinUser->loggedon) {
        emit signalLockWindows(m_selectedWinUser->userName, true);
    }
    emit signalDeleteUser(sAMAccountName);

    QTimer::singleShot(3000, this, SLOT(slotRefresh()));

}

void WinUserManagerWidget::slotRefresh()
{
    emit signalGetUsersInfo();
}

void WinUserManagerWidget::showUserInfoWidget(WinUserInfo *adUser, bool creareNewUser)
{
    qDebug() << "--WinUserManagerWidget::showADUserInfoWidget(...)";

    if(creareNewUser && !verifyPrivilege()) {
        return;
    }

    QDialog dlg(this);
    QVBoxLayout vbl(&dlg);
    vbl.setContentsMargins(1, 1, 1, 1);

    WinUserInfoWidget wgt(adUser, &dlg);
    connect(&wgt, SIGNAL(signalChangesSaved()), this, SLOT(slotRefresh()));
    connect(&wgt, SIGNAL(signalCloseWidget()), &dlg, SLOT(accept()));
    connect(&wgt, SIGNAL(signalCreateOrModifyWinUser(const QByteArray &)), this, SIGNAL(signalCreateOrModifyWinUser(const QByteArray &)));

    connect(activityTimer, SIGNAL(timeout()), &dlg, SLOT(accept()));

    vbl.addWidget(&wgt);
    dlg.setLayout(&vbl);
    dlg.updateGeometry();
    if(creareNewUser) {
        dlg.setWindowTitle(tr("Create New User"));
    } else {
        dlg.setWindowTitle(tr("User Info"));
    }
    dlg.exec();

}

void WinUserManagerWidget::slotResetUserPassword()
{


    if(!verifyPrivilege()) {
        return;
    }

    QString sAMAccountName = m_selectedWinUser->userName;
    if(sAMAccountName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to find SAM AccountName"));
        return;
    }

    QString newPassword = "";
    bool ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Reset Password"),
                                             tr("New Password(8 Characters MIN.):"), QLineEdit::Password,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            newPassword = text.trimmed();
            if(newPassword.size() < 8) {
                QMessageBox::critical(this, tr("Error"), tr("At least 8 characters are required fro the password!"));
            } else {
                break;
            }
        } else {
            return;
        }

    } while (ok);

    ok = false;
    do {
        QString text = QInputDialog::getText(this, tr("Reset Password"),
                                             tr("Confirm Password:"), QLineEdit::Password,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            if(newPassword != text.trimmed() ) {
                QMessageBox::critical(this, tr("Error"), tr("Passwords do not match!"));
            } else {
                break;
            }
        } else {
            return;
        }

    } while (ok);


//    if(!m_adsi->AD_SetPassword(sAMAccountName, newPassword)){
//        QMessageBox::critical(this, tr("Error"), QString("Failed to reset password for user '%1'! \r\n %2").arg(sAMAccountName).arg(m_adsi->AD_GetLastErrorString()) );
//    }else{
//        QMessageBox::information(this, tr("OK"), QString("Password has been reset for user '%1'!").arg(sAMAccountName) );
//    }


}

void WinUserManagerWidget::slotShowCustomContextMenu(const QPoint &pos)
{

    QTableView *tableView = qobject_cast<QTableView *> (sender());
    if (!tableView) {
        return;
    }


    QMenu menu(this);
    menu.addAction(ui.actionRefresh);
    if(!m_selectedWinUser) {
        menu.exec(tableView->viewport()->mapToGlobal(pos));
        return;
    }



    menu.addSeparator();
    menu.addAction(ui.actionExport);

#ifndef QT_NO_PRINTER

    ui.actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui.actionPrint);

#endif


    menu.addSeparator();

    QMenu accountMenu(tr("Account"), this);
    accountMenu.addAction(ui.actionProperties);
    accountMenu.addSeparator();
    accountMenu.addAction(ui.actionCreateNewAccount);
    accountMenu.addAction(ui.actionDeleteAccount);
    menu.addMenu(&accountMenu);

    menu.addSeparator();
    menu.addAction(ui.actionSendMessage);
    menu.addAction(ui.actionMonitorDesktop);

    menu.addSeparator();
    menu.addAction(ui.actionLockDesktop);
    menu.addAction(ui.actionLogoff);


    //#endif

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void WinUserManagerWidget::getSelectedUser(const QModelIndex &index)
{


    if(!index.isValid()) {
        m_selectedWinUser = 0;
        return;
    }

    m_selectedWinUser = m_userInfoModel->getUser(index);


    //bool enableExp = ui.tableViewUsers->currentIndex().isValid() && ui.tableViewUsers->selectionModel()->selectedIndexes().size();
    bool enableExp = m_userInfoModel->rowCount();
    ui.actionExport->setEnabled(enableExp);
    ui.actionPrint->setEnabled(enableExp);

    bool enableModify = true;
    bool userSelected = false;
    bool userLoggedon = false;
    if(m_selectedWinUser) {
        if(m_selectedWinUser->userName .toLower() == "administrator") {
            enableModify = false;
        }
        if(m_selectedWinUser->loggedon) {
            userLoggedon = true;
        }
        userSelected = true;
    }

    ui.actionProperties->setEnabled(userSelected);
    ui.actionCreateNewAccount->setEnabled(true);
    ui.actionDeleteAccount->setEnabled(enableModify);

    ui.actionSendMessage->setEnabled(userLoggedon);
    ui.actionMonitorDesktop->setEnabled(userLoggedon);
    ui.actionLockDesktop->setEnabled(userLoggedon);
    ui.actionLogoff->setEnabled(userLoggedon);

}

void WinUserManagerWidget::activityTimeout()
{
    m_verified = false;
}

bool WinUserManagerWidget::verifyPrivilege()
{

    AdminUser *adminUser = AdminUser::instance();
    if(!adminUser->isAdminVerified()) {
        return false;
    }
    if(adminUser->isReadonly()) {
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
