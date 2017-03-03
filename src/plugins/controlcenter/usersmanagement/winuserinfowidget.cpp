

#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include "winuserinfowidget.h"

#ifdef Q_OS_WIN
#include "HHSharedSystemUtilities/WinUtilities"
#else
#include "HHSharedSystemUtilities/UnixUtilities"
#endif


namespace HEHUI {


WinUserInfoWidget::WinUserInfoWidget(WinUserInfo *winUser, QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

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
    this->close();
    emit signalCloseWidget();

}

void WinUserInfoWidget::on_pushButtonClose_clicked(){

    if(!ui.lineEditFullName->isReadOnly()){

        int rep = QMessageBox::question(this, tr("Question"), tr("Do you want to save changes before quit?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        if(rep == QMessageBox::Yes){
            saveChanges();
        }
    }

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

    QString password = ui.lineEditPassword->text();
    if(password != ui.lineEditConfirmPassword->text()){
        QMessageBox::critical(this, tr("Error"), tr("Passwords do not match!"));
        return;
    }

    QJsonObject userObject;
    userObject["UserName"] = accountName;

    if(!password.isEmpty()){
        userObject["Password"] = password;
    }

    //    QString homeDir = ui.lineEditHomeDir->text();
    //    if(homeDir != m_winUser.homeDir){
    //        userObject["HomeDir"] = homeDir;
    //    }

    QString comment = ui.lineEditComment->text();
    if(comment != m_winUser.comment){
        userObject["Comment"] = comment;
    }

    bool accountDisabled = ui.checkBoxAccountDisabled->isChecked();
    if(accountDisabled != m_winUser.accountDisabled){
        userObject["UF_ACCOUNTDISABLE"] = QString::number(accountDisabled);
    }

    bool cannotChangePassword = ui.checkBoxUserCannotChangePassword->isChecked();
    if(cannotChangePassword != m_winUser.cannotChangePassword){
        userObject["UF_PASSWD_CANT_CHANGE"] = QString::number(cannotChangePassword);
    }

    bool unlockAccount = ui.checkBoxUnlockAccount->isChecked();
    if(unlockAccount){
        userObject["UF_LOCKOUT"] = QString::number(unlockAccount);
    }

    bool passwordNeverExpires = ui.checkBoxPasswordNeverExpires->isChecked();
    if(passwordNeverExpires != m_winUser.passwordNeverExpires){
        userObject["UF_DONT_EXPIRE_PASSWD"] = QString::number(passwordNeverExpires);
    }

    QString fullName = ui.lineEditFullName->text();
    if(fullName != m_winUser.fullName){
        userObject["FullName"] = fullName;
    }

//    QString profile = ui.lineEditProfile->text();
//    if(profile != m_winUser.profile){
//        userObject["Profile"] = profile;
//    }

    bool mustChangePassword = ui.checkBoxUserMustChangePassword->isChecked();
    if(mustChangePassword != m_winUser.mustChangePassword){
        userObject["MustChangePassword"] = QString::number(mustChangePassword);
    }

    QStringList groups;
    if(ui.checkBoxGuests->isChecked()){
        groups.append("Guests");
    }
    if(ui.checkBoxPowerUsers->isChecked()){
        groups.append("Power Users");
    }
    if(ui.checkBoxRDusers->isChecked()){
        groups.append("Remote Desktop Users");
    }
    if(ui.checkBoxAdministrators->isChecked()){
        groups.append("Administrators");
    }
    QString otherGroups = ui.lineEditGroupsOther->text().trimmed();
    rx.setPattern("^(\\w+)([\\w\\s]+;[\\w\\s]+)*(\\w+)$");
    rxValidator.setRegExp(rx);
    if(rxValidator.validate(otherGroups, pos) != QValidator::Acceptable){
        QMessageBox::critical(this, tr("Error"), tr("Group names should be separated by semicolon(';')."));
        ui.lineEditGroupsOther->setFocus();
        return ;
    }
    if(!otherGroups.isEmpty()){
        groups.append(otherGroups);
    }
    groups.sort(Qt::CaseInsensitive);
    QString groupString = groups.join(";");
    if(groupString != m_winUser.groups){
        userObject["Groups"] = groupString;
    }

    if(userObject.size() != 1){
        QJsonDocument doc(userObject);
        emit signalCreateOrModifyWinUser(doc.toJson(QJsonDocument::Compact));
    }

//    this->close();
//    emit signalCloseWidget();
}


void WinUserInfoWidget::initUI(){
    qDebug()<<"--ADUserInfoWidget::initUI()";

    QString accountName = m_winUser.userName;

    if(accountName.isEmpty()){
        switchToCreatingMode();
        return;
    }

    switchToViewMode();

    ui.lineEditSAMAccount->setText(accountName);
    ui.lineEditFullName->setText(m_winUser.fullName);
    ui.lineEditComment->setText(m_winUser.comment);

    bool accountDisabled = m_winUser.accountDisabled;
    ui.checkBoxAccountDisabled->setChecked(accountDisabled);

    bool userLocked = m_winUser.accountLocked;
    ui.checkBoxUnlockAccount->setVisible(userLocked);

    bool userMustChangePasword = m_winUser.mustChangePassword;
    ui.checkBoxUserMustChangePassword->setChecked(userMustChangePasword);

    bool userCannotChangePassword = m_winUser.cannotChangePassword;
    ui.checkBoxUserCannotChangePassword->setChecked(userCannotChangePassword);

    bool passwordNeverExpires = m_winUser.passwordNeverExpires;
    ui.checkBoxPasswordNeverExpires->setChecked(passwordNeverExpires);


    QStringList groups = m_winUser.groups.toLower().split(";");
    ui.checkBoxGuests->setChecked(groups.contains("Guests", Qt::CaseInsensitive));
    groups.removeAll("guests");
    ui.checkBoxPowerUsers->setChecked(groups.contains("Power Users", Qt::CaseInsensitive));
    groups.removeAll("power users");
    ui.checkBoxRDusers->setChecked(groups.contains("Remote Desktop Users", Qt::CaseInsensitive));
    groups.removeAll("remote desktop users");
    ui.checkBoxAdministrators->setChecked(groups.contains("Administrators", Qt::CaseInsensitive));
    groups.removeAll("administrators");
    ui.lineEditGroupsOther->setText(groups.join(";"));

    ui.lineEditSID->setText(m_winUser.sid);

    unsigned long time_t = m_winUser.lastLogonTime_t;
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
    ui.lineEditComment->setReadOnly(false);

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
    ui.lineEditComment->setReadOnly(true);
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
