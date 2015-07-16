/*
 ****************************************************************************
 * servermanagerwindow.h
 *
 * Created On: 2010-5-24
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified On: 2015-4-16
 * Last Modified By: 贺辉
 ***************************************************************************
 */





#ifndef SERVERADDRESSMANAGERWINDOW_H
#define SERVERADDRESSMANAGERWINDOW_H

#include <QWidget>

#include "serverinfomodel.h"


namespace Ui {
class ServerAddressManagerWindowClass;
}

namespace HEHUI {

class ServerDiscoveryPacket;

class ServerAddressManagerWindow : public QWidget
{
    Q_OBJECT

public:
    ServerAddressManagerWindow(QWidget *parent = 0);
    ~ServerAddressManagerWindow();


protected:
    void closeEvent(QCloseEvent *event);

private:
    void updateModel();
    bool isIPAddressValid();
    
signals:
    void signalLookForServer(const QString &targetAddress, quint16 targetPort);
    void signalServerSelected(const QString &serverAddress, quint16 serverPort, const QString &serverName, const QString &version);
    

public slots:
    void serverFound(const ServerDiscoveryPacket &packet);
    
private slots:
    void slotRequestForLANServer(const QString &ip, quint16 port);
    void slotTestServers();
    void slotTimeout();

    void slotSaveServers();
    void slotLoadServers();

    void slotUpdateUI();

    void on_toolButtonSearchServer_clicked();
    void on_toolButtonAddServer_clicked();
    void on_lineEditIP_editingFinished ();
    void on_toolButtonDeleteServer_clicked();
    void on_toolButtonTestServers_clicked();

    void slotServerSelected(const QModelIndex &index);



private:
    Ui::ServerAddressManagerWindowClass *ui;

    ServerInfoModel *model;

    QHash<QString/*Server Address*/, ServerInfo *> serversHash;



};

} //namespace HEHUI

#endif // SERVERADDRESSMANAGERWINDOW_H
