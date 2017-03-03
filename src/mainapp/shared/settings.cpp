#include <QDir>
#include <QFile>

#include "settings.h"
#include <QDebug>

namespace HEHUI
{

Settings::Settings(const QString fileBaseName, const QString fileDirPath, QObject *parent )
    : SettingsBase(fileBaseName, fileDirPath, parent )
{

}

Settings::~Settings()
{

}









} //namespace HEHUI

