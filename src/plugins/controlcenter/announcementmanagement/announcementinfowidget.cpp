


#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>

#include "announcementinfowidget.h"

#include "HHSharedGUI/DataOutputDialog"

namespace HEHUI
{


AnnouncementInfoWidget::AnnouncementInfoWidget(bool readonly, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.comboBoxAnnouncementType->addItem(tr("Normal"), QVariant(quint8(MS::ANNOUNCEMENT_NORMAL)));
    ui.comboBoxAnnouncementType->addItem(tr("Critical"), QVariant(quint8(MS::ANNOUNCEMENT_CRITICAL)));
    ui.comboBoxAnnouncementType->setCurrentIndex(0);


    m_myself = AdminUser::instance();
    ui.lineEditAdmin->setText(m_myself->getUserID());
    bool readonlyUser = m_myself->isReadonly();
    ui.pushButtonClone->setEnabled(!readonlyUser);
    ui.pushButtonEdit->setEnabled(!readonlyUser);
    ui.pushButtonSave->setEnabled(!readonlyUser);

    ui.tabReplies->setEnabled(false);
    ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabReplies));

    m_model = new AnnouncementTargetModel(this);
    ui.tableView->setModel(m_model);
    m_model->setJsonData(QByteArray());

    connect(ui.tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedInfo(const QModelIndex &)));
    //connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewAdminInfo(const QModelIndex &)));
    connect(ui.textBrowserReplies, SIGNAL(anchorClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    QHeaderView *view = ui.tableView->horizontalHeader();
    view->setSectionResizeMode(QHeaderView::Stretch);
    //view->resizeSection(0, 100);
    //view->resizeSection(2, 250);
    //view->resizeSection(3, 200);
    view->setVisible(true);

    m_localTempID = QDateTime::currentDateTime().toTime_t();

    m_readonly = true;
    setReadonly(readonly);

    m_targetsTouched = false;

    m_jobMonitor = JobMonitor::instance();
    connect(m_jobMonitor, SIGNAL(jobFinished(quint32)), this, SLOT(jobFinished(quint32)));
    m_jobID = 0;

}

AnnouncementInfoWidget::~AnnouncementInfoWidget()
{
    m_jobMonitor->deleteJob(m_jobID);

    m_model->clear();
    delete m_model;
}

void AnnouncementInfoWidget::setAnnouncementInfo(const AnnouncementInfo *info)
{
    if(!info) {
        return;
    }

    m_info = *info;

    ui.lineEditAnnouncementID->setText(QString::number(info->ID));
    ui.lineEditAdmin->setText(info->Admin);
    ui.dateTimeEditPublishDate->setDateTime(QDateTime::fromString(info->PublishDate, Qt::ISODate));
    ui.checkBoxActive->setChecked(info->Active);

    ui.comboBoxAnnouncementType->setCurrentIndex(ui.comboBoxAnnouncementType->findData(info->Type));
    ui.spinBoxValidityPeriod->setValue(info->ValidityPeriod);
    ui.spinBoxDisplayTimes->setValue(info->DisplayTimes);
    ui.checkBoxConfirmationRequired->setChecked(info->ACKRequired);

    ui.textEdit->setText(info->Content);

    if(!info->ID) {
        ui.tabReplies->setEnabled(false);
        ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabReplies));
    } else {
        on_actionRefresh_triggered();
        m_localTempID = info->ID;

        if(!info->Replies.isEmpty()) {
            if(ui.tabWidget->indexOf(ui.tabReplies) < 0) {
                ui.textBrowserReplies->setText(info->Replies);
                ui.tabWidget->addTab(ui.tabReplies, tr("Replies"));
            }
        }
    }

}

void AnnouncementInfoWidget::getAnnouncementInfo(AnnouncementInfo *info)
{
    if(!info) {
        return;
    }

    info->ID = ui.lineEditAnnouncementID->text().trimmed().toUInt();
    info->Type = ui.comboBoxAnnouncementType->currentData().toUInt();
    info->Content = ui.textEdit->toHtml();
    info->ACKRequired = ui.checkBoxConfirmationRequired->isChecked();
    info->Admin = ui.lineEditAdmin->text().trimmed();
    //info->PublishDate = ui.dateTimeEditPublishDate->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    info->ValidityPeriod = ui.spinBoxValidityPeriod->value();
    info->TargetType = ui.groupBoxTargets->isChecked() ? quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC) : quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE);
    info->DisplayTimes = ui.spinBoxDisplayTimes->value();
    info->Active = ui.checkBoxActive->isChecked();
    info->Targets = m_model->getNewTargetsStringForSQL();

    info->TempID = m_localTempID;

}

