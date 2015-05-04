


#include <QMessageBox>
#include <QMenu>

#include "announcementinfowidget.h"

#include "HHSharedGUI/hdataoutputdialog.h"


namespace HEHUI {



AnnouncementInfoWidget::AnnouncementInfoWidget(bool readonly, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.comboBoxAnnouncementType->addItem(tr("Normal"), QVariant(quint8(MS::ANNOUNCEMENT_NORMAL)));
    ui.comboBoxAnnouncementType->addItem(tr("Critical"), QVariant(quint8(MS::ANNOUNCEMENT_CRITICAL)));
    ui.comboBoxAnnouncementType->setCurrentIndex(0);

    ui.comboBoxTargetType->addItem(tr("Everyone"), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE)));
    ui.comboBoxTargetType->addItem(tr("Computers"), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_COMPUTERS)));
    ui.comboBoxTargetType->addItem(tr("Users"), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_USERS)));
    ui.comboBoxTargetType->setCurrentIndex(0);

    ui.progressBar->hide();

    setReadonly(readonly);

    m_myself = AdminUser::instance();

    m_model = new AnnouncementTargetModel(this);
    ui.tableView->setModel(m_model);
    m_model->setJsonData(QByteArray());

    connect(ui.tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedInfo(const QModelIndex &)));
    //connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewAdminInfo(const QModelIndex &)));

    QHeaderView *view = ui.tableView->horizontalHeader();
    //view->resizeSection(0, 100);
    //view->resizeSection(2, 250);
    //view->resizeSection(3, 200);
    view->setVisible(true);

    m_localTempID = QDateTime::currentDateTime().toTime_t();

}


AnnouncementInfoWidget::~AnnouncementInfoWidget()
{

} 

void AnnouncementInfoWidget::setAnnouncementInfo(const AnnouncementInfo *info){
    if(!info){return;}
    ui.lineEditAnnouncementID->setText(info->ID);
    ui.lineEditAdmin->setText(info->Admin);
    ui.dateTimeEditPublishDate->setDateTime(QDateTime::fromString(info->PublishDate, Qt::ISODate));
    ui.checkBoxActive->setChecked(info->Active);

    ui.comboBoxAnnouncementType->setCurrentIndex(ui.comboBoxAnnouncementType->findData(info->Type));
    ui.spinBoxValidityPeriod->setValue(info->ValidityPeriod);
    ui.spinBoxDisplayTimes->setValue(info->DisplayTimes);
    ui.checkBoxConfirmationRequired->setChecked(info->ACKRequired);

    ui.textEdit->setText(info->Content);

    ui.comboBoxTargetType->setCurrentIndex(ui.comboBoxTargetType->findData(info->TargetType));
    MS::AnnouncementTarget target = MS::AnnouncementTarget(info->TargetType);
    switch (target) {
    case MS::ANNOUNCEMENT_TARGET_EVERYONE:
        ui.stackedWidget->setCurrentWidget(ui.pageEveryone);
        break;

    case MS::ANNOUNCEMENT_TARGET_COMPUTERS:
        ui.stackedWidget->setCurrentWidget(ui.pageComputers);
        break;

    case MS::ANNOUNCEMENT_TARGET_USERS:
        ui.stackedWidget->setCurrentWidget(ui.pageUsers);
        break;

    default:
        ui.stackedWidget->setCurrentWidget(ui.pageEveryone);
        break;
    }


}

void AnnouncementInfoWidget::getAnnouncementInfo(AnnouncementInfo *info){
    if(!info){return;}

    info->Type = ui.comboBoxAnnouncementType->currentData().toUInt();
    info->ValidityPeriod = ui.spinBoxValidityPeriod->value();
    info->DisplayTimes = ui.spinBoxDisplayTimes->value();
    info->ACKRequired = ui.checkBoxConfirmationRequired->isChecked();

    info->Content = ui.textEdit->toHtml();

    info->TargetType = ui.comboBoxTargetType->currentData().toUInt();

}

void AnnouncementInfoWidget::setReadonly(bool readonly){

    ui.groupBoxID->setEnabled(!readonly);
    ui.groupBoxID->setVisible(readonly);

    ui.comboBoxAnnouncementType->setEnabled(!readonly);
    ui.spinBoxValidityPeriod->setEnabled(!readonly);
    ui.spinBoxDisplayTimes->setEnabled(!readonly);

    ui.textEdit->setReadOnly(readonly);
    ui.comboBoxTargetType->setEnabled(!readonly);

    ui.pushButtonSend->setEnabled(!readonly);
    ui.pushButtonSend->setVisible(!readonly);

}

void AnnouncementInfoWidget::setComputerTargets(const QStringList &targets){

}

void AnnouncementInfoWidget::getMessageInfo(quint32 *messageID, QString *message, bool *confirmationRequired, int *validityPeriod){

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

void AnnouncementInfoWidget::on_pushButtonSend_clicked(){
    
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


    emit signalCloseWidget();
    this->close();

}

void AnnouncementInfoWidget::on_actionRefresh_triggered(){

     m_myself->packetsParser()->sendRequestAnnouncementTargetsPacket(m_myself->socketConnectedToServer(), ui.lineEditAnnouncementID->text());

}

void AnnouncementInfoWidget::on_actionExport_triggered(){
    DataOutputDialog dlg(ui.tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();
}

void AnnouncementInfoWidget::on_actionPrint_triggered(){
#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif
}

void AnnouncementInfoWidget::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *tableView = qobject_cast<QTableView*> (sender());
    if (!tableView){
        return;
    }


    QMenu menu(this);
    menu.addAction(ui.actionRefresh);
    if(m_selectedInfoList.isEmpty()){
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
    menu.addAction(ui.actionAdd);
    menu.addAction(ui.actionDelete);

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void AnnouncementInfoWidget::getSelectedInfo(const QModelIndex &index){

    m_selectedInfoList.clear();

    if(!index.isValid()){
        m_selectedInfoList.clear();
        return;
    }

    QModelIndexList indexList =  ui.tableView->selectionModel()->selectedRows(0);
    if(indexList.isEmpty()){return;}
    foreach (QModelIndex idx, indexList) {
        m_selectedInfoList.append(m_model->getInfoID(idx));
    }

    bool enableExp = m_model->rowCount();
    ui.actionExport->setEnabled(enableExp);
    ui.actionPrint->setEnabled(enableExp);

    bool enableModify = false;
    if(!m_myself->isReadonly()){
        enableModify = true;
    }

    ui.actionAdd->setEnabled(enableModify);
    ui.actionDelete->setEnabled(enableModify);

}


bool AnnouncementInfoWidget::verifyPrivilege(){

    if(!m_myself->isAdminVerified()){
        return false;
    }
    if(m_myself->isReadonly()){
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;
}















} //namespace HEHUI
