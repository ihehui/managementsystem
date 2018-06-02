#include "servicesmanagement.h"
#include "ui_servicesmanagement.h"

#include <QMenu>

#include "../adminuser.h"
#include "HHSharedGUI/hdataoutputdialog.h"

#include "HHSharedSystemUtilities/SystemUtilities"


namespace HEHUI
{



ServicesManagement::ServicesManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServicesManagement)
{
    ui->setupUi(this);

    connect(ui->tableViewServices, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));
    connect(ui->tableViewServices, SIGNAL(clicked(const QModelIndex &)), this, SLOT(updateSelectedServiceInfo(const QModelIndex &)));

    connect(ui->actionRefresh, SIGNAL(triggered()), this, SIGNAL(signalGetServicesInfo()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(slotExportQueryResult()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(slotPrintQueryResult()));

    connect(ui->actionAutoStart, SIGNAL(triggered()), this, SLOT(changServiceStartupType()));
    connect(ui->actionManuallyStart, SIGNAL(triggered()), this, SLOT(changServiceStartupType()));
    connect(ui->actionDisableService, SIGNAL(triggered()), this, SLOT(changServiceStartupType()));

    connect(ui->actionStartService, SIGNAL(triggered()), this, SLOT(changServiceStatus()));
    connect(ui->actionStopService, SIGNAL(triggered()), this, SLOT(changServiceStatus()));



    m_serviceInfoModel = new ServiceInfoModel(this);
    m_proxyModel = new ServiceInfoSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_serviceInfoModel);
    m_proxyModel->setDynamicSortFilter(true);

    ui->tableViewServices->setModel(m_proxyModel);
    m_serviceInfoModel->setJsonData(QByteArray());

    m_selectedService = 0;


    QHeaderView *view = ui->tableViewServices->horizontalHeader();
    view->resizeSection(1, 200);
    view->resizeSection(3, 200);
    view->setVisible(true);

}

ServicesManagement::~ServicesManagement()
{
    delete ui;

    if(m_proxyModel) {
        delete m_proxyModel;
        m_proxyModel = 0;
    }

    if(m_serviceInfoModel) {
        delete m_serviceInfoModel;
        m_serviceInfoModel = 0;
    }

}

void ServicesManagement::setData(const QByteArray &data)
{
    m_selectedService = 0;
    unsigned long errorCode = 0;
    QString errorMessage = "";
    m_serviceInfoModel->setJsonData(data, &errorCode, &errorMessage);
    if(errorCode){
        QMessageBox::critical(this, tr("Error"), tr("Failed to get services info!<p>Error Code: %1</p><p>Error Message: %2</p>").arg(errorCode).arg(errorMessage));
    }
}

void ServicesManagement::serviceConfigChanged(const QString &serviceName, quint64 processID, quint64 startupType)
{

    ServiceInfo *info = m_serviceInfoModel->getServiceInfo(serviceName);
    if(!info) {
        return;
    }
    if(info->processID != processID) {
        QMessageBox::information(this, tr("Service"), tr("Service '%1' %2 successfully.").arg(serviceName).arg(processID ? tr("started") : tr("stopped")));
    }

    if(info->startType != startupType) {
        QMessageBox::information(this, tr("Service"), tr("Service '%1' startup type changed successfully.").arg(serviceName));
    }

    m_serviceInfoModel->updateServiceInfo(serviceName, processID, startupType);


}

void ServicesManagement::slotShowCustomContextMenu(const QPoint &pos)
{

    QTableView *tableView = qobject_cast<QTableView *> (sender());
    if (!tableView) {
        return;
    }

    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
    if(!m_selectedService) {
        menu.exec(tableView->viewport()->mapToGlobal(pos));
        return;
    }

    menu.addSeparator();
    menu.addAction(ui->actionExport);

#ifndef QT_NO_PRINTER


    ui->actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui->actionPrint);

    //	ui.actionPrintPreview->setShortcut(Qt::CTRL + Qt::Key_P);
    //  menu.addAction(ui.actionPrintPreview);

#endif
    if(AdminUser::instance()->isReadonly()) {
        menu.exec(tableView->viewport()->mapToGlobal(pos));
        return;
    }


    menu.addSeparator();
    menu.addAction(ui->actionStartService);
    menu.addAction(ui->actionStopService);

    menu.addSeparator();
    QMenu startupTypeMenu(tr("Startup Type"), this);
    startupTypeMenu.addAction(ui->actionAutoStart);
    startupTypeMenu.addAction(ui->actionManuallyStart);
    startupTypeMenu.addAction(ui->actionDisableService);

    QActionGroup group(&startupTypeMenu);
    group.addAction(ui->actionAutoStart);
    group.addAction(ui->actionManuallyStart);
    group.addAction(ui->actionDisableService);


    menu.addMenu(&startupTypeMenu);
    menu.exec(tableView->viewport()->mapToGlobal(pos));

}

void ServicesManagement::slotExportQueryResult()
{

    DataOutputDialog dlg(ui->tableViewServices, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void ServicesManagement::slotPrintQueryResult()
{

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableViewServices, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}

void ServicesManagement::updateSelectedServiceInfo(const QModelIndex &index)
{

    if(!index.isValid()) {
        return;
    }

    QString serviceName = index.sibling(index.row(), 0).data().toString();
    m_selectedService = m_serviceInfoModel->getServiceInfo(serviceName);
    if(!m_selectedService) {
        return;
    }

    //bool selected = ui->tableViewServices->currentIndex().isValid() && ui->tableViewServices->selectionModel()->selectedIndexes().size();

    ui->actionAutoStart->setEnabled(m_selectedService->startType != SERVICE_AUTO_START);
    ui->actionManuallyStart->setEnabled(m_selectedService->startType != SERVICE_DEMAND_START);
    ui->actionDisableService->setEnabled(m_selectedService->startType != SERVICE_DISABLED);

    ui->actionStartService->setEnabled((m_selectedService->processID == 0) && (m_selectedService->startType != SERVICE_DISABLED));
    ui->actionStopService->setEnabled(m_selectedService->processID != 0);


}

void ServicesManagement::changServiceStartupType()
{

    if(!verifyPrivilege()) {
        return;
    }


    if(!m_selectedService) {
        return;
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if(!action) {
        return;
    }

    AdminUser *adminUser = AdminUser::instance();
    if(!adminUser->isAdminVerified()) {
        return;
    }
    if(adminUser->isReadonly()) {
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return;
    }

    unsigned long startupType;
    if(action == ui->actionAutoStart) {
        startupType = SERVICE_AUTO_START;
    } else if(action == ui->actionManuallyStart) {
        startupType = SERVICE_DEMAND_START;
    } else {
        startupType = SERVICE_DISABLED;
    }

    emit signalChangServiceConfig(m_selectedService->serviceName, m_selectedService->processID, startupType);

}

void ServicesManagement::changServiceStatus()
{

    if(!verifyPrivilege()) {
        return;
    }

    if(!m_selectedService) {
        return;
    }

    QAction *action = qobject_cast<QAction *>(sender());
    if(!action) {
        return;
    }

    unsigned long startupType = 0xFFFFFFFF;
    emit signalChangServiceConfig(m_selectedService->serviceName, !m_selectedService->processID, startupType);

}

bool ServicesManagement::verifyPrivilege()
{

    AdminUser *adminUser = AdminUser::instance();
    if(!adminUser->isAdminVerified()) {
        return false;
    }
    if(adminUser->isReadonly()) {
        QMessageBox::critical(this, tr("Access Denied"), tr("You dont have the access permissions!"));
        return false;
    }

    return true;

}



} //namespace HEHUI