void AnnouncementInfoWidget::setReadonly(bool readonly)
{

    m_readonly = readonly;
    ui.frameID->setVisible(true);
    ui.checkBoxActive->setEnabled(!readonly);
    bool createMode = ui.lineEditAnnouncementID->text().trimmed().isEmpty();

    if(readonly) {
        setWindowTitle(tr("Announcement Info"));
    } else if(createMode) {
        setWindowTitle(tr("Create New Announcement"));
        ui.frameID->setVisible(false);
    } else {
        setWindowTitle(tr("Modify Announcement Info"));
    }

    ui.comboBoxAnnouncementType->setEnabled(!readonly);
    ui.spinBoxValidityPeriod->setEnabled(!readonly);
    ui.spinBoxDisplayTimes->setEnabled(!readonly);
    ui.checkBoxConfirmationRequired->setEnabled(!readonly);

    ui.textEdit->setReadOnly(readonly);
    ui.groupBoxTargets->setCheckable(!readonly);

    ui.pushButtonClone->setVisible(readonly);
    ui.pushButtonEdit->setVisible(readonly);
    ui.pushButtonSave->setVisible(!readonly);
    ui.pushButtonSave->setEnabled(!readonly);

    QHeaderView *view = ui.tableView->horizontalHeader();
    if(readonly) {
        view->showSection(2);
        view->showSection(3);
    } else {
        view->hideSection(2);
        view->hideSection(3);
    }

}

bool AnnouncementInfoWidget::isReadonly()
{
    return m_readonly;
}

void AnnouncementInfoWidget::addComputerTargets(const QStringList &targets)
{
    if(m_readonly) {
        return;
    }
    m_model->addComputerTargets(targets);
}

void AnnouncementInfoWidget::setAnnouncementTargetsData(const QString &announcementID, const QByteArray &data)
{
    qDebug() << "--AnnouncementInfoWidget::setAnnouncementTargetsData(...) announcementID:" << announcementID;

    if(announcementID != ui.lineEditAnnouncementID->text().trimmed()) {
        return;
    }

    m_model->setJsonData(data);

    m_targetsTouched = true;
}

void AnnouncementInfoWidget::on_pushButtonClone_clicked()
{

    m_info.ID = 0;
    m_info.Admin = m_myself->getUserID();
    m_info.Active = true;

    ui.lineEditAnnouncementID->setText("");
    ui.lineEditAdmin->setText(m_myself->getUserID());
    ui.checkBoxActive->setChecked(true);

    ui.pushButtonSave->setEnabled(true);

    setReadonly(false);

    m_model->switchToCloneMode();
}

void AnnouncementInfoWidget::on_pushButtonEdit_clicked()
{
    //setReadonly(false);

    ui.pushButtonClone->setEnabled(false);
    ui.pushButtonClone->setVisible(false);
    ui.pushButtonEdit->setEnabled(false);
    ui.pushButtonEdit->setVisible(false);
    ui.pushButtonSave->setEnabled(true);
    ui.pushButtonSave->setVisible(true);

    ui.checkBoxActive->setEnabled(true);

    ui.groupBoxTargets->setCheckable(true);
    ui.groupBoxTargets->setChecked(m_info.Type == quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC));

}

