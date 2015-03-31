#include "remotedesktopmonitor.h"
#include "ui_remotedesktopmonitor.h"

RemoteDesktopMonitor::RemoteDesktopMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteDesktopMonitor)
{
    ui->setupUi(this);
}

RemoteDesktopMonitor::~RemoteDesktopMonitor()
{
    delete ui;
}
