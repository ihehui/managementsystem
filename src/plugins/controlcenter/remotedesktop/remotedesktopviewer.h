#ifndef REMOTEDESKTOPVIEWER_H
#define REMOTEDESKTOPVIEWER_H

#include <QWidget>
#include <QBuffer>

#include "HHSharedGUI/ImageViewer"


class CAviFile;

namespace HEHUI {


class RemoteDesktopViewer : public ImageViewer
{
    Q_OBJECT

public:
    RemoteDesktopViewer(QWidget *parent = 0, Qt::WindowFlags flag = Qt::Widget);
    ~RemoteDesktopViewer();


    void setDesktopInfo(quint32 userSocketID, const QString &id, int desktopWidth, int desktopHeight, int blockWidth, int blockHeight);
    void updatePixmap(QList<QPoint> locations, QList<QByteArray> images);

    quint32 userSocketID();
    QString viewerID() const;



signals:
    //void signalCloseWindow();
    //void toBeDstroyed();

public slots:
    void peerDisconnected();

private slots:
    void save();

    void startRecord();
    void stopRecord();

    void showContextMenu(const QPoint &pos);
    void startOrStopRecording();

private:
    quint32 m_userSocketID;
    QString m_id;


    quint8 m_blockCount;
    QSize m_blockSize;
    //QList<QPoint> locations;
    //QList<QPixmap> pixmaps;

    QImage m_image;

#ifdef Q_OS_WIN
    CAviFile *m_aviFile;
#endif

    QString m_aviFileName;

    QAction *m_actionRecord;


};

} //namespace HEHUI

#endif // REMOTEDESKTOPVIEWER_H
