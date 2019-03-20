#include "alarmsmanagementwidget.h"
#include "ui_alarmsmanagementwidget.h"

#include <QMenu>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

#include "HHSharedGUI/DataOutputDialog"


namespace HEHUI
{


AlarmsManagementWidget::AlarmsManagementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmsManagementWidget)
{
    ui->setupUi(this);


    ui->comboBoxType->addItem(tr("All"), QVariant(-1));
    ui->comboBoxType->addItem(tr("Hardware Changed"), QVariant(quint8(MS::ALARM_HARDWARECHANGE)));
    ui->comboBoxType->addItem(tr("Process Monitor"), QVariant(quint8(MS::ALARM_PROCESSMONITOR)));
    ui->comboBoxType->addItem(tr("Unknown"), QVariant(quint8(MS::ALARM_UNKNOWN)));
    ui->comboBoxType->setCurrentIndex(0);

    ui->comboBoxAcknowledged->addItem(tr("All"), QVariant(-1));
    ui->comboBoxAcknowledged->addItem(tr("Acknowledged"), QVariant(1));
    ui->comboBoxAcknowledged->addItem(tr("Unacknowledged"), QVariant(0));
    ui->comboBoxAcknowledged->setCurrentIndex(2);

    ui->comboBoxPeriod->addItem(tr("Today"), QVariant(quint8(Period_Today)));
    ui->comboBoxPeriod->addItem(tr("Last 24 Hours"), QVariant(quint8(Period_Last_24_Hours)));
    ui->comboBoxPeriod->addItem(tr("Yesterday"), QVariant(quint8(Period_Yesterday)));
    ui->comboBoxPeriod->addItem(tr("This Week"), QVariant(quint8(Period_This_Week)));
    ui->comboBoxPeriod->addItem(tr("This Month"), QVariant(quint8(Period_This_Month)));
    ui->comboBoxPeriod->addItem(tr("Custom Selection"), QVariant(quint8(Period_Custom)));
    ui->comboBoxPeriod->setCurrentIndex(0);

    m_model = new AlarmInfoModel(this);
    m_proxyModel = new AlarmInfoSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    ui->tableView->setModel(m_proxyModel);
    m_model->setJsonData(QByteArray());

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getSelectedInfo(const QModelIndex &)));
    //connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotViewAdminInfo(const QModelIndex &)));

    QHeaderView *view = ui->tableView->horizontalHeader();
    view->resizeSection(0, 100);
    view->resizeSection(2, 250);
    //view->resizeSection(3, 200);
    view->setVisible(true);

    m_myself = AdminUser::instance();


}

AlarmsManagementWidget::~AlarmsManagementWidget()
{
    qDebug() << "--AlarmsManagementWidget::~AlarmsManagementWidget()";

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    ui->tableView->setModel(0);
    delete selectionModel;

    delete m_proxyModel;
    delete m_model;
    m_selectedInfoList.clear();

    delete ui;
}

void AlarmsManagementWidget::setData(const QByteArray &data)
{
    m_selectedInfoList.clear();
    m_model->setJsonData(data);
}

void AlarmsManagementWidget::on_toolButtonQuery_clicked()
{
    on_actionRefresh_triggered();
}

void AlarmsManagementWidget::on_toolButtonFilter_clicked()
{
    filter();
}

