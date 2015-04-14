#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QCoreApplication>

#include "global_shared.h"
#include "sharedmslib.h"

#include "HHSharedCore/hsettingscore.h"
#include "HHSharedCore/hcryptography.h"
#include "HHSharedCore/hglobal_core.h"



namespace HEHUI{

class SHAREDMSLIB_API Settings : public SettingsCore
{
	Q_OBJECT

public:
    Settings(const QString fileBaseName = "settings", const QString fileDirPath = QCoreApplication::applicationDirPath(), QObject *parent = 0 );
    Settings(const QString fileName, Format format, QObject* parent= 0 );

    ~Settings();

    void setEncryptionKey(const QByteArray &encryptionKey);


    void setDBType(unsigned int databaseType);
    DatabaseType getDBType() const;

    void setDBDriver(const QString &driverName);
    QString getDBDriver() const;

    void setDBServerHost(const QString &host);
    QString getDBServerHost() const;

    void setDBServerPort(quint16 port);
    quint16 getDBServerPort();

    void setDBServerUserName(const QString &userName);
    QString getDBServerUserName() const;

    void setDBServerUserPassword(const QString &userPassword);
    QString getDBServerUserPassword() const;
    void setDBName(const QString &databaseName);
    QString getDBName() const;

    void setAppServerIP(const QString &serverIP);
    QString getAppServerIP();
    void setAppServerPort(quint16 port);
    quint16 getAppServerPort();

private:


private:
    QByteArray m_encryptionKey;



};

} //namespace HEHUI

#endif // SETTINGS_H
