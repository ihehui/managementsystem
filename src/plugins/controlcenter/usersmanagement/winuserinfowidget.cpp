

#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

#include "winuserinfowidget.h"

#include "HHSharedWindowsManagement/WinUtilities"



namespace HEHUI {


WinUserInfoWidget::WinUserInfoWidget(WinUserInfo *winUser, QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    m_accountName = "";
    m_fullName = "";
    m_comment = "";
    m_sid = "";

    if(winUser){
        m_winUser = *winUser;
    }

    initUI();

}

WinUserInfoWidget::~WinUserInfoWidget()
{

}

void WinUserInfoWidget::on_pushButtonEdit_clicked(){

    if(ui.lineEditFullName->isReadOnly()){
        switchToEditMode();
        return;
    }

    saveChanges();

}

void WinUserInfoWidget::on_pushButtonClose_clicked(){

//    if(!ui.lineEditFullName->isReadOnly()){

//        QString accountName = ui.lineEditSAMAccount->text().trimmed();
//        QString displayName = ui.lineEditFullName->text();
//        QString description = ui.lineEditDescription->text();
//        QString userWorkstations = ui.lineEditUserWorkstations->text().trimmed();
//        QString telephone = ui.lineEditTelephone->text();
//        QString ouString = ui.comboBoxOU->currentText();

//        if(m_accountName != accountName || m_displayName != displayName
//                || m_description != description || m_userWorkstations != userWorkstations
//                || m_telephone != telephone || m_simpleOUString != ouString){

            int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to save changes before quit?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
            if(rep == QMessageBox::Yes){
                saveChanges();
            }

//        }
//    }

    this->close();
    emit signalCloseWidget();

}

void WinUserInfoWidget::on_checkBoxUserMustChangePassword_clicked(){
    if(ui.checkBoxUserMustChangePassword->isChecked()){
        ui.checkBoxUserCannotChangePassword->setChecked(false);
        ui.checkBoxPasswordNeverExpires->setChecked(false);
    }
}

void WinUserInfoWidget::on_checkBoxUserCannotChangePassword_clicked(){
    if(ui.checkBoxUserCannotChangePassword->isChecked()){
        ui.checkBoxUserMustChangePassword->setChecked(false);
    }
}

void WinUserInfoWidget::on_checkBoxPasswordNeverExpires_clicked(){
    if(ui.checkBoxPasswordNeverExpires->isChecked()){
        ui.checkBoxUserMustChangePassword->setChecked(false);
    }
}

void WinUserInfoWidget::saveChanges(){

    int pos = 0;
    QRegExpValidator rxValidator(this);
    QRegExp rx;

    QString accountName = ui.lineEditSAMAccount->text().trimmed();
    rx.setPattern("^\\w+$");
    rxValidator.setRegExp(rx);
    if(rxValidator.validate(accountName, pos) != QValidator::Acceptable){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Account Name!"));
        ui.lineEditSAMAccount->setFocus();
        return ;
    }

    QString fullName = ui.lineEditFullName->text();
    if(fullName.contains(";") || fullName.contains("|")){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Display Name!"));
        ui.lineEditFullName->setFocus();
        return ;
    }

    QString description = ui.lineEditDescription->text();
    if(description.contains(";") || description.contains("|")){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Description!"));
        ui.lineEditDescription->setFocus();
        return ;
    }


    QString password = ui.lineEditPassword->text();
    if(password != ui.lineEditConfirmPassword->text()){
        QMessageBox::critical(this, tr("Error"), tr("Passwords do not match!"));
        return;
    }

    bool ok = false;
    bool saved = true;
    DWORD errorCode = ERROR_SUCCESS;

    if(m_accountName != accountName){
        ok = WinUtilities::createLocalUser(accountName, password, description, &errorCode);
        if(!ok){
            QMessageBox::critical(this, tr("Error"), tr("Failed to create new account! \r\n %1").arg(WinUtilities::WinSysErrorMsg(errorCode)));
            return;
        }

        m_accountName = accountName;
        ui.lineEditSAMAccount->setReadOnly(true);

        ui.lineEditPassword->clear();
        ui.lineEditConfirmPassword->clear();
        password = "";


//        m_sid = m_adsi->AD_GetObjectAttribute(accountName, "objectSid");
//        m_winUser.setAttribute("objectSid", m_sid);

        ui.lineEditSID->setText(m_sid);

        ui.pushButtonEdit->setText(tr("&Save"));
    }

    if(!password.isEmpty()){
        ok = WinUtilities::updateUserPassword(accountName, password, &errorCode);
        if(!ok){
            QMessageBox::critical(this, tr("Error"), QString("Failed to set password for user '%1'! \r\n %2").arg(accountName).arg(WinUtilities::WinSysErrorMsg(errorCode)));
            saved = false;
        }
    }

//    if(m_fullName != fullName){
//        ok = m_adsi->AD_ModifyAttribute(accountName, "displayName", fullName, 0);
//        if(!ok){
//            m_fullName = m_adsi->AD_GetObjectAttribute(accountName, "displayName");
//            ui.lineEditFullName->setText(m_fullName);
//            QMessageBox::critical(this, tr("Error"), tr("Failed to update display name! \r\n %1").arg(WinUtilities::WinSysErrorMsg(errorCode)));
//            saved = false;
//        }else{
//            m_fullName = fullName;
//        }
//    }

//    if(m_description != description){
//        ok = m_adsi->AD_ModifyAttribute(accountName, "description", description, 0);
//        if(!ok){
//            m_description = m_adsi->AD_GetObjectAttribute(accountName, "description");
//            ui.lineEditDescription->setText(m_description);
//            QMessageBox::critical(this, tr("Error"), tr("Failed to update description! \r\n %1").arg(m_adsi->AD_GetLastErrorString()));
//            saved = false;
//        }else{
//            m_description = description;
//        }
//    }


    bool accountDisabled = ui.checkBoxAccountDisabled->isChecked();
    //WinUtilities::getUserAccountState(user) != WindowsManagement::UAS_Enabled

//    if(m_winUser.accountDisabled() != accountDisabled){
//        ok = m_adsi->AD_EnableObject(accountName, !accountDisabled);
//        if(!ok){
//            QMessageBox::critical(this, tr("Error"), QString("Failed to %1 user '%2'! \r\n %3").arg((!accountDisabled)?tr("enable"):tr("disable")).arg(accountName).arg(m_adsi->AD_GetLastErrorString()) );
//            saved = false;
//        }
//    }

//    if(ui.checkBoxUnlockAccount->isChecked()){
//        ok = m_adsi->AD_UnlockObject(accountName);
//        if(!ok){
//            QMessageBox::critical(this, tr("Error"), QString("Failed to unlock user '%1'! \r\n %2").arg(accountName).arg(m_adsi->AD_GetLastErrorString()) );
//            saved = false;
//        }
//    }

//    bool userMustChangePassword = ui.checkBoxUserMustChangePassword->isChecked();
//    if(userMustChangePassword != m_adsi->userMustChangePassword(accountName)){
//        ok = m_adsi->AD_ModifyAttribute(accountName, "pwdLastSet", userMustChangePassword?"0":"-1");
//        if(!ok){
//            QMessageBox::critical(this, tr("Error"), QString("Operation Failed! \r\n %1").arg(accountName).arg(m_adsi->AD_GetLastErrorString()) );
//            saved = false;
//        }
//    }

//    bool userCannotChangePassword = ui.checkBoxUserCannotChangePassword->isChecked();
//    if(userCannotChangePassword != m_adsi->userCannotChangePassword(accountName)){
//        ok = m_adsi->AD_SetUserCannotChangePassword(accountName, userCannotChangePassword);
//        if(!ok){
//            QMessageBox::critical(this, tr("Error"), QString("Operation Failed! \r\n %1").arg(accountName).arg(m_adsi->AD_GetLastErrorString()) );
//            saved = false;
//        }
//    }

//    bool passwordNeverExpires = ui.checkBoxPasswordNeverExpires->isChecked();
//    if(passwordNeverExpires != m_adsi->passwordNeverExpires(accountName)){
//        ok = m_adsi->AD_SetPasswordExpire(accountName, !passwordNeverExpires);
//        if(!ok){
//            QMessageBox::critical(this, tr("Error"), QString("Operation Failed! \r\n %1").arg(accountName).arg(m_adsi->AD_GetLastErrorString()) );
//            saved = false;
//        }
//    }


    if(saved){
        switchToViewMode();
    }

    emit signalChangesSaved();

}


void WinUserInfoWidget::initUI(){
    qDebug()<<"--ADUserInfoWidget::initUI()";

    m_accountName = m_winUser.userName;

    if(m_accountName.isEmpty()){
        switchToCreatingMode();
        return;
    }

    switchToViewMode();

    ui.lineEditSAMAccount->setText(m_accountName);

    m_fullName = m_winUser.fullName;
    ui.lineEditFullName->setText(m_fullName);


    m_comment = m_winUser.comment;
    ui.lineEditDescription->setText(m_comment);


    bool accountDisabled = m_winUser.accountDisabled;
    ui.checkBoxAccountDisabled->setChecked(accountDisabled);

    bool userMustChangePasword = m_winUser.mustChangePassword;
    ui.checkBoxUserMustChangePassword->setChecked(userMustChangePasword);

    bool userCannotChangePassword = m_winUser.cannotChangePassword;
    ui.checkBoxUserCannotChangePassword->setChecked(userCannotChangePassword);

    bool passwordNeverExpires = m_winUser.passwordNeverExpires;
    ui.checkBoxPasswordNeverExpires->setChecked(passwordNeverExpires);


    QStringList groups = m_winUser.groups.split(";");
    ui.checkBoxPowerUsers->setChecked(groups.contains("Power Users", Qt::CaseInsensitive));
    ui.checkBoxRDusers->setChecked(groups.contains("Remote Desktop Users", Qt::CaseInsensitive));
    ui.checkBoxAdministrators->setChecked(groups.contains("Administrators", Qt::CaseInsensitive));

    m_sid = m_winUser.sid;
    ui.lineEditSID->setText(m_sid);

    unsigned int time_t = m_winUser.lastLogonTime_t;
    if(time_t){
        ui.lineEditLastLogon->setText(QDateTime::fromTime_t(time_t).toString("yyyy.MM.dd HH:mm:ss"));
    }

    time_t = m_winUser.lastLogoffTime_t;
    if(time_t){
        ui.lineEditLastLogoff->setText(QDateTime::fromTime_t(time_t).toString("yyyy.MM.dd HH:mm:ss"));
    }

}

void WinUserInfoWidget::switchToCreatingMode(){

    switchToEditMode();

    ui.lineEditSAMAccount->setReadOnly(false);
    ui.lineEditSAMAccount->setFocus();
    ui.checkBoxUnlockAccount->hide();
    ui.groupBoxID->hide();

    ui.pushButtonEdit->setText(tr("&Create"));
}

void WinUserInfoWidget::switchToEditMode(){

    ui.lineEditSAMAccount->setReadOnly(true);
    ui.lineEditFullName->setReadOnly(false);
    ui.lineEditFullName->setFocus();
    ui.lineEditDescription->setReadOnly(false);

    ui.checkBoxAccountDisabled->setEnabled(true);
    ui.checkBoxUnlockAccount->setEnabled(true);

    ui.framePassword->show();
    ui.lineEditPassword->clear();
    ui.lineEditConfirmPassword->clear();
    ui.checkBoxUserMustChangePassword->setEnabled(true);
    ui.checkBoxUserCannotChangePassword->setEnabled(true);
    ui.checkBoxPasswordNeverExpires->setEnabled(true);


    ui.pushButtonEdit->setText(tr("&Save"));

}

void WinUserInfoWidget::switchToViewMode(){

    ui.lineEditSAMAccount->setReadOnly(true);
    ui.lineEditFullName->setReadOnly(true);
    ui.lineEditDescription->setReadOnly(true);
    ui.checkBoxAccountDisabled->setEnabled(false);
    ui.checkBoxUnlockAccount->setEnabled(false);
    ui.checkBoxUnlockAccount->setChecked(false);

    ui.framePassword->hide();
    ui.lineEditPassword->clear();
    ui.lineEditConfirmPassword->clear();
    ui.checkBoxUserMustChangePassword->setEnabled(false);
    ui.checkBoxUserCannotChangePassword->setEnabled(false);
    ui.checkBoxPasswordNeverExpires->setEnabled(false);

    ui.groupBoxID->show();

    ui.pushButtonEdit->setText(tr("&Edit"));
    ui.pushButtonClose->setFocus();

}











} //namespace HEHUI
