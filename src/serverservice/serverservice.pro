TARGET = serverservice
TEMPLATE = app
CONFIG += qt \
    console
QT += core \
    sql \
    network \
    concurrent
QT -= gui

# include config file
include( ../config.pri )
HHSharedLibs += HHSharedCore \
    HHSharedNetwork \
    HHSharedService \
    HHSharedENET \
#    HHSharedUDT \
    HHSharedSysUtilities
include(../../HHSharedLibs.pri)
HHSharedLibs += HHSharedMS
include(../sharedms/sharedms.pri)

# ###
DEPENDPATH += 
INCLUDEPATH += 
SOURCES += \
    packetmanager/serverpacketsparser.cpp \
    main.cpp \
    serverservice/serverservice.cpp \
    resourcesmanagerinstance.cpp
HEADERS += \
    app_constants.h \
    packetmanager/serverpacketsparser.h \
    serverservice/serverservice.h \
    resourcesmanagerinstance.h

# ##
# unix:target.path += ../bin
# win32:target.path += %windir%/system32
# INSTALLS += target
RESOURCES += resources.qrc

# win32:RC_FILE = iconresource.rc
#win32:APP_ICON = resources/images/app.ico
win32:RC_ICONS = resources/images/app.ico
mac:ICON = ./resources/images/app.icns

# define some usefull values
# QMAKE_TARGET_COMPANY	= "He Hui Team"
QMAKE_TARGET_PRODUCT = "Server Service Application For Management System"
QMAKE_TARGET_DESCRIPTION = "Server Service Application"
DEFINES *= "APP_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\""
