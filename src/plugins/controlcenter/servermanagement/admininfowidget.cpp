#include "admininfowidget.h"
#include "ui_admininfowidget.h"

#include <QMessageBox>

namespace HEHUI {


AdminInfoWidget::AdminInfoWidget(AdminUserInfo *info, bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminInfoWidget),
    m_admininfo(info)
{
    ui->setupUi(this);

    Q_ASSERT(info);


    QString userID = m_admininfo->getUserID();
    ui->lineEditID->setText(userID);
    ui->lineEditName->setText(m_admininfo->getUserName());
    ui->lineEditAddress->setText(m_admininfo->businessAddress);
    ui->lineEditRemark->setText(m_admininfo->remark);
    ui->checkBoxReadOnly->setChecked(m_admininfo->readonly);
    ui->checkBoxActive->setChecked(m_admininfo->active);

    ui->lineEditIP->setText(m_admininfo->lastLoginIP);
    ui->lineEditPCName->setText(m_admininfo->lastLoginPC);
    ui->lineEditTime->setText(m_admininfo->lastLoginTime);

    setReadonly(readonly);

    if(!userID.isEmpty()){
        ui->lineEditID->setReadOnly(true);
    }

}

AdminInfoWidget::~AdminInfoWidget()
{
    delete ui;
}

void AdminInfoWidget::setReadonly(bool readonly){

    ui->lineEditID->setReadOnly(readonly);
    ui->lineEditName->setReadOnly(readonly);
    ui->lineEditAddress->setReadOnly(readonly);
    ui->lineEditRemark->setReadOnly(readonly);
    ui->checkBoxReadOnly->setEnabled(!readonly);
    ui->checkBoxActive->setEnabled(!readonly);
    ui->lineEditPassword->setReadOnly(readonly);
    ui->lineEditPassword2->setReadOnly(readonly);
    ui->framePassword->setVisible(!readonly);

    ui->groupBoxLatestLogin->setVisible(readonly);

}

void AdminInfoWidget::on_pushButtonOK_clicked(){
    QString adminID =  m_admininfo->getUserID();
    QString password = ui->lineEditPassword->text();

    if(adminID.isEmpty()){
        adminID = ui->lineEditID->text().trimmed();
        if(adminID.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Invalid admin ID!"));
            ui->lineEditID->setFocus();
            return;
        }
        if(password.size() < 8){
            QMessageBox::critical(this, tr("Error"), tr("At least 8 characters are required fro the password!"));
            ui->lineEditPassword->setFocus();
            return;
        }
        if(password != ui->lineEditPassword2->text()){
            QMessageBox::critical(this, tr("Error"), tr("Passwords do not match!"));
            ui->lineEditPassword2->setFocus();
            return;
        }

        m_admininfo->setUserID(adminID);
        m_admininfo->setPassword(password, true);

    }else{
        if(!password.isEmpty()){
            if(password.size() < 8){
                QMessageBox::critical(this, tr("Error"), tr("At least 8 characters are required fro the password!"));
                ui->lineEditPassword->setFocus();
                return;
            }
            if(password != ui->lineEditPassword2->text()){
                QMessageBox::critical(this, tr("Error"), tr("Passwords do not match!"));
                ui->lineEditPassword2->setFocus();
                return;
            }
            m_admininfo->setPassword(password);
        }

    }

    m_admininfo->setUserName(ui->lineEditName->text());
    m_admininfo->businessAddress = ui->lineEditAddress->text();
    m_admininfo->readonly = ui->checkBoxReadOnly->isChecked();
    m_admininfo->active = ui->checkBoxActive->isChecked();
    m_admininfo->remark = ui->lineEditRemark->text();

    emit signalOK();
}

void AdminInfoWidget::on_pushButtonCancel_clicked(){
    emit signalCancel();
}







} //namespace HEHUI
