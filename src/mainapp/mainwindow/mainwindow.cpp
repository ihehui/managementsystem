/*
 ****************************************************************************
 * mainwindow.cpp
 *
 * Created on: 2009-4-27
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
  * Last Modified on: 2010-05-17
  * Last Modified by: 贺辉
  ***************************************************************************
*/




#include <QtGui>

#include "mainwindow.h"
#include "../about/aboutdialog.h"
#include "../shared/settings.h"

#include "HHSharedCore/DatabaseUtility"
#include "HHSharedGUI/LoginBase"
#include "HHSharedGUI/GUIInterface"
#include "HHSharedGUI/GUIPluginBase"
#include "HHSharedGUI/PluginManagerWindow"
#include "HHSharedCore/CoreUtilities"



namespace HEHUI
{


MainWindow::MainWindow(const QString &settingsFile, QWidget *parent) :
    MainWindowBase(settingsFile, parent)
{
    ui.setupUi(this);

    setWindowTitle(QString(APP_NAME) + " Build " + QString(APP_VERSION));
    //setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::WindowStaysOnTopHint);


    //resize(QSize(0,0));
    //showMinimized();
    //showMaximized();

    //应用样式
    //Apply the app style
    //	originalPalette = QApplication::palette();
    //	changeStyle(Settings::instance()->getStyle());


    //初始化UI
    //Init the UI
    initUI();

    //创建托盘图标
    //Create the system tray
    setupSystemTray();

    if(Settings::instance()->getRestoreWindowStateOnStartup()) {
        Settings::instance()->restoreState(this);
    } else {
        showMaximized();
    }


    //        qmLocale = Settings::instance()->getLanguage();
    //        qmPath = QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR);
    //更新菜单
    //Update the menus
    //setupMenus();



}

MainWindow::~MainWindow()
{
    qDebug() << "--MainWindow::~MainWindow()";

    if(Settings::instance()->getRestoreWindowStateOnStartup()) {
        Settings::instance()->saveState(this);
    }

    //Close all database connections
    DatabaseUtility::closeAllDBConnections();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Close all subwindows
    ui.mdiArea->closeAllSubWindows();
    if(ui.mdiArea->activeSubWindow()) {
        event->ignore();
        return;
    }

    unloadPlugins();

    event->accept();
}


void MainWindow::changeEvent ( QEvent *event )
{
    if(event->type() == QEvent::WindowStateChange) {
        switch (windowState ()) {
        case Qt::WindowNoState:
            ui.actionShowMaximized->setEnabled(true);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(false);
            break;
        case Qt::WindowMinimized:
            ui.actionShowMaximized->setEnabled(true);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(false);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowMaximized:
            ui.actionShowMaximized->setEnabled(false);
            ui.actionShowFullScreen->setEnabled(true);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        case Qt::WindowFullScreen:
            ui.actionShowMaximized->setEnabled(false);
            ui.actionShowFullScreen->setEnabled(false);
            ui.actionShowMinimized->setEnabled(true);
            ui.actionShowNormal->setEnabled(true);
            break;
        default:
            //TODO: Other State
            break;
        }

        event->accept();
    }

    event->ignore();

}

void MainWindow::initUI()
{

    ui.menuView->addSeparator();
    ui.menuView->addMenu(getStyleMenu());
    QStringList qmPathList;
    qmPathList.append(QApplication::applicationDirPath() + QDir::separator () + QString(LANGUAGE_FILE_DIR));
    qmPathList.append(":/translations");
    QString qmLocale = guiUtilities()->getPreferedLanguage();
    guiUtilities()->setTranslationFileDirList(qmPathList);
    ui.menuView->addMenu(getLanguageMenu());

    pluginsMenu = getPluginsMenu();
    menuBar()->insertMenu(ui.menuHelp->menuAction(), pluginsMenu);
    ui.toolBarPlugins->addAction(getPluginsManagementAction());


    connect(ui.actionBugReport, SIGNAL(triggered()), this, SLOT(slotBugReport()));

    connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(slotHelp()));


    connect(ui.actionSystemConfigure, SIGNAL(triggered()), this, SLOT(slotSystemConfig()));

    connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(slotQuit()));


    //connect(ui.menuSubWindows, SIGNAL(aboutToShow()), this, SLOT(slotUpdateWindowActions()));
    connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(slotUpdateWindowActions()));

}

