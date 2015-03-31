#include "remotedesktopviewer.h"


#include <QPainter>


namespace HEHUI {

RemoteDesktopViewer::RemoteDesktopViewer(QWidget *parent)
    :ImageViewer(parent)
{

    setAttribute(Qt::WA_DeleteOnClose);

}

RemoteDesktopViewer::~RemoteDesktopViewer()
{
    emit toBeDstroyed();
}

void RemoteDesktopViewer::setDesktopInfo(const QString &id, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight){
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




}

void RemoteDesktopViewer::updatePixmap(QList<QPoint> locations, QList<QByteArray> images){

    QPainter painter(&m_image);

    for(int i=0;i<locations.size();i++){
        QPoint point = locations.at(i);
        QRect target(point, m_blockSize);
        QRect source(QPoint(0, 0), m_blockSize);
        QImage image = QImage::fromData(images.at(i));

        painter.drawImage(target, image, source);
    }

    updateAnimationFrame(m_image);

}

QString RemoteDesktopViewer::viewerID() const{
    return m_id;
}











} //namespace HEHUI
