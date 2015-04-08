#include "remotedesktopviewer.h"


#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QApplication>
#include <QFileDialog>
#include <QMenu>

#include <QtWin>


#include "AviFile.h"

#include "HHSharedGUI/himageresourcebase.h"


namespace HEHUI {


RemoteDesktopViewer::RemoteDesktopViewer(QWidget *parent, Qt::WindowFlags flag)
    :ImageViewer(parent, flag)
{

    setAttribute(Qt::WA_DeleteOnClose);

    m_userSocketID = 0;
    m_id = "";

    m_aviFile = 0;
    m_aviFileName = "";
    m_actionRecord = 0;


}

RemoteDesktopViewer::~RemoteDesktopViewer()
{
    //emit toBeDstroyed();

    stopRecord();
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

    if(m_aviFile){

        HBITMAP bitmap = QtWin::toHBITMAP(QPixmap::fromImage(m_image));
        if(FAILED(m_aviFile->AppendNewFrame(bitmap)))
        {
            QMessageBox::critical(this, tr("Error"), QString::fromWCharArray(m_aviFile->GetLastErrorMessage()));
            stopRecord();
        }
        DeleteObject(bitmap);

    }




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

    stopRecord();
}

void RemoteDesktopViewer::save(){

    QString savePath = QApplication::applicationDirPath() + "/snapshot";
    if(!setDefaultSavePath(savePath)){
        return;
    }

    QString fileName = savePath + QString("/%1-%2.jpg").arg(m_id).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    if(!m_image.save(fileName)){
        QMessageBox::critical(this, tr("Error"), tr("Can not save image as:<p>%1</p>").arg(fileName));
    }


}

void RemoteDesktopViewer::startRecord(){

    if(m_aviFileName.trimmed().isEmpty()){

        QStringList filters;
        filters << "AVI (*.avi)" << tr("All Files (*)");

        QMultiHash <QString, QString>filtersHash;
        filtersHash.insert(".avi", filters.at(0));
        filtersHash.insert(".avi", filters.at(1) );


        QFileDialog dlg;
        QString selectedFilter;
        QString path = dlg.getSaveFileName(0, tr("Save Video As:"), defaultSavePath(), filters.join(";;"), &selectedFilter);
        if(path.isEmpty()){return;}
        QFileInfo info(path);
        QString sufffix = info.suffix().trimmed();
        if(sufffix.isEmpty()){
            sufffix = filtersHash.key(selectedFilter);
            path += sufffix;
        }

        m_aviFileName = path;

    }else{
        return;
    }


    if(!m_aviFile){
        //DWORD dwCodec = mmioFOURCC('M','S','V','C');
        //DWORD dwCodec = mmioFOURCC('I','Y','U','V');
        DWORD dwCodec = 0;
        DWORD dwFrameRate = 1;
        m_aviFile = new CAviFile(m_aviFileName.toStdWString().c_str(), dwCodec, dwFrameRate);
    }


}

void RemoteDesktopViewer::stopRecord(){
    if(m_aviFile){
        delete m_aviFile;
        m_aviFile = 0;
    }
    m_aviFileName = "";


}

void RemoteDesktopViewer::showContextMenu(const QPoint &pos){

    QMenu menu;

    if(!m_actionRecord){
        m_actionRecord = new QAction(tr("Start Recording"), this);
        connect(m_actionRecord, SIGNAL(triggered()), this, SLOT(startOrStopRecording()));
    }
    if(m_aviFile){
        m_actionRecord->setText(tr("Stop Recording"));
    }

    menu.addAction(m_actionRecord);
    menu.exec(mapToGlobal(pos));
}

void RemoteDesktopViewer::startOrStopRecording(){
    if(m_aviFile){
        stopRecord();
    }else{
        startRecord();
    }
}






} //namespace HEHUI
