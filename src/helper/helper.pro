TARGET = helper
TEMPLATE = app
QT += core \
    gui \
    widgets \
    sql \
    network \
    concurrent


# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedGUI \
    HHSharedNetwork \
    HHSharedService \
#    HHSharedUDT \
    HHSharedENET \
    HHSharedSysUtilities
#win32:HHSharedLibs += HHSharedSysUtilities
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../sharedms/sharedms.pri)


# ##
UI_DIR = ./ui
INCLUDEPATH += $$UI_DIR \
    $$PWD



# Input
HEADERS += \
    networkmanager/bulletinboardpacketsparser.h \
    remoteassistance/remoteassistance.h \
    updatepassword/updatepasswordwidget.h \
    bulletinboard/bulletinboardwidget.h \
    systemsummaryinfo.h \
    networkmanager/resourcesmanagerinstance.h \
    helper.h
FORMS += remoteassistance/remoteassistance.ui \
    updatepassword/updatepasswordwidget.ui \
    bulletinboard/bulletinboardwidget.ui \
    systemsummaryinfo.ui
SOURCES += \
    networkmanager/bulletinboardpacketsparser.cpp \
    remoteassistance/remoteassistance.cpp \
    updatepassword/updatepasswordwidget.cpp \
    bulletinboard/bulletinboardwidget.cpp \
    systemsummaryinfo.cpp \
    networkmanager/resourcesmanagerinstance.cpp \
    main.cpp \
    helper.cpp
RESOURCES += \
    resources.qrc

# win32:RC_FILE = iconresource.rc
#win32:APP_ICON = resources/images/app.ico
win32:RC_ICONS = resources/images/app.ico
mac:ICON = ./resources/images/app.icns

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Helper"
QMAKE_TARGET_DESCRIPTION = "Helper Application For Management System"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
