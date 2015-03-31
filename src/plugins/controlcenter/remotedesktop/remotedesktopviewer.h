#ifndef REMOTEDESKTOPVIEWER_H
#define REMOTEDESKTOPVIEWER_H

#include <QWidget>
#include <QBuffer>

#include "HHSharedGUI/ImageViewer"


namespace HEHUI {


class RemoteDesktopViewer : public ImageViewer
{
    Q_OBJECT

public:
    RemoteDesktopViewer(QWidget *parent = 0);
    ~RemoteDesktopViewer();


    void setDesktopInfo(const QString &id, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight);
    void updatePixmap(QList<QPoint> locations, QList<QByteArray> images);


    QString viewerID() const;


signals:
    //void signalCloseWindow();
    void toBeDstroyed();




private:

    QString m_id;

    quint8 m_blockCount;
    QSize m_blockSize;
    //QList<QPoint> locations;
    //QList<QPixmap> pixmaps;

    QImage m_image;


};

} //namespace HEHUI

#endif // REMOTEDESKTOPVIEWER_H
