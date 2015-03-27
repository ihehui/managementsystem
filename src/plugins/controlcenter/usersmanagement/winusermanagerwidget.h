/*
 ****************************************************************************
 * aduserinfowidget.h
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



#ifndef WINUSERINFO_H
#define WINUSERINFO_H

#include <QTimer>


#include "ui_winusermanagerwidget.h"
#include "winuserinfomodel.h"

#include "../../sharedms/global_shared.h"



namespace HEHUI {

class WinUserManagerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit WinUserManagerWidget(QWidget *parent = 0);
    ~WinUserManagerWidget();


protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void changeEvent(QEvent *e);


signals:
    void signalGetUsersInfo(quint8 infoType = MS::SYSINFO_USERS);
    void signalDeleteUser(const QString &userName);
    void signalLockWindows(const QString &userName, bool logoff);


public slots:
    void setData(const QByteArray &data);
    bool isActive();


private slots:

    void on_actionRefresh_triggered();

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_actionProperties_triggered();
    void on_actionCreateNewAccount_triggered();
    void on_actionDeleteAccount_triggered();

    void on_actionLogoff_triggered();
    void on_actionLockDesktop_triggered();

    void slotExportQueryResult();
    void slotPrintQueryResult();

    void slotViewWinUserInfo(const QModelIndex &index);
    void slotCreateADUser(WinUserInfo *adUser);
    void slotDeleteADUser();
    void slotRefresh();

    void showADUserInfoWidget(WinUserInfo *adUser, bool creareNewUser = false);

    void slotResetADUserPassword();


    void slotShowCustomContextMenu(const QPoint & pos);
    void getSelectedUser(const QModelIndex &index);

    void activityTimeout();
    bool verifyPrivilege();





private:
    Ui::WinUserManagerWidgetUI ui;

    WinUserInfo *m_selectedWinUser;


    WinUserInfoModel *m_userInfoModel;
    WinUserInfoSortFilterProxyModel *m_sortFilterProxyModel;

    QTimer *activityTimer;
    bool m_verified;


};

} //namespace HEHUI

#endif // WINUSERINFO_H
