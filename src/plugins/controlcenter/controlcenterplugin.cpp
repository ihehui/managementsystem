/*
 ****************************************************************************
 * controlcenterplugin.cpp
 *
 * Created on: 2010-07-13
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2010-07-13
 * Last Modified by: 贺辉
 ***************************************************************************
 */




#include "controlcenterplugin.h"
#include "constants.h"


#include "HHSharedCore/User"
#include "HHSharedCore/SettingsCore"
#include "HHSharedGUI/LoginBase"


namespace HEHUI
{


ControlCenterPlugin::ControlCenterPlugin()
    : GUIPluginBase()
{
    widgetList = QList<QWidget *> ();

}

ControlCenterPlugin::~ControlCenterPlugin()
{
    qDebug() << "--ControlCenterPlugin::~ControlCenterPlugin()";
}

bool ControlCenterPlugin::isSingle()
{
    return true;
}

QString ControlCenterPlugin::name () const
{
    return QString(tr("Control Center"));
}

QString ControlCenterPlugin::version() const
{
    return "2010.05.10";
}

QString ControlCenterPlugin::description() const
{
    return QString(tr("Control Center Plugin"));
}

QIcon ControlCenterPlugin::icon () const
{
    return QIcon(":/icon/resources/images/controlcenter.png");
}

QString ControlCenterPlugin::whatsThis () const
{
    return QString(tr("Control Center"));
}

QString ControlCenterPlugin::toolTip () const
{
    return QString(tr("Control Center"));
}

bool ControlCenterPlugin::unload()
{

    //    emit signalPluginToBeUnloaded();

    foreach(QWidget *wgt, widgetList) {
        if(!wgt) {
            continue;
        }
        if(wgt->close()) {
            //widgetList.removeAll(wgt);
            delete wgt;
            wgt = 0;
        }
    }

    //    if(controlCenterWidgetList.isEmpty()){
    //        return true;
    //    }

    //    QWidget *parentWidget = qobject_cast<QWidget *> (parent());
    //    if(!parentWidget){
    //        foreach(ControlCenter *controlCenter, controlCenterWidgetList){
    //            if(!controlCenter){break;}
    //            if(controlCenter->close()){
    //                controlCenterWidgetList.removeAll(controlCenter);
    //                delete controlCenter;
    //                controlCenter = 0;
    //            }
    //        }
    //    }

    return widgetList.isEmpty();
}

void ControlCenterPlugin::slotMainActionForMenuTriggered()
{
    if(isSingle() && ControlCenter::isRunning()) {
        //TODO: Activate the widget
        return;
    }


    QWidget *parentWidget = qobject_cast<QWidget *> (parent());
    ControlCenter *wgt = new ControlCenter(name(), parentWidget);
    //connect(controlCenter, SIGNAL(destroyed(QObject *)), SLOT(slotControlCenterWidgetDestoryed(QObject *)));

    AdminUser *m_myself = AdminUser::instance();
    if(!m_myself->isAdminVerified()) {
        wgt->close();
        wgt->deleteLater();
        return;
    }

    QMdiArea *mdiArea = 0;
    if(parentWidget) {
        mdiArea = qobject_cast<QMdiArea *>(parentWidget);
    }
    if(mdiArea) {
        QMdiSubWindow *subWindow = new QMdiSubWindow;
        subWindow->setWidget(wgt);
        subWindow->setAttribute(Qt::WA_DeleteOnClose);
        mdiArea->addSubWindow(subWindow);
        //connect(this, SIGNAL(signalPluginToBeUnloaded()), subWindow, SLOT(close()));

        connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestoryed(QObject *)));
        widgetList.append(subWindow);
    } else {
        connect(wgt, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestoryed(QObject *)));
        widgetList.append(wgt);
    }

    wgt->show();
}

void ControlCenterPlugin::slotWidgetDestoryed(QObject *obj)
{
    qDebug() << "--ControlCenterPlugin::slotWidgetDestoryed(...)";

    QWidget *wgt = static_cast<QWidget *> (sender());
    if(wgt) {
        widgetList.removeAll(wgt);
    }
}


//Q_EXPORT_PLUGIN2(controlCenterPlugin, ControlCenterPlugin);


} //namespace HEHUI
