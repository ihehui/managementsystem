#include "announcementmanagementwidget.h"
#include "ui_announcementmanagementwidget.h"

#include <QTimer>
#include <QMenu>

#include "announcementinfowidget.h"

#include "../../sharedms/global_shared.h"

#include "HHSharedGUI/hdataoutputdialog.h"


namespace HEHUI {


AnnouncementManagementWidget::AnnouncementManagementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnouncementManagementWidget)
{
    ui->setupUi(this);


    ui->comboBoxValidity->addItem(tr("All"), QVariant(-1));
    ui->comboBoxValidity->addItem(tr("Already Expired"), QVariant(0));
    ui->comboBoxValidity->addItem(tr("Not Expired"), QVariant(1));
    ui->comboBoxValidity->setCurrentIndex(0);

    ui->comboBoxTarget->addItem(tr(""), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_ALL)));
    ui->comboBoxTarget->addItem(tr("Everyone"), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE)));
    ui->comboBoxTarget->addItem(tr("Specific Targets"), QVariant(quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC)));
    ui->comboBoxTarget->setCurrentIndex(0);

    ui->comboBoxPeriod->addItem(tr("Today"), QVariant(quint8(Period_Today)));
    ui->comboBoxPeriod->addItem(tr("Last 24 Hours"), QVariant(quint8(Period_Last_24_Hours)));
    ui->comboBoxPeriod->addItem(tr("Yesterday"), QVariant(quint8(Period_Yesterday)));
    ui->comboBoxPeriod->addItem(tr("This Week"), QVariant(quint8(Period_This_Week)));
    ui->comboBoxPeriod->addItem(tr("This Month"), QVariant(quint8(Period_This_Month)));
    ui->comboBoxPeriod->addItem(tr("Custom Selection"), QVariant(quint8(Period_Custom)));
    ui->comboBoxPeriod->setCurrentIndex(3);

    m_model = new AnnouncementInfoModel(this);
    m_proxyModel = new SortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    ui->tableView->setModel(m_proxyModel);
    m_model->setJsonData(QByteArray());

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedInfo(const QModelIndex &)));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showAnnouncementInfo(const QModelIndex &)));

    QHeaderView *view = ui->tableView->horizontalHeader();
    //view->resizeSection(0, 100);
    //view->resizeSection(2, 250);
    //view->resizeSection(3, 200);
    view->setVisible(true);

    m_infoWidget = 0;

    m_myself = AdminUser::instance();

}

AnnouncementManagementWidget::~AnnouncementManagementWidget()
{
    delete ui;

    m_selectedInfoList.clear();

    if(m_infoWidget){
        delete m_infoWidget;
    }

    delete m_proxyModel;
    delete m_model;

}

void AnnouncementManagementWidget::setAnnouncementsData(const QByteArray &data){
    m_selectedInfoList.clear();
    m_model->setJsonData(data);
}

void AnnouncementManagementWidget::setAnnouncementTargetsData(const QString &announcementID, const QByteArray &data){
    if(!m_infoWidget){return;}
    m_infoWidget->setAnnouncementTargetsData(announcementID, data);
}

void AnnouncementManagementWidget::setAnnouncementReplies(const QByteArray &data){
    m_model->setAnnouncementRepliesData(data);
}

void AnnouncementManagementWidget::on_toolButtonQuery_clicked(){
    on_actionRefresh_triggered();
}

void AnnouncementManagementWidget::on_toolButtonFilter_clicked(){
    filter();
}