void MainWindow::slotInitPlugin(AbstractPluginInterface *plugin)
{
    qDebug("----MainWindow::slotInitPlugin(AbstractPluginInterface *plugin)");

    if(!plugin) {
        return;
    }

    GUIInterface *guiPlugin = static_cast<GUIInterface *> (plugin);
    if(guiPlugin) {
        guiPlugin->initialize(ui.mdiArea, pluginsMenu, ui.toolBarPlugins, systemTray, APP_NAME, APP_VERSION);
        pluginsMenu->addMenu(guiPlugin->menu());
        ui.toolBarPlugins->addAction(guiPlugin->menu()->menuAction());
    }
}

void MainWindow::setupSystemTray()
{
    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(QString(RESOURCE_PATH) + QString(APP_ICON_PATH)));
    systemTray->setToolTip(APP_NAME);
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotIconActivated(QSystemTrayIcon::ActivationReason)));

    trayMenu = new QMenu(this);
    trayMenu->addMenu(pluginsMenu);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionShowMaximized);
    trayMenu->addAction(ui.actionShowMinimized);
    trayMenu->addAction(ui.actionShowFullScreen);
    trayMenu->addAction(ui.actionShowNormal);
    trayMenu->addSeparator();
    trayMenu->addAction(ui.actionQuit);

    systemTray->setContextMenu(trayMenu);
    systemTray->show();

}

QSystemTrayIcon *MainWindow::SystemTrayIcon()
{
    if (!systemTray) {
        setupSystemTray();
    }

    return systemTray;
}

void MainWindow::setTrayIconVisible(bool visible)
{
    visible = true;
}

void MainWindow::slotIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:

    case QSystemTrayIcon::DoubleClick:
        //systemInfo();
        break;
    case QSystemTrayIcon::MiddleClick:
        slotAbout();
        break;
    default:
        ;
    }
}

void MainWindow::slotSystemConfig()
{
    QMessageBox::information(this, tr("~_~"), tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotSystemConfig()";
}

void MainWindow::slotBugReport()
{
    QMessageBox::information(this, tr("~_~"), tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotBugReport()";
}

void MainWindow::slotHelp()
{
    QMessageBox::information(this, tr("~_~"), tr(" Not accomplished !"));
    qDebug() << "----MainWindow::slotHelp()";
}

void MainWindow::slotAbout()
{
    AboutDialog aboutDlg(this);
    aboutDlg.exec();

}

void MainWindow::slotQuit()
{
    //Closes all database connections
    //dbo->closeAllDBConnections();

    //writeSettings();
    //Settings::instance()->saveState(this);

    systemTray->hide();

    qApp->quit();

}

void MainWindow::retranslateUi()
{
    //重新翻译UI
    //Retranslate UI
    ui.retranslateUi(this);
}

bool MainWindow::hasActiveMDIChild()
{
    if (ui.mdiArea->activeSubWindow()) {
        return true;
    }
    return false;
}

void MainWindow::slotUpdateWindowActions()
{
    bool hasSubWindow = (ui.mdiArea->subWindowList().size() > 0);
    bool hasSubWindows = (ui.mdiArea->subWindowList().size() > 1);

    ui.actionCloseSubWindow->setEnabled(hasSubWindow);
    ui.actionCloseAllSubWindows->setEnabled(hasSubWindow);

    ui.actionTitleSubWindows->setEnabled(hasSubWindow);
    ui.actionCascadeSubWindows->setEnabled(hasSubWindow);

    ui.actionNextSubWindow->setEnabled(hasSubWindows);
    ui.actionPreviousSubWindow->setEnabled(hasSubWindows);

    ui.menuSubWindows->setEnabled(hasSubWindows);


    /*

    ui.menuSubWindows->clear();

    QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();

    //ui.menuSubWindows->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QWidget *child = qobject_cast<QWidget *>(windows.at(i)->widget());

       QString text;
       if (i < 9) {
          text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
       } else {
             text = tr("%1 %2").arg(i + 1).arg(child->windowTitle());
       }
       QAction *action  = ui.menuSubWindows->addAction(text);
       action->setCheckable(true);

       //action ->setChecked(child == qobject_cast<QWidget *>(ui.mdiArea->activeSubWindow()));

       connect(action, SIGNAL(triggered()), ui.mdiArea, SLOT(setActiveSubWindow(child)));

    }
    */

}

















} //namespace HEHUI
