#ifndef ADMINSMANAGEMENTWIDGET_H
#define ADMINSMANAGEMENTWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>

#include "../adminuser.h"
#include "../../sharedms/adminuserinfo.h"



namespace Ui {
class AdminsManagementWidget;
}

namespace HEHUI {



class AdminUserInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    AdminUserInfoModel(QObject *parent = 0);
    virtual ~AdminUserInfoModel();

    void clearUsers();

    void setJsonData(const QByteArray &data);
    AdminUserInfo * getUser(const QModelIndex & index);


    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:

private:
    QList<AdminUserInfo *> usersList;

};






/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
class AdminsManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminsManagementWidget(QWidget *parent = 0);
    ~AdminsManagementWidget();


signals:


public slots:
    void setData(const QByteArray &data);

private slots:
    void on_actionRefresh_triggered();

    void on_actionExport_triggered();
    void on_actionPrint_triggered();

    void on_actionProperties_triggered();
    void on_actionCreateNewAccount_triggered();
    void on_actionModifyAccount_triggered();
    void on_actionDeleteAccount_triggered();

    void slotViewAdminInfo(const QModelIndex &index);
    void slotDeleteAdmin();

    void showUserInfoWidget(AdminUserInfo *adUser, bool readonly = true);

    void slotShowCustomContextMenu(const QPoint & pos);
    void getSelectedUser(const QModelIndex &index);

    bool verifyPrivilege();


private:
    Ui::AdminsManagementWidget *ui;

    AdminUserInfo *m_selectedAdmin;
    AdminUserInfoModel *m_userInfoModel;

    AdminUser *m_myself;

};

} //namespace HEHUI

#endif // ADMINSMANAGEMENTWIDGET_H
