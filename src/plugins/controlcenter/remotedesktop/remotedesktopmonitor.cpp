#include "remotedesktopmonitor.h"
#include "ui_remotedesktopmonitor.h"

#include <QMdiSubWindow>
#include <QDebug>

namespace HEHUI
{



RemoteDesktopMonitor::RemoteDesktopMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteDesktopMonitor)
{
    ui->setupUi(this);
}

RemoteDesktopMonitor::~RemoteDesktopMonitor()
{

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        window->close();
    }

    delete ui;

}

void RemoteDesktopMonitor::setDesktopInfo(quint32 userSocketID, const QString &userID, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight)
{

    RemoteDesktopViewer *viewer = m_remoteDesktopHash.value(userID);
    if(!viewer) {
        viewer = new RemoteDesktopViewer(this);
        viewer->setScaleButtonsVisible(true);
        viewer->setRotateButtonsVisible(false);
        viewer->setFlipButtonsVisible(false);
        viewer->setCloseButtonVisible(false);
        viewer->setDragable(false);
        viewer->setWindowTitle(userID);
        //connect(contactChatWindow, SIGNAL(signalCloseWindow()), this, SLOT(handleCloseWindowRequest()));
        //connect(viewer, SIGNAL(toBeDstroyed()), this, SLOT(handleSubWindowClosed()));

        QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(viewer);
        subWindow->installEventFilter(this);

        viewer->show();
        m_remoteDesktopHash.insert(userID, viewer);
    }

    viewer->setDesktopInfo(userSocketID, userID, desktopWidth, desktopHeight, blockWidth, blockHeight);

}

void RemoteDesktopMonitor::updateScreenshot(const QString &userID, QList<QPoint> locations, QList<QByteArray> images)
{
    //qDebug()<<"--RemoteDesktopMonitor::updateScreenshot(...)";

    RemoteDesktopViewer *viewer = m_remoteDesktopHash.value(userID);
    if(!viewer) {
        qCritical() << QString("ERROR! Viewer for '%1' not found.").arg(userID);
        return;
    }

    viewer->updatePixmap(locations, images);

}

bool RemoteDesktopMonitor::eventFilter(QObject *obj, QEvent *event)
{

    switch (event->type()) {

    case QEvent::Close: {

        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow *>(obj);
        if(subWindow) {
            RemoteDesktopViewer *viewer = qobject_cast<RemoteDesktopViewer *>(subWindow->widget());
            if(viewer) {
                m_remoteDesktopHash.remove(viewer->viewerID());
                quint32 socketID = viewer->userSocketID();
                if(socketID) {
                    emit closeUserSocket(socketID);
                }
                delete viewer;
            }

        }

    }
    break;

    default:
        break;
    }

    return QObject::eventFilter(obj, event);

}


void RemoteDesktopMonitor::peerDisconnected(quint32 socketID)
{
    foreach (RemoteDesktopViewer *viewer, m_remoteDesktopHash.values()) {
        if(viewer->userSocketID() == socketID) {
            //viewer->setText(tr("Disconnected"));
            viewer->peerDisconnected();
        }
    }
}


//void RemoteDesktopMonitor::handleCloseWindowRequest(){

//    QMdiSubWindow * subWindow = ui->mdiArea->currentSubWindow();
//    if(subWindow){
//        subWindow->close();
//    }

//}






} //namespace HEHUI
