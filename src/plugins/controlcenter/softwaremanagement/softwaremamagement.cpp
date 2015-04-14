#include "softwaremamagement.h"
#include "ui_softwaremamagement.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QMenu>

#include "HHSharedGUI/hdataoutputdialog.h"


namespace HEHUI {



SoftwareMamagement::SoftwareMamagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoftwareMamagement)
{
    ui->setupUi(this);

    connect(ui->tableWidgetSoftware, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowCustomContextMenu(QPoint)));

    connect(ui->actionRefresh, SIGNAL(triggered()), this, SIGNAL(signalGetSoftwaresInfo()));

    QHeaderView *view = ui->tableWidgetSoftware->horizontalHeader();
    view->resizeSection(0, 300);
    view->setVisible(true);

}

SoftwareMamagement::~SoftwareMamagement()
{
    delete ui;
}

QTableWidget *SoftwareMamagement::softwareTable(){
    return ui->tableWidgetSoftware;
}

void SoftwareMamagement::setData(const QByteArray &data){
    qDebug()<<"--SoftwareMamagement::setData(...)";

    if(data.isEmpty()){
        qCritical()<<"ERROR! Invalid softwares data.";
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){
        qCritical()<<"ERROR! Invalid softwares data.";
        return;
    }

    QJsonArray array = object["Software"].toArray();

    ui->tableWidgetSoftware->clearContents();
    int softwareCount = array.size();
    ui->tableWidgetSoftware->setRowCount(softwareCount);

    for(int i=0;i<softwareCount;i++){
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 4){continue;}
        //        qDebug()<<infoArray.at(0).toString();
        //        qDebug()<<infoArray.at(1).toString();
        //        qDebug()<<infoArray.at(2).toString();
        //        qDebug()<<infoArray.at(3).toString();

        for(int j=0; j<4; j++){
            ui->tableWidgetSoftware->setItem(i, j, new QTableWidgetItem(infoArray.at(j).toString()));
        }
    }
}

void SoftwareMamagement::slotShowCustomContextMenu(const QPoint & pos){

    QTableWidget *wgt = qobject_cast<QTableWidget*> (sender());
    if (!wgt){
        return;
    }

    QMenu menu(this);
    menu.addAction(ui->actionRefresh);
    if(!wgt->rowCount()){
        menu.exec(wgt->viewport()->mapToGlobal(pos));
        return;
    }

    menu.addSeparator();
    menu.addAction(ui->actionExport);

#ifndef QT_NO_PRINTER


    ui->actionPrint->setShortcut(QKeySequence::Print);
    menu.addAction(ui->actionPrint);

    //	ui.actionPrintPreview->setShortcut(Qt::CTRL + Qt::Key_P);
    //  menu.addAction(ui.actionPrintPreview);

#endif


    menu.exec(wgt->viewport()->mapToGlobal(pos));

}

void SoftwareMamagement::slotExportQueryResult(){

    DataOutputDialog dlg(ui->tableWidgetSoftware, DataOutputDialog::EXPORT, this);
    dlg.exec();

}

void SoftwareMamagement::slotPrintQueryResult(){

#ifndef QT_NO_PRINTER
    //TODO
    DataOutputDialog dlg(ui->tableWidgetSoftware, DataOutputDialog::PRINT, this);
    dlg.exec();
#endif

}







} //namespace HEHUI
