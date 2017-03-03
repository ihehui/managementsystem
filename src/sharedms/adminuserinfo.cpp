#include "adminuserinfo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


namespace HEHUI
{


AdminUserInfo::AdminUserInfo(const QString &adminUserID, QObject *parent)
    : UserBase(adminUserID, "", "", parent)
{

    businessAddress = "";
    lastLoginIP = "";
    lastLoginPC = "";
    lastLoginTime = "";
    readonly = true;
    active = true;
    remark = "";

    socketID = 0;

}

AdminUserInfo::~AdminUserInfo()
{

}

void AdminUserInfo::setJsonData(const QByteArray &data)
{

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    if(object.contains("ID")) {
        setUserID( object["ID"].toString() );
    }

    if(object.contains("Name")) {
        setUserName( object["Name"].toString() );
    }

    if(object.contains("PWD")) {
        setPassword( object["PWD"].toString() );
    }

    if(object.contains("BAddress")) {
        businessAddress = object["BAddress"].toString();
    }

    if(object.contains("IP")) {
        lastLoginIP = object["IP"].toString();
    }

    if(object.contains("PC")) {
        lastLoginPC = object["PC"].toString();
    }

    if(object.contains("Time")) {
        lastLoginTime = object["Time"].toString();
    }

    if(object.contains("Readonly")) {
        readonly = object["Readonly"].toString().toUInt();
    }

    if(object.contains("Active")) {
        active = object["Active"].toString().toUInt();
    }

    if(object.contains("Remark")) {
        remark = object["Remark"].toString();
    }


}

QByteArray AdminUserInfo::getJsonData(bool withPassword)
{

    QJsonObject object;

    object["ID"] = getUserID();
    object["Name"] = getUserName();
    if(withPassword) {
        object["PWD"] = getPassword();
    }

    object["BAddress"] = businessAddress;
    object["IP"] = lastLoginIP;
    object["PC"] = lastLoginPC;
    object["Time"] = lastLoginTime;
    object["Readonly"] = readonly ? "1" : "0";
    object["Active"] = active ? "1" : "0";
    object["Remark"] = remark;

    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Compact);

}





} //namespace HEHUI
