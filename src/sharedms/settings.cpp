
#include "settings.h"



namespace HEHUI {

Settings::Settings(const QString fileBaseName, const QString fileDirPath, QObject* parent )
    : SettingsCore(fileBaseName, fileDirPath, parent )
{
    m_encryptionKey = QString(CRYPTOGRAPHY_KEY).toUtf8();
}

Settings::Settings(const QString fileName, Format format, QObject* parent )
    : SettingsCore(fileName, format, parent )
{
    m_encryptionKey = QString(CRYPTOGRAPHY_KEY).toUtf8();
}

Settings::~Settings()
{


}

void Settings::setEncryptionKey(const QByteArray &encryptionKey){
    m_encryptionKey = encryptionKey;
}

QByteArray Settings::getEncryptionKey() const{
    return m_encryptionKey;
}

void Settings::setDBType(unsigned int databaseType){
    setValue("Database/Type", databaseType);
}
DatabaseType Settings::getDBType() const{
    return DatabaseType(value("Database/Type", HEHUI::MYSQL).toUInt());
}

void Settings::setDBDriver(const QString &driverName)
{
    setValue("Database/Driver", driverName);
}
QString Settings::getDBDriver() const{
    return value("Database/Driver", "QMYSQL").toString();
}

void Settings::setDBServerHost(const QString &host){
    setValueWithEncryption("Database/Host", host, m_encryptionKey);
    //setValue("Database/Host", host);
}
QString Settings::getDBServerHost() const{
    return getValueWithDecryption("Database/Host", m_encryptionKey, "").toString();

//    return value("Database/Host", REMOTE_SITOY_COMPUTERS_DB_SERVER_HOST).toString();
}

void Settings::setDBServerPort(quint16 port){
    setValueWithEncryption("Database/Port", port, m_encryptionKey);
    //setValue("Database/Port", port);
}
quint16 Settings::getDBServerPort(){
    return getValueWithDecryption("Database/Port", m_encryptionKey, 0).toUInt();

//    return value("Database/Port", REMOTE_SITOY_COMPUTERS_DB_SERVER_PORT).toUInt();
}

void Settings::setDBServerUserName(const QString &userName){
    setValueWithEncryption("Database/UserName", userName, m_encryptionKey);

//    QByteArray *destination = new QByteArray();
//    Cryptography cryptography;
//    cryptography.teaCrypto(destination, userName.toUtf8(), m_encryptionKey, true);
//    setValue("Database/UserName", *destination);
//    delete destination;
}
QString Settings::getDBServerUserName() const{
    return getValueWithDecryption("Database/UserName", m_encryptionKey, "").toString();

//    QString userName = "";
//    QByteArray userNameArray = value("Database/UserName").toByteArray();
//    if(userNameArray.isEmpty()){
//        userName = REMOTE_SITOY_COMPUTERS_DB_USER_NAME;
//    }else{
//        QByteArray *destination = new QByteArray();
//        Cryptography cryptography;
//        cryptography.teaCrypto(destination, userNameArray, m_encryptionKey, false);
//        userName = QString(*destination);
//        delete destination;

//    }
//    return userName;
}

void Settings::setDBServerUserPassword(const QString &userPassword){
    setValueWithEncryption("Database/Password", userPassword, m_encryptionKey);
}
QString Settings::getDBServerUserPassword() const{
    return getValueWithDecryption("Database/Password", m_encryptionKey, "").toString();
}

void Settings::setDBName(const QString &databaseName){
    setValueWithEncryption("Database/DatabaseName", databaseName, m_encryptionKey);

    //setValue("Database/DatabaseName", databaseName);
}

QString Settings::getDBName() const{
    return getValueWithDecryption("Database/DatabaseName", m_encryptionKey, "").toString();

    //return value("Database/DatabaseName", REMOTE_SITOY_COMPUTERS_DB_NAME).toString();
}

void Settings::setAppServers(const QString &serversList){
    setValueWithEncryption("AppServers", serversList, m_encryptionKey);
}

QString Settings::getAppServers() const{
    return getValueWithDecryption("AppServers", m_encryptionKey, "").toString();
}

void Settings::setLastUsedAppServer(const QString &server){
    setValueWithEncryption("AppServerLastUsed", server, m_encryptionKey);
}

QString Settings::getLastUsedAppServer() const{
    return getValueWithDecryption("AppServerLastUsed", m_encryptionKey, QString()).toString();
}












} //namespace HEHUI

