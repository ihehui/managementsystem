#include "remotedesktopmonitor.h"
#include "ui_remotedesktopmonitor.h"

#include <QMdiSubWindow>
#include <QDebug>

namespace HEHUI {



RemoteDesktopMonitor::RemoteDesktopMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteDesktopMonitor)
{
    ui->setupUi(this);
}

RemoteDesktopMonitor::~RemoteDesktopMonitor()
{
    delete ui;

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        window->close();
    }

}

void RemoteDesktopMonitor::setDesktopInfo(const QString &userID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){

    RemoteDesktopViewer *viewer = m_remoteDesktopHash.value(userID);
    if(!viewer){
        viewer = new RemoteDesktopViewer(this);
        //connect(contactChatWindow, SIGNAL(signalCloseWindow()), this, SLOT(handleCloseWindowRequest()));
        connect(viewer, SIGNAL(toBeDstroyed()), this, SLOT(handleWindowClosed()));

        ui->mdiArea->addSubWindow(viewer);
        viewer->show();
        m_remoteDesktopHash.insert(userID, viewer);
    }

    viewer->setDesktopInfo(userID, desktopWidth, desktopHeight, blockWidth, blockHeight);


}

void RemoteDesktopMonitor::updateScreenshot(const QString &userID, QList<QPoint> locations, QList<QByteArray> images){

    RemoteDesktopViewer *viewer = m_remoteDesktopHash.value(userID);
    if(!viewer){
        qCritical()<<QString("ERROR! Viewer for '%1' not found.").arg(userID);
        return;
    }

    viewer->updatePixmap(locations, images);

}


void RemoteDesktopMonitor::handleCloseWindowRequest(){

    QMdiSubWindow * subWindow = ui->mdiArea->currentSubWindow();
    if(subWindow){
        subWindow->close();
    }

}

void RemoteDesktopMonitor::handleWindowClosed(){

    RemoteDesktopViewer *wgt = qobject_cast<RemoteDesktopViewer *>(sender());
    if(wgt){
        m_remoteDesktopHash.remove(wgt->viewerID());
    }

}




} //namespace HEHUI
