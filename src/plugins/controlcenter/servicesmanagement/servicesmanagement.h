#ifndef SERVICESMANAGEMENT_H
#define SERVICESMANAGEMENT_H

#include <QWidget>

#include "serviceinfomodel.h"
#include "../../sharedms/global_shared.h"

namespace Ui
{
class ServicesManagement;
}


namespace HEHUI
{


class ServicesManagement : public QWidget
{
    Q_OBJECT

public:
    explicit ServicesManagement(QWidget *parent = 0);
    ~ServicesManagement();



signals:
    void signalGetServicesInfo(quint8 infoType = MS::SYSINFO_SERVICES);
    void signalChangServiceConfig(const QString &serviceName, bool start, quint64 startupType);

public slots:
    void setData(const QByteArray &data);
    void serviceConfigChanged(const QString &serviceName, quint64 processID, quint64 startupType);


private slots:
    void slotShowCustomContextMenu(const QPoint &pos);

    void slotExportQueryResult();
    void slotPrintQueryResult();

    void updateSelectedServiceInfo(const QModelIndex &index);
    void changServiceStartupType();
    void changServiceStatus();

private:
    bool verifyPrivilege();


private:
    Ui::ServicesManagement *ui;

    ServiceInfoSortFilterProxyModel *m_proxyModel;
    ServiceInfoModel *m_serviceInfoModel;

    ServiceInfo *m_selectedService;


};

} //namespace HEHUI

#endif // SERVICESMANAGEMENT_H
