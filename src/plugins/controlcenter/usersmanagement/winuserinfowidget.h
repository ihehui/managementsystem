#ifndef WINUSERINFOWIDGET_H
#define WINUSERINFOWIDGET_H

#include <QWidget>

#include "ui_winuserinfowidget.h"
#include "winuserinfo.h"



namespace HEHUI {


//namespace Ui {
//class ADUserInfoWidget;
//}

class WinUserInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit WinUserInfoWidget(WinUserInfo *winUser, QWidget *parent = 0);
    ~WinUserInfoWidget();
    
signals:
    void signalCloseWidget();
    void signalChangesSaved();


private slots:
    void on_pushButtonEdit_clicked();
    void on_pushButtonClose_clicked();

    void on_checkBoxUserMustChangePassword_clicked();
    void on_checkBoxUserCannotChangePassword_clicked();
    void on_checkBoxPasswordNeverExpires_clicked();

    void saveChanges();

private:
    void initUI();
    void switchToCreatingMode();
    void switchToEditMode();
    void switchToViewMode();


private:
    Ui::WinUserInfoWidgetUI ui;

    WinUserInfo m_winUser;

    QString m_accountName;
    QString m_fullName;
    QString m_comment;

    QString m_sid;


};

} //namespace HEHUI

#endif // WINUSERINFOWIDGET_H
