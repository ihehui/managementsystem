#include "remotedesktopviewer.h"


#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QApplication>



namespace HEHUI {


RemoteDesktopViewer::RemoteDesktopViewer(QWidget *parent, Qt::WindowFlags flag)
    :ImageViewer(parent, flag)
{

    setAttribute(Qt::WA_DeleteOnClose);

    m_userSocketID = 0;
    m_id = "";


}

RemoteDesktopViewer::~RemoteDesktopViewer()
{
    //emit toBeDstroyed();
}

void RemoteDesktopViewer::setDesktopInfo(quint32 userSocketID, const QString &id, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){

    m_userSocketID = userSocketID;
    m_id = id;

    m_blockSize = QSize(blockWidth, blockHeight);
    //m_blockCount = (desktopWidth*desktopHeight)/(blockWidth*blockHeight);

    int columnCount = desktopWidth/blockWidth;
    int rowCount = desktopHeight/blockHeight;
    m_blockCount = columnCount*rowCount;

//    for(int i=0;i<columnCount;i++){
//        for(int j=0;j<rowCount;j++){
//            locations.append(QPoint(i*blockWidth, j*blockHeight));
//        }
//    }


    m_image = QImage(desktopWidth, desktopHeight, QImage::Format_RGB32);
    m_image.fill(Qt::lightGray);

//    QPainter painter(&m_image);
//    //painter.setCompositionMode(QPainter::CompositionMode_HardLight);
//    painter.fillRect(QRect(0, 0, desktopWidth, desktopHeight), Qt::lightGray);
//    painter.end();


}

void RemoteDesktopViewer::updatePixmap(QList<QPoint> locations, QList<QByteArray> images){
    //qDebug()<<"--RemoteDesktopViewer::updatePixmap(...)";

    QPainter painter(&m_image);

    for(int i=0;i<locations.size();i++){
        QPoint point = locations.at(i);
        QRect target(point, m_blockSize);
        QRect source(QPoint(0, 0), m_blockSize);
        QImage image = QImage::fromData(images.at(i));

        painter.drawImage(target, image, source);
    }
    painter.end();

    updateAnimationFrame(m_image);

}

quint32 RemoteDesktopViewer::userSocketID(){
    return m_userSocketID;
}

QString RemoteDesktopViewer::viewerID() const{
    return m_id;
}

void RemoteDesktopViewer::peerDisconnected(){
    qDebug()<<"--RemoteDesktopViewer::peerDisconnected()";

    m_blockSize = QSize(0, 0);
    m_image = QImage();

    m_userSocketID = 0;
    setText(tr("Disconnected"));
}

void RemoteDesktopViewer::save(){

    QString savePath = QApplication::applicationDirPath() + "/snapshot";
    QDir dir;
    if(!dir.mkpath(savePath)){
        QMessageBox::critical(this, tr("Error"), tr("Can not create path:<p>%1</p>").arg(savePath));
        return;
    }

    QString fileName = savePath + QString("/%1-%2.jpg").arg(m_id).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    if(!m_image.save(fileName)){
        QMessageBox::critical(this, tr("Error"), tr("Can not save image as:<p>%1</p>").arg(fileName));
    }


}

void RemoteDesktopViewer::showContextMenu(const QPoint &pos){

}






} //namespace HEHUI
