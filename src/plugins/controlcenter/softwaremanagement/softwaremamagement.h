#ifndef SOFTWAREMAMAGEMENT_H
#define SOFTWAREMAMAGEMENT_H

#include <QWidget>
#include <QTableWidget>

#include "../../sharedms/global_shared.h"



namespace Ui
{
class SoftwareMamagement;
}

namespace HEHUI
{


class SoftwareMamagement : public QWidget
{
    Q_OBJECT

public:
    explicit SoftwareMamagement(QWidget *parent = 0);
    ~SoftwareMamagement();

    QTableWidget *softwareTable();

signals:
    void signalGetSoftwaresInfo(quint8 infoType = MS::SYSINFO_SOFTWARE);




public slots:
    void setData(const QByteArray &data);

private slots:
    void slotShowCustomContextMenu(const QPoint &pos);

    void slotExportQueryResult();
    void slotPrintQueryResult();

private:
    Ui::SoftwareMamagement *ui;



};

} //namespace HEHUI

#endif // SOFTWAREMAMAGEMENT_H
