#ifndef SERVERMANAGEMENTWIDGET_H
#define SERVERMANAGEMENTWIDGET_H

#include <QWidget>
#include <QCloseEvent>

#include "adminsmanagementwidget.h"
#include "alarmsmanagementwidget.h"



namespace Ui {
class ServerManagementWidget;
}

namespace HEHUI {


class ServerManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerManagementWidget(QWidget *parent = 0);
    ~ServerManagementWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *e);

public slots:
    void setAdminsData(const QByteArray &infoData);
    void setAlarmsData(const QByteArray &infoData);

    void updateServerInfo(const QByteArray &infoData);
    void updateRealtimeInfo(const QByteArray &infoData);


private slots:
    void initTabWidget();
    void slotTabPageChanged();
    void slotcloseTab();

    void showTabMenu();
    void showMenu(const QPoint & pos);

    void manageAdmins();
    void manageAlarms();

    bool verifyPrivilege();
    void adminVerified();





private:
    Ui::ServerManagementWidget *ui;

    AdminUser *m_myself;
    //SOCKETID m_socketConnectedToServer;

    AdminsManagementWidget *m_adminsWidget;
    AlarmsManagementWidget *m_alarmsWidget;


};

} //namespace HEHUI

#endif // SERVERMANAGEMENTWIDGET_H
