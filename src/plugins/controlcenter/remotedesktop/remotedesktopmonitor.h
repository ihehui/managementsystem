#ifndef REMOTEDESKTOPMONITOR_H
#define REMOTEDESKTOPMONITOR_H

#include <QWidget>

#include "remotedesktopviewer.h"

namespace Ui
{
class RemoteDesktopMonitor;
}


namespace HEHUI
{



class RemoteDesktopMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteDesktopMonitor(QWidget *parent = 0);
    ~RemoteDesktopMonitor();

    void setDesktopInfo(quint32 userSocketID, const QString &userID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight);
    void updateScreenshot(const QString &userID, QList<QPoint> locations, QList<QByteArray> images);



protected:
    bool eventFilter(QObject *obj, QEvent *event);


signals:
    void closeUserSocket(quint32 userSocketID);
    void signalClose();

public slots:
    void peerDisconnected(quint32 socketID);


private slots:
//    void handleCloseWindowRequest();

private:
    Ui::RemoteDesktopMonitor *ui;

    QHash<QString/*ID*/, RemoteDesktopViewer *> m_remoteDesktopHash;


};

} //namespace HEHUI

#endif // REMOTEDESKTOPMONITOR_H
