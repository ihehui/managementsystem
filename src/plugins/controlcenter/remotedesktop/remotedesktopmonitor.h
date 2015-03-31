#ifndef REMOTEDESKTOPMONITOR_H
#define REMOTEDESKTOPMONITOR_H

#include <QWidget>

namespace Ui {
class RemoteDesktopMonitor;
}

class RemoteDesktopMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteDesktopMonitor(QWidget *parent = 0);
    ~RemoteDesktopMonitor();

private:
    Ui::RemoteDesktopMonitor *ui;
};

#endif // REMOTEDESKTOPMONITOR_H