void AnnouncementManagementWidget::filter(){

    QRegExp assetNORegExp;
    QRegExp typeRegExp;
    QRegExp acknowledgedRegExp;

    QString filterString = ui->lineEditAssetNO->text();
    if(!filterString.trimmed().isEmpty()){
        assetNORegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = validityString();
    if(filterString != "-1"){
        typeRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = targetString();
    if(filterString != "-1"){
        acknowledgedRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    m_proxyModel->setFilters(assetNORegExp, typeRegExp, acknowledgedRegExp );

}

void AnnouncementManagementWidget::on_actionRefresh_triggered(){

    if(!verifyPrivilege()){
        return;
    }

    QString assetNOs = ui->lineEditAssetNO->text().trimmed();
    QString userNames = ui->lineEditUserName->text().trimmed();
    MS::AnnouncementTarget tp = MS::AnnouncementTarget(ui->comboBoxTarget->currentData().toUInt());
    if(tp == MS::ANNOUNCEMENT_TARGET_EVERYONE){
        assetNOs = "";
        userNames = "";
    }

    QString startTime = "1970-01-01", endTime = "2099-01-01";
    periodString(&startTime, &endTime);

    m_myself->packetsParser()->sendRequestAnnouncementsPacket(m_myself->socketConnectedToServer(),
                                                              ui->lineEditID->text().trimmed(),
                                                              ui->lineEditKeyword->text(),
                                                              validityString(),
                                                              assetNOs,
                                                              userNames,
                                                              targetString(),
                                                              startTime,
                                                              endTime
                                                              );

}

void AnnouncementManagementWidget::on_comboBoxPeriod_currentIndexChanged(int index){
    periodString(0, 0);
    Period period = Period(ui->comboBoxPeriod->currentData().toUInt());
    ui->dateTimeEditStartTime->setReadOnly((period != Period_Custom));
    ui->dateTimeEditEndTime->setReadOnly((period != Period_Custom));
}

void AnnouncementManagementWidget::on_actionExport_triggered(){
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();
}

void AnnouncementManagementWidget::on_actionPrint_triggered(){
#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif
}

void AnnouncementManagementWidget::on_actionCreate_triggered(){
    AnnouncementInfo info;
    showAnnouncementInfoWidget(&info, false);
}

void AnnouncementManagementWidget::on_actionClone_triggered(){

    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows(0);
    if(indexList.isEmpty()){return;}
    QModelIndex index = indexList.first();
    if(!index.isValid()){
        return;
    }

    //AnnouncementInfo *info = m_model->getInfo(index);

    AnnouncementInfo info(*(m_model->getInfo(index)));
    info.ID = "";
    info.Admin = m_myself->getUserID();
    info.Active = true;

    showAnnouncementInfoWidget(&info, false);
}

void AnnouncementManagementWidget::on_actionDisable_triggered(){

}

void AnnouncementManagementWidget::requestAcknowledgeAlarms(bool deleteAlarms){

    if(!verifyPrivilege()){
        return;
    }

    m_myself->packetsParser()->sendAcknowledgeSysAlarmsPacket(m_myself->socketConnectedToServer(), m_myself->getUserID(), m_selectedInfoList, deleteAlarms);

    QTimer::singleShot(3000, this, SLOT(on_actionRefresh_triggered()));

}

void AnnouncementManagementWidget::showAnnouncementInfo(const QModelIndex &index){

    if(!index.isValid()){
        return;
    }

    AnnouncementInfo *info = m_model->getInfo(index);
    showAnnouncementInfoWidget(info, true);
}

void AnnouncementManagementWidget::showAnnouncementInfoWidget(AnnouncementInfo *info, bool readonly){

    if(readonly && (!verifyPrivilege()) ){
        return;
    }


    if(!m_infoWidget){
        m_infoWidget = new AnnouncementInfoWidget(readonly);
        connect(m_infoWidget, SIGNAL(signalAnnouncementUpdated()), this, SLOT(on_toolButtonQuery_clicked()));
        //connect(&m_infoWidget, SIGNAL(signalOK()), &dlg, SLOT(accept()));
        //connect(&m_infoWidget, SIGNAL(signalCancel()), &dlg, SLOT(reject()));
    }

    m_infoWidget->setAnnouncementInfo(info);
    m_infoWidget->setReadonly(readonly);

    m_infoWidget->showNormal();

}

void AnnouncementManagementWidget::slotShowCustomContextMenu(const QPoint & pos){

    QTableView *tableView = qobject_cast<QTableView*> (sender());
    if (!tableView){
        return;
    }

    getSelectedInfo(ui->tableView->selectionModel()->currentIndex());

    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
    if(m_selectedInfoList.isEmpty()){
        menu.exec(tableView->viewport()->mapToGlobal(pos));
        return;
    }

    menu.addSeparator();
    menu.addAction(ui->actionExport);

#ifndef QT_NO_PRINTER

    ui->actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui->actionPrint);

#endif

    menu.addSeparator();
    menu.addAction(ui->actionCreate);
    menu.addAction(ui->actionClone);
    menu.addAction(ui->actionDisable);

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void AnnouncementManagementWidget::getSelectedInfo(const QModelIndex &index){

    m_selectedInfoList.clear();

    if(!index.isValid()){
        m_selectedInfoList.clear();
        return;
    }

    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows(0);
    if(indexList.isEmpty()){return;}
    foreach (QModelIndex idx, indexList) {
        m_selectedInfoList.append(m_model->getInfoID(idx));
    }

    bool enableExp = m_model->rowCount();
    ui->actionExport->setEnabled(enableExp);
    ui->actionPrint->setEnabled(enableExp);

    bool enableModify = false;
    if(!m_myself->isReadonly()){
        enableModify = true;
    }

    ui->actionCreate->setEnabled(enableModify);
    ui->actionClone->setEnabled( enableModify && (!m_selectedInfoList.isEmpty()) );
    ui->actionDisable->setEnabled( enableModify && (!m_selectedInfoList.isEmpty()) );

}


bool AnnouncementManagementWidget::verifyPrivilege(){

    if(!m_myself->isAdminVerified()){
        return false;
    }
    if(m_myself->isReadonly()){
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;
}

QString AnnouncementManagementWidget::validityString() const{
    return QString::number(ui->comboBoxValidity->currentData().toInt());
}

QString AnnouncementManagementWidget::targetString() const{

    MS::AnnouncementTarget tp = MS::AnnouncementTarget(ui->comboBoxTarget->currentData().toUInt());
    if(tp == MS::ANNOUNCEMENT_TARGET_ALL){
        return "-1";
    }
    return QString::number(quint8(tp));
}

void AnnouncementManagementWidget::periodString(QString *startTime, QString *endTime){

    QDateTime curTime = QDateTime::currentDateTime();
    QTime zeroTime = QTime(0, 0);

    Period period = Period(ui->comboBoxPeriod->currentData().toUInt());
    switch (period) {
    case Period_Custom:
    {

    }
        break;

    case Period_Today:
    {
        ui->dateTimeEditStartTime->setDate(curTime.date());
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
        break;

    case Period_Last_24_Hours:
    {
        QDateTime dateTime = curTime.addSecs(-(24*3600));
        ui->dateTimeEditStartTime->setDateTime(dateTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
        break;

    case Period_Yesterday:
    {
        QDate date = curTime.date().addDays(-1);
        ui->dateTimeEditStartTime->setDate(date);
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDate(date);
        ui->dateTimeEditEndTime->setTime(QTime(23, 59));
    }
        break;

    case Period_This_Week:
    {
        QDate date = curTime.date();
        int dayOfWeek = date.dayOfWeek();
        date = date.addDays(-1*(dayOfWeek-1));
        ui->dateTimeEditStartTime->setDate(date);
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
        break;

    case Period_This_Month:
    {
        QDate date = curTime.date();
        int day = date.day();
        date = date.addDays(-1*(day-1));
        ui->dateTimeEditStartTime->setDate(date);
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
        break;

    default:
        ui->dateTimeEditStartTime->setDate(curTime.date());
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
        break;
    }

    if(startTime){
        *startTime = ui->dateTimeEditStartTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if(endTime){
        *endTime = ui->dateTimeEditEndTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

}



} //namespace HEHUI