void AnnouncementInfoWidget::on_pushButtonSave_clicked()
{

    QString announcementString = ui.textEdit->toPlainText();
    if(announcementString.trimmed().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid Content!"));
        return;
    }

    if(ui.groupBoxTargets->isChecked() && (!m_model->rowCount())) {
        int ret = QMessageBox::question(this, tr("Question"),
                                        tr("You chose to send the announcement to specific targets, but no target has been appointed!<br>Do you want to specify the targets?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes);
        if(ret == QMessageBox::Yes) {
            ui.tabWidget->setCurrentWidget(ui.tabTargets);
            return;
        }
    }

    int ret = QMessageBox::question(this, tr("Confirm"),
                                    tr("Do you really want to save the content?"),
                                    QMessageBox::Yes | QMessageBox::Cancel,
                                    QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel) {
        return;
    }

    ui.pushButtonSave->setEnabled(false);

    unsigned int announcementID = m_info.ID;
    if(!announcementID) {
        m_info.Active = ui.checkBoxActive->isChecked();
        m_info.Type = ui.comboBoxAnnouncementType->currentData().toUInt();
        m_info.ValidityPeriod = ui.spinBoxValidityPeriod->value();
        m_info.DisplayTimes = ui.spinBoxDisplayTimes->value();
        m_info.ACKRequired = ui.checkBoxConfirmationRequired->isChecked();

        m_info.Content = ui.textEdit->toPlainText();
    }

    bool ok = false;
    if(!announcementID) {
        ok = m_myself->packetsParser()->sendCreateAnnouncementPacket(
                 m_myself->socketConnectedToServer(),
                 newJob(CreateAnnouncement, tr("Creating Announcement")),
                 m_localTempID,
                 m_myself->getUserID(),
                 ui.comboBoxAnnouncementType->currentData().toUInt(),
                 announcementString,
                 ui.checkBoxConfirmationRequired->isChecked(),
                 ui.spinBoxValidityPeriod->value(),
                 ui.groupBoxTargets->isChecked() ? quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC) : quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE),
                 m_model->getNewTargetsStringForSQL()
             );
    } else {
        ok = m_myself->packetsParser()->sendUpdateAnnouncementPacket(
                 m_myself->socketConnectedToServer(),
                 newJob(ModifyAnnouncement, tr("Modifying Announcement")),
                 m_myself->getUserID(),
                 announcementID,
                 ui.groupBoxTargets->isChecked() ? quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC) : quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE),
                 ui.checkBoxActive->isChecked(),
                 m_model->getNewTargetsStringForSQL(),
                 m_model->getDeletedTargetsStringForSQL()
             );
    }

    if(!ok) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to send data!"));
        return;
    }

    m_jobMonitor->startJobMonitoring(m_jobID, 5000);

    setEnabled(false);
    ui.pushButtonSave->setEnabled(false);

}

void AnnouncementInfoWidget::on_actionRefresh_triggered()
{
    m_myself->packetsParser()->sendRequestAnnouncementTargetsPacket(m_myself->socketConnectedToServer(), ui.lineEditAnnouncementID->text());
}

void AnnouncementInfoWidget::on_actionExport_triggered()
{
    DataOutputDialog dlg(ui.tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();
}

void AnnouncementInfoWidget::on_actionPrint_triggered()
{
#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui.tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif
}

void AnnouncementInfoWidget::on_actionAddComputer_triggered()
{
    QString tip = tr("Format:<br>1. Computer<br>Use ',' to seprate multi items.");

    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Add Computer"),
                                         tr("%1<br>Please input computer name:").arg(tip),
                                         QLineEdit::Normal,
                                         "",
                                         &ok
                                        ).trimmed();
    if (ok && !text.isEmpty()) {
        m_model->addComputerTargets(text.split(","));
    }

}

void AnnouncementInfoWidget::on_actionAddUser_triggered()
{
    QString tip = tr("Format:<br> 1. Computer\\User<br> 2. User<br>Use ',' to seprate multi items.");
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("Add User"),
                                         tr("%1<br>Please input user name:").arg(tip),
                                         QLineEdit::Normal,
                                         "",
                                         &ok
                                        ).trimmed();
    if (ok && !text.isEmpty()) {
        m_model->addUserTargets(text.split(","));
    }
}

void AnnouncementInfoWidget::on_actionDeleteTarget_triggered()
{

    QModelIndexList indexList =  ui.tableView->selectionModel()->selectedRows(0);
    if(indexList.isEmpty()) {
        return;
    }
    foreach (QModelIndex idx, indexList) {
        m_model->deleteTarget(idx);
    }

}