void AlarmsManagementWidget::filter()
{

    QRegExp assetNORegExp;
    QRegExp typeRegExp;
    QRegExp acknowledgedRegExp;

    QString filterString = ui->lineEditAssetNO->text();
    if(!filterString.trimmed().isEmpty()) {
        assetNORegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = typeString();
    if(filterString != "-1") {
        typeRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    filterString = acknowledgedString();
    if(filterString != "-1") {
        acknowledgedRegExp = QRegExp(filterString, Qt::CaseInsensitive);
    }

    m_proxyModel->setFilters(assetNORegExp, typeRegExp, acknowledgedRegExp );

}

void AlarmsManagementWidget::on_actionRefresh_triggered()
{

    if(!verifyPrivilege()) {
        return;
    }

    QString startTime = "1970-01:01", endTime = "2099-01-01";
    periodString(&startTime, &endTime);
    m_myself->packetsParser()->sendRequestSysAlarmsPacket(m_myself->socketConnectedToServer(), ui->lineEditAssetNO->text(), typeString(), acknowledgedString(), startTime, endTime);

}

void AlarmsManagementWidget::on_comboBoxPeriod_currentIndexChanged(int index)
{
    periodString(0, 0);
    Period period = Period(ui->comboBoxPeriod->currentData().toUInt());
    ui->dateTimeEditStartTime->setReadOnly((period != Period_Custom));
    ui->dateTimeEditEndTime->setReadOnly((period != Period_Custom));
}

void AlarmsManagementWidget::on_actionExport_triggered()
{
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::EXPORT, this);
    dlg.exec();
}

void AlarmsManagementWidget::on_actionPrint_triggered()
{
#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableView, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif
}

void AlarmsManagementWidget::on_actionAcknowledge_triggered()
{
    requestAcknowledgeAlarms(false);
}

void AlarmsManagementWidget::on_actionDelete_triggered()
{
    requestAcknowledgeAlarms(true);
}

void AlarmsManagementWidget::requestAcknowledgeAlarms(bool deleteAlarms)
{

    if(!verifyPrivilege()) {
        return;
    }

    m_myself->packetsParser()->sendAcknowledgeSysAlarmsPacket(m_myself->socketConnectedToServer(), m_myself->getUserID(), m_selectedInfoList, deleteAlarms);

    QTimer::singleShot(3000, this, SLOT(on_actionRefresh_triggered()));

}



void AlarmsManagementWidget::slotShowCustomContextMenu(const QPoint &pos)
{

    QTableView *tableView = qobject_cast<QTableView *> (sender());
    if (!tableView) {
        return;
    }


    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
    if(m_selectedInfoList.isEmpty()) {
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
    menu.addAction(ui->actionAcknowledge);
    menu.addAction(ui->actionDelete);

    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void AlarmsManagementWidget::getSelectedInfo(const QModelIndex &index)
{

    m_selectedInfoList.clear();

    if(!index.isValid()) {
        m_selectedInfoList.clear();
        return;
    }

    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows(0);
    if(indexList.isEmpty()) {
        return;
    }
    foreach (QModelIndex idx, indexList) {
        m_selectedInfoList.append(m_model->getAlarmInfoID(idx));
    }

    bool enableExp = m_model->rowCount();
    ui->actionExport->setEnabled(enableExp);
    ui->actionPrint->setEnabled(enableExp);

    bool enableModify = false;
    if(!m_myself->isReadonly()) {
        enableModify = true;
    }

    ui->actionAcknowledge->setEnabled(enableModify);
    ui->actionDelete->setEnabled(enableModify);

}


bool AlarmsManagementWidget::verifyPrivilege()
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

QString AlarmsManagementWidget::typeString() const
{
    return QString::number(ui->comboBoxType->currentData().toInt());
}

QString AlarmsManagementWidget::acknowledgedString() const
{
    return QString::number(ui->comboBoxAcknowledged->currentData().toInt());
}

void AlarmsManagementWidget::periodString(QString *startTime, QString *endTime)
{

    QDateTime curTime = QDateTime::currentDateTime();
    QTime zeroTime = QTime(0, 0);

    Period period = Period(ui->comboBoxPeriod->currentData().toUInt());
    switch (period) {
    case Period_Custom: {

    }
    break;

    case Period_Today: {
        ui->dateTimeEditStartTime->setDate(curTime.date());
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
    break;

    case Period_Last_24_Hours: {
        QDateTime dateTime = curTime.addSecs(-(24 * 3600));
        ui->dateTimeEditStartTime->setDateTime(dateTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
    break;

    case Period_Yesterday: {
        QDate date = curTime.date().addDays(-1);
        ui->dateTimeEditStartTime->setDate(date);
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDate(date);
        ui->dateTimeEditEndTime->setTime(QTime(23, 59));
    }
    break;

    case Period_This_Week: {
        QDate date = curTime.date();
        int dayOfWeek = date.dayOfWeek();
        date = date.addDays(-1 * (dayOfWeek - 1));
        ui->dateTimeEditStartTime->setDate(date);
        ui->dateTimeEditStartTime->setTime(zeroTime);
        ui->dateTimeEditEndTime->setDateTime(curTime);
    }
    break;

    case Period_This_Month: {
        QDate date = curTime.date();
        int day = date.day();
        date = date.addDays(-1 * (day - 1));
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

    if(startTime) {
        *startTime = ui->dateTimeEditStartTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if(endTime) {
        *endTime = ui->dateTimeEditEndTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

}




} //namespace HEHUI
