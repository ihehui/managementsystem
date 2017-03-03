#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>


#include "app_constants.h"

//#include "../../shared/gui/settingsbase.h"
//#include "singleton.h"

#include "HHSharedCore/hsingleton.h"
#include "HHSharedGUI/hsettingsbase.h"



namespace HEHUI
{

class Settings : public SettingsBase, public Singleton<Settings>
{
    Q_OBJECT
    friend class Singleton<Settings>;

public:
    Settings(const QString fileBaseName = QString(APP_NAME).remove(" "), const QString fileDirPath = QCoreApplication::applicationDirPath(), QObject *parent = 0 );
    ~Settings();



private:



};

} //namespace HEHUI

#endif // SETTINGS_H
