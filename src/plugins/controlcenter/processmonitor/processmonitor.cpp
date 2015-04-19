#include "processmonitor.h"
#include "ui_processmonitor.h"


#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "HHSharedGUI/hdataoutputdialog.h"

#include "ruleinfowidget.h"
#include "../adminuser.h"

namespace HEHUI {


ProcessMonitor::ProcessMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcessMonitor)
{
    ui->setupUi(this);

    m_selectedRule = 0;

    m_processMonitorRuleModel = new ProcessMonitorRuleModel(this);
    m_proxyModel = new ProcessMonitorRuleSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_processMonitorRuleModel);
    ui->tableView->setModel(m_proxyModel);
    m_processMonitorRuleModel->setJsonData(QJsonArray());



    m_updatedDate = "";
    m_updatedBy = "";

    m_rulesModified = false;




    this->installEventFilter(this);
    ui->tableView->installEventFilter(this);

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedRule(const QModelIndex &)));
    //connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex &,QModelIndex &)), this, SLOT(slotShowUserInfo(const QModelIndex &)));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewRuleInfo(const QModelIndex &)));

    QHeaderView *view = ui->tableView->horizontalHeader();
    view->resizeSection(0, 250);
    view->setVisible(true);

    getSelectedRule(QModelIndex());

}

ProcessMonitor::~ProcessMonitor()
{
    delete ui;
}

bool ProcessMonitor::eventFilter(QObject *obj, QEvent *event) {

    switch(event->type()){
    case QEvent::KeyRelease:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);

        if(keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down){
            getSelectedRule(ui->tableView->currentIndex());
        }

        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_O){
            slotExportQueryResult();
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_P){
            slotPrintQueryResult();
        }

        return true;
    }
        break;
    case QEvent::MouseButtonPress:
    case QEvent::Leave:
    {
        //return QObject::eventFilter(obj, event);
    }
        break;
        //    case QEvent::ToolTip:
        //    {
        //        if(obj == ui.userPSWDLineEdit){
        //            QString pwd = ui.userPSWDLineEdit->text();
        //            if(pwd.isEmpty()){pwd = tr("Password");}
        //            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        //            QString tip = QString("<b><h1>%1</h1></b>").arg(pwd);
        //            QToolTip::showText(helpEvent->globalPos(), tip);
        //            return true;
        //        }

        //    }
        //        break;
    default:
        break;
        //return QObject::eventFilter(obj, event);


    }

    return QObject::eventFilter(obj, event);

}


void ProcessMonitor::setJsonData(const QByteArray &data){


    if(data.isEmpty()){
        qCritical()<<"ERROR! Empty rules data.";
        return;
    }


    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){return;}

    bool locallyEnableProcMon = object["LocallyEnableProcMon"].toString().toUInt();
    ui->checkBoxLocallyEnableProcessMon->setChecked(locallyEnableProcMon);

    bool locallyEnablePassthrough = object["LocallyEnablePassthrough"].toString().toUInt();
    ui->checkBoxLocallyEnablePassthrough->setChecked(locallyEnablePassthrough);

    bool locallyEnableLogAllowedProcess = object["LocallyEnableLogAllowedProcess"].toString().toUInt();
    ui->checkBoxLocallyEnableLogAllowedProcess->setChecked(locallyEnableLogAllowedProcess);

    bool locallyEnableLogBlockedProcess = object["LocallyEnableLogBlockedProcess"].toString().toUInt();
    ui->checkBoxLocallyEnableLogBlockedProcess->setChecked(locallyEnableLogBlockedProcess);

    m_updatedDate = object["UpdatedDate"].toString();
    m_updatedBy = object["UpdatedBy"].toString();


    QJsonArray array = object["Rules"].toArray();

    m_selectedRule = 0;
    m_processMonitorRuleModel->setJsonData(array);


    ui->pushButtonApply->setEnabled(true);


}

void ProcessMonitor::rulesSaved(){
    m_rulesModified = false;
}

void ProcessMonitor::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *wgt = qobject_cast<QTableView*> (sender());
    if (!wgt){
        return;
    }

    //getSelectedRule(wgt->currentIndex());


    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
//    if(!m_selectedRule){
//        menu.exec(wgt->viewport()->mapToGlobal(pos));
//        return;
//    }


    menu.addSeparator();
    menu.addAction(ui->actionExport);

#ifndef QT_NO_PRINTER

    ui->actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui->actionPrint);

#endif

    if(AdminUser::instance()->isReadonly()){
        menu.exec(wgt->viewport()->mapToGlobal(pos));
        return;
    }

    menu.addSeparator();
    menu.addAction(ui->actionAddFileHash);
    menu.addAction(ui->actionAddFilePath);
    menu.addAction(ui->actionDelete);
    menu.addAction(ui->actionModify);


    menu.exec(wgt->viewport()->mapToGlobal(pos));

}

void ProcessMonitor::getSelectedRule(const QModelIndex &index){

    if(!index.isValid()){
        m_selectedRule = 0;
    }else{
        m_selectedRule = m_processMonitorRuleModel->getRule(index);
    }


    bool enableExp = m_processMonitorRuleModel->rowCount();
    ui->actionExport->setEnabled(enableExp);
    ui->actionPrint->setEnabled(enableExp);

    bool enableModify = false;
    if(m_selectedRule){
        if(!m_selectedRule->globalRule){
            enableModify = true;
        }
    }

    bool enableAdd = ui->pushButtonApply->isEnabled();
    ui->actionAddFileHash->setEnabled(enableAdd);
    ui->actionAddFilePath->setEnabled(enableAdd);

    ui->actionDelete->setEnabled(enableModify);
    ui->actionModify->setEnabled(enableModify);


}

