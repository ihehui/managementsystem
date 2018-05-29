# ##
TEMPLATE = lib
CONFIG += plugin shared
TARGET = controlcenter

# VERSION = 2009.09.08.1055
QT += core \
    gui \
    widgets \
    sql \
    network \
    printsupport


win32: QT += winextras

# include config file
include( ../../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedENET #\
#    HHSharedUDT
win32:HHSharedLibs += HHSharedSysUtilities
include(../../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../../sharedms/sharedms.pri)

# ##
UI_DIR = ./ui

INCLUDEPATH +=  $$UI_DIR \
                $$PWD \
                filemanagement \
                servicesmanagement \
                usersmanagement \
                softwaremanagement \
                processmonitor \
                servermanagement

# Input
HEADERS += \
    networkmanager/controlcenterpacketsparser.h \
    taskmanagement/taskinfoviewwidget.h \
    taskmanagement/taskmanagement.h \
    systemmanagement/systemmanagementwidget.h \
    controlcenter.h \
    controlcenterplugin.h \
    constants.h \
    networkmanager/resourcesmanagerinstance.h \
    servicesmanagement/servicesmanagement.h \
    servicesmanagement/serviceinfomodel.h \
    clientmanagement/clientinfomodel.h \
    systemmanagement/shutdowndialog.h \
    usersmanagement/winuserinfomodel.h \
    usersmanagement/winuserinfowidget.h \
    usersmanagement/winusermanagerwidget.h \
    usersmanagement/winuserinfo.h \
    softwaremanagement/softwaremamagement.h \
    remotedesktop/remotedesktopmonitor.h \
    remotedesktop/remotedesktopviewer.h \
    processmonitor/processmonitor.h \
    processmonitor/processmonitorrulemodel.h \
    processmonitor/ruleinfowidget.h \
    servermanagement/serveraddressmanagerwindow.h \
    servermanagement/serverinfomodel.h \
    adminuser.h \
    servermanagement/adminsmanagementwidget.h \
    servermanagement/admininfowidget.h \
    servermanagement/alarmsmanagementwidget.h \
    servermanagement/alarminfomodel.h \
    servermanagement/servermanagementwidget.h \
    announcementmanagement/announcementinfowidget.h \
    announcementmanagement/announcementmanagementwidget.h \
    announcementmanagement/announcementinfomodel.h \
    filemanagement/filemanagementwidget.h

SOURCES += \
    networkmanager/controlcenterpacketsparser.cpp \
    taskmanagement/taskinfoviewwidget.cpp \
    taskmanagement/taskmanagement.cpp \
    systemmanagement/systemmanagementwidget.cpp \
    controlcenter.cpp \
    controlcenterplugin.cpp \
    networkmanager/resourcesmanagerinstance.cpp \
    servicesmanagement/servicesmanagement.cpp \
    servicesmanagement/serviceinfomodel.cpp \
    clientmanagement/clientinfomodel.cpp \
    systemmanagement/shutdowndialog.cpp \
    usersmanagement/winuserinfomodel.cpp \
    usersmanagement/winuserinfowidget.cpp \
    usersmanagement/winusermanagerwidget.cpp \
    usersmanagement/winuserinfo.cpp \
    softwaremanagement/softwaremamagement.cpp \
    remotedesktop/remotedesktopmonitor.cpp \
    remotedesktop/remotedesktopviewer.cpp \
    processmonitor/processmonitor.cpp \
    processmonitor/processmonitorrulemodel.cpp \
    processmonitor/ruleinfowidget.cpp \
    servermanagement/serveraddressmanagerwindow.cpp \
    servermanagement/serverinfomodel.cpp \
    adminuser.cpp \
    servermanagement/adminsmanagementwidget.cpp \
    servermanagement/admininfowidget.cpp \
    servermanagement/alarmsmanagementwidget.cpp \
    servermanagement/alarminfomodel.cpp \
    servermanagement/servermanagementwidget.cpp \
    announcementmanagement/announcementinfowidget.cpp \
    announcementmanagement/announcementmanagementwidget.cpp \
    announcementmanagement/announcementinfomodel.cpp \
    filemanagement/filemanagementwidget.cpp


FORMS += taskmanagement/taskinfoviewwidget.ui \
    taskmanagement/taskmanagement.ui \
    systemmanagement/systemmanagementwidget.ui \
    controlcenter.ui \
    filemanagement/filemanagement.ui \
    servicesmanagement/servicesmanagement.ui \
    systemmanagement/shutdowndialog.ui \
    usersmanagement/winuserinfowidget.ui \
    usersmanagement/winusermanagerwidget.ui \
    softwaremanagement/softwaremamagement.ui \
    remotedesktop/remotedesktopmonitor.ui \
    processmonitor/processmonitor.ui \
    processmonitor/ruleinfowidget.ui \
    servermanagement/serveraddressmanagerwindow.ui \
    servermanagement/adminsmanagementwidget.ui \
    servermanagement/admininfowidget.ui \
    servermanagement/alarmsmanagementwidget.ui \
    servermanagement/servermanagementwidget.ui \
    announcementmanagement/announcementinfowidget.ui \
    announcementmanagement/announcementmanagementwidget.ui


win32 {
HEADERS +=     remotedesktop/avifile.h
SOURCES +=     remotedesktop/AviFile.cpp
LIBS += -lvfw32 -lgdiplus -lGdi32
}


RESOURCES += controlcenter.qrc



# define some usefull values
QMAKE_TARGET_PRODUCT = "Control Center"
QMAKE_TARGET_DESCRIPTION = "Control Center"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
