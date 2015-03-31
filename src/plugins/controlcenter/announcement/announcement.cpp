


#include <QMessageBox>


#include "announcement.h"




namespace HEHUI {



Announcement::Announcement(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.progressBar->hide();

    ui.lineEditAnnouncementID->setText(QDateTime::currentDateTime().toString("Mddhhmmss"));


}


Announcement::~Announcement()
{

} 

void Announcement::getMessageInfo(quint32 *messageID, QString *message, bool *confirmationRequired, int *validityPeriod){

    if(messageID){
        *messageID = ui.lineEditAnnouncementID->text().toUInt();
    }

    if(message){
        *message = ui.textEdit->toPlainText();
    }

    if(confirmationRequired){
        *confirmationRequired = ui.checkBoxConfirmationRequired->isChecked();
    }

    if(validityPeriod){
        *validityPeriod = ui.spinBoxValidityPeriod->value();
    }


}

void Announcement::on_pushButtonSend_clicked(){
    
    QString announcementString = ui.textEdit->toPlainText();
    if(announcementString.trimmed().isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("Invalid Content!"));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm"),
                                    tr("Do you really want to send the content?"),
                                    QMessageBox::Yes|QMessageBox::Cancel,
                                    QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel){
        return;
    }
    
    ui.pushButtonSend->setEnabled(false);
          

    quint32 id = ui.lineEditAnnouncementID->text().toUInt();
    
    //emit signalSendMessage(id, announcementString, ui.checkBoxConfirmationRequired->isChecked(), ui.spinBoxValidityPeriod->value());
    
    emit signalCloseWidget();
    this->close();

}













} //namespace HEHUI