void ProcessMonitor::slotViewRuleInfo(const QModelIndex &index){
    getSelectedRule(index);

    if(!m_selectedRule){
        return;
    }

    QString ruleString = m_selectedRule->ruleString;
    QString comment = m_selectedRule->comment;
    bool blacklist = m_selectedRule->blacklistRule;
    showRuleInfo(m_selectedRule->hashRule, true, &ruleString, &comment, &blacklist);
}

bool ProcessMonitor::verifyPrivilege(){

    AdminUser *adminUser = AdminUser::instance();
    if(!adminUser->isAdminVerified()){
        return false;
    }
    if(adminUser->isReadonly()){
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;

}

void ProcessMonitor::on_actionRefresh_triggered(){
    emit signalGetProcessMonitorInfo(MS::SYSINFO_PROCESSMONITOR);
}

void ProcessMonitor::on_actionAddFileHash_triggered(){
    QString ruleString, comment;
    bool blacklistRule = true;
    if(!showRuleInfo(true, false, &ruleString, &comment, &blacklistRule)){
        return;
    }

    m_processMonitorRuleModel->addLocalRule(ruleString, comment, true, blacklistRule);

    m_rulesModified = true;

}

void ProcessMonitor::on_actionAddFilePath_triggered(){
    QString ruleString, comment;
    bool blacklistRule = true;
    if(!showRuleInfo(false, false, &ruleString, &comment, &blacklistRule)){
        return;
    }

    m_processMonitorRuleModel->addLocalRule(ruleString, comment, true, blacklistRule);

    m_rulesModified = true;
}

void ProcessMonitor::on_actionDelete_triggered(){
    m_processMonitorRuleModel->deleteRule(m_selectedRule->ruleString);
    m_rulesModified = true;
}

void ProcessMonitor::on_actionModify_triggered(){

    if(!m_selectedRule){
        return;
    }

    QString ruleString = m_selectedRule->ruleString;
    QString comment = m_selectedRule->comment;
    bool hashRule = m_selectedRule->hashRule;
    bool blacklistRule = m_selectedRule->blacklistRule;
    if(!showRuleInfo(hashRule, false, &ruleString, &comment, &blacklistRule)){
        return;
    }


    m_processMonitorRuleModel->deleteRule(m_selectedRule->ruleString);
    m_processMonitorRuleModel->addLocalRule(ruleString, comment, hashRule, blacklistRule);

    m_rulesModified = true;

}

void ProcessMonitor::on_actionExport_triggered(){
    slotExportQueryResult();
}

void ProcessMonitor::on_actionPrint_triggered(){
    slotPrintQueryResult();
}

void ProcessMonitor::on_pushButtonApply_clicked(){

    if(!verifyPrivilege()){return;}

    bool useGlobalRules = ui->checkBoxUseGlobaRules->isChecked();;
    bool enableProcMon = ui->checkBoxLocallyEnableProcessMon->isChecked();
    bool enablePassthrough = ui->checkBoxLocallyEnablePassthrough->isChecked();
    bool enableLogAllowedProcess = ui->checkBoxLocallyEnableLogAllowedProcess->isChecked();
    bool enableLogBlockedProcess = ui->checkBoxLocallyEnableLogBlockedProcess->isChecked();

    QJsonObject object;
    if(m_rulesModified){
        QJsonArray rulesArray = m_processMonitorRuleModel->getLocalRulesJsonData();
        object["Rules"] = rulesArray;
    }

    QJsonDocument doc(object);
    emit signalSetProcessMonitorInfo(doc.toJson(QJsonDocument::Compact), enableProcMon,enablePassthrough, enableLogAllowedProcess, enableLogBlockedProcess, useGlobalRules, "");

}

void ProcessMonitor::slotExportQueryResult(){

    DataOutputDialog dlg(ui->tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void ProcessMonitor::slotPrintQueryResult(){

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

bool ProcessMonitor::showRuleInfo(bool hashMode, bool readonly, QString *ruleString, QString *ruleComment, bool *blacklistRule){

    if(!readonly && (!verifyPrivilege())){return false;}

    QDialog dlg(this);
    QVBoxLayout vbl(&dlg);
    vbl.setContentsMargins(1, 1, 1, 1);

    RuleInfoWidget wgt(hashMode, readonly, &dlg);
    connect(&wgt, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(&wgt, SIGNAL(rejected()), &dlg, SLOT(reject()));
    QString str, comment;
    bool blacklist;
    if(ruleString){
        str = *ruleString;
    }
    if(ruleComment){
        comment = *ruleComment;
    }
    if(blacklistRule){
        blacklist = *blacklistRule;
    }
    wgt.setRuleInfo(str, comment, blacklist);

    vbl.addWidget(&wgt);
    dlg.setLayout(&vbl);
    dlg.updateGeometry();
    if(!readonly){
        dlg.setWindowTitle(tr("Edit Rule"));
    }else{
        dlg.setWindowTitle(tr("Rule Info"));
    }
    if(dlg.exec() != QDialog::Accepted){
        return false;
    }

    if(!readonly){
        if(ruleString){
            *ruleString = wgt.rule();
        }
        if(ruleComment){
            *ruleComment = wgt.comment();
        }
        if(blacklistRule){
            *blacklistRule = wgt.blacklistRule();
        }
    }

    return true;

}









} //namespace HEHUI