void AnnouncementInfoWidget::slotShowCustomContextMenu(const QPoint &pos)
{

    QTableView *tableView = qobject_cast<QTableView *> (sender());
    if (!tableView) {
        return;
    }


    QMenu menu(this);


    if(m_readonly) {
        menu.addAction(ui.actionRefresh);
        if(!m_model->rowCount()) {
            menu.exec(tableView->viewport()->mapToGlobal(pos));
            return;
        }

        menu.addSeparator();
        menu.addAction(ui.actionExport);

#ifndef QT_NO_PRINTER
        ui.actionPrint->setShortcut(QKeySequence::Print);
        menu.addAction(ui.actionPrint);
#endif

    } else if(m_targetsTouched || ui.lineEditAnnouncementID->text().trimmed().isEmpty()) {
        menu.addSeparator();
        menu.addAction(ui.actionAddComputer);
        menu.addAction(ui.actionAddUser);
        menu.addAction(ui.actionDelete);
    } else {
        menu.addAction(ui.actionRefresh);
    }


    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void AnnouncementInfoWidget::getSelectedInfo(const QModelIndex &index)
{


    if(!index.isValid()) {
        return;
    }

    //    QModelIndexList indexList =  ui.tableView->selectionModel()->selectedRows(0);
    //    if(indexList.isEmpty()){return;}
    //    foreach (QModelIndex idx, indexList) {
    //    }

    bool enableExp = m_model->rowCount();
    ui.actionExport->setEnabled(enableExp);
    ui.actionPrint->setEnabled(enableExp);

    bool enableModify = false;
    if(!m_myself->isReadonly()) {
        enableModify = true;
    }

    ui.actionAddComputer->setEnabled(enableModify);
    ui.actionAddUser->setEnabled(enableModify);
    ui.actionDelete->setEnabled(enableModify);

}

void AnnouncementInfoWidget::linkClicked(const QUrl &url)
{
    QString scheme = url.scheme();

    if(scheme == URLScheme_Reply) {
        QString sender = url.userInfo();
        QString sendersAssetNO = url.host();

        bool ok;
        QString text = QInputDialog::getMultiLineText(this, tr("Reply to %1").arg(sender),
                       tr("Message:"), "", &ok);
        if (!ok || text.isEmpty()) {
            return;
        }

        ok = m_myself->packetsParser()->sendAdminReplyMessagePacket(m_myself->socketConnectedToServer(), m_info.ID, m_myself->getUserID(), sender, sendersAssetNO, text);
        if(!ok) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to send data!"));
            return;
        }

        QString title = QString("<span>%1 %2</span>").arg(m_myself->getUserID()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        m_info.Replies += title;

    }

    qDebug() << "URL scheme:" << scheme << " host:" << url.host() << " userInfo:" << url.userInfo();

}

void AnnouncementInfoWidget::jobFinished(quint32 jobID)
{
    Job *job = m_jobMonitor->getJob(jobID);
    if(!job) {
        return;
    }
    if(jobID != m_jobID) {
        return;
    }

    QMessageBox msgbox(this);
    bool createAnnouncement = (job->Type == quint32(CreateAnnouncement));
    QString message = "";
    Job::JobResult result = Job::JobResult(job->Result);
    switch (result) {
    case Job::FinishedWithError: {
        if(createAnnouncement) {
            unsigned int id = job->ExtraData.toUInt();
            if(id) {
                m_info.ID = id;
                m_info.Admin = m_myself->getUserID();
                m_info.PublishDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

                AnnouncementInfo info = m_info;
                setAnnouncementInfo(&info);
            }
        }
        setReadonly(true);
        emit signalAnnouncementUpdated();

        message = tr("Error occured when %1 the announcement!<br>%2").arg(createAnnouncement ? tr("creating") : tr("modifying"));
        msgbox.setIcon(QMessageBox::Warning);

    }
    break;

    case Job::Failed: {
        message = tr("Failed to %1 the announcement!<br>%2").arg(createAnnouncement ? tr("creat") : tr("modify"));
        msgbox.setIcon(QMessageBox::Critical);
    }
    break;

    case Job::Timeout: {
        message = tr("Timed out when %1 the announcement!<br>%2").arg(createAnnouncement ? tr("creating") : tr("modifying"));
        msgbox.setIcon(QMessageBox::Critical);
    }
    break;

    case Job::Finished: {
        if(createAnnouncement) {
            unsigned int id = job->ExtraData.toUInt();
            if(id) {
                m_info.ID = id;
                m_info.Admin = m_myself->getUserID();
                m_info.PublishDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

                AnnouncementInfo info = m_info;
                setAnnouncementInfo(&info);
            }
        }
        setReadonly(true);
        emit signalAnnouncementUpdated();

    }
    break;


    default:
        break;
    }

    m_jobID = 0;
    m_jobMonitor->deleteJob(jobID);

    if(!message.isEmpty()) {
        msgbox.setText(message);
        msgbox.exec();
    }

    setEnabled(true);

}

quint32 AnnouncementInfoWidget::newJob(AnnouncementJobType jobType, const QString &jobTitle)
{
    m_jobMonitor->deleteJob(m_jobID);
    return (m_jobID = m_jobMonitor->newJob(quint32(jobType), jobTitle));
}

bool AnnouncementInfoWidget::verifyPrivilege()
{

    if(!m_myself->isAdminVerified()) {
        return false;
    }
    if(m_myself->isReadonly()) {
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;
}















} //namespace HEHUI
