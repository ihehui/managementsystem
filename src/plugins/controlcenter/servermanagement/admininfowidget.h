#ifndef ADMININFOWIDGET_H
#define ADMININFOWIDGET_H

#include <QWidget>
#include "../../sharedms/adminuserinfo.h"


namespace Ui
{
class AdminInfoWidget;
}


namespace HEHUI
{


class AdminInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoWidget(AdminUserInfo *info, bool readonly, QWidget *parent = 0);
    ~AdminInfoWidget();


    void setReadonly(bool readonly = true);


signals:
    void signalOK();
    void signalCancel();


private slots:
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();


private:
    Ui::AdminInfoWidget *ui;

    AdminUserInfo *m_admininfo;



};

}//namespace HEHUI

#endif // ADMININFOWIDGET_H
