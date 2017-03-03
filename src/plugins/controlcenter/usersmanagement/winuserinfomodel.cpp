#include "winuserinfomodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace HEHUI
{

WinUserInfoModel::WinUserInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

WinUserInfoModel::~WinUserInfoModel()
{
    clearUsers();
}

void WinUserInfoModel::setJsonData(const QByteArray &data)
{
    qDebug() << "--WinUserInfoModel::setJsonData(...)";

    clearUsers();

    if(data.isEmpty()) {
        qCritical() << "ERROR! Empty user info data.";
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()) {
        return;
    }

    beginResetModel();

    QJsonArray array = object["Users"].toArray();

    for(int i = 0; i < array.size(); i++) {
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 15) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }

        WinUserInfo *info = new WinUserInfo();
        int index = 0;
        info->userName = infoArray.at(index++).toString();
        info->loggedon = infoArray.at(index++).toString().toUInt();

        info->homeDir = infoArray.at(index++).toString();
        info->comment = infoArray.at(index++).toString();

        info->accountDisabled = infoArray.at(index++).toString().toUInt();
        info->cannotChangePassword = infoArray.at(index++).toString().toUInt();
        info->accountLocked = infoArray.at(index++).toString().toUInt();
        info->passwordNeverExpires = infoArray.at(index++).toString().toUInt();

        info->fullName = infoArray.at(index++).toString();

        info->lastLogonTime_t = infoArray.at(index++).toString().toDouble();
        info->lastLogoffTime_t = infoArray.at(index++).toString().toDouble();

        info->sid = infoArray.at(index++).toString();
        info->profile = infoArray.at(index++).toString();

        info->mustChangePassword = infoArray.at(index++).toString().toUInt();

        info->groups = infoArray.at(index++).toString();

        usersList.append(info);
    }

    endResetModel();
}

WinUserInfo *WinUserInfoModel::getUser(const QModelIndex &index)
{
    if(!index.isValid()) {
        return 0;
    }

    return usersList.at(index.data(Qt::UserRole).toInt());
}

int WinUserInfoModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return usersList.size();

}

int	 WinUserInfoModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 6;

}

QVariant WinUserInfoModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= usersList.size())) {
        return QVariant();
    }

    WinUserInfo *info = static_cast<WinUserInfo *> (usersList.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return info->userName;
            break;

        case 1:
            return info->fullName;
            break;

        case 2:
            return info->accountDisabled ? tr("Disabled") : tr("Active");
            break;

        case 3:
            return info->loggedon ? tr("Yes") : tr("No");
            break;

        case 4:
            return info->comment;
            break;

        case 5:
            return info->groups;
            break;

        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::UserRole) {
        return row;
    }

    return QVariant();

}

QVariant WinUserInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("User Name"));
            break;
        case 1:
            return QString(tr("Full Name"));
            break;

        case 2:
            return QString(tr("Status"));
            break;

        case 3:
            return QString(tr("Logged On"));
            break;

        case 4:
            return QString(tr("Comment"));
            break;

        case 5:
            return QString(tr("Local Groups"));
            break;

        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}


void WinUserInfoModel::clearUsers()
{

    if(usersList.isEmpty()) {
        return;
    }

    beginResetModel();


    foreach (WinUserInfo *user, usersList) {
        delete user;
        user = 0;
    }
    usersList.clear();

    endResetModel();

}



////////////////////////////////////////////////////////////

WinUserInfoSortFilterProxyModel::WinUserInfoSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

//    computerName = QRegExp(".*", Qt::CaseInsensitive);
//    userName = QRegExp(".*", Qt::CaseInsensitive);
//    workgroup = QRegExp(".*", Qt::CaseInsensitive);
//    usbSD = QRegExp(".*", Qt::CaseInsensitive);
//    mac = QRegExp(".*", Qt::CaseInsensitive);
//    ip = QRegExp(".*", Qt::CaseInsensitive);
//    os = QRegExp(".*", Qt::CaseInsensitive);

}

//void ADUserInfoSortFilterProxyModel::cleanFilters(){

//    computerName = QRegExp(".*", Qt::CaseInsensitive);
//    userName = QRegExp(".*", Qt::CaseInsensitive);
//    workgroup = QRegExp(".*", Qt::CaseInsensitive);
//    usbSD = QRegExp(".*", Qt::CaseInsensitive);
//    mac = QRegExp(".*", Qt::CaseInsensitive);
//    ip = QRegExp(".*", Qt::CaseInsensitive);
//    os = QRegExp(".*", Qt::CaseInsensitive);

//    invalidateFilter();
//}

//void ADUserInfoSortFilterProxyModel::setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs){

//    this->computerName = computerName;
//    this->userName = userName;
//    this->workgroup = workgroup;
//    this->usbSD = usbSD;
//    this->mac = mac;
//    this->ip = ip;
//    this->os = os;
//    this->programs = programs;

//    invalidateFilter();

//}

bool WinUserInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
//    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
//    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
//    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
//    QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
//    QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);
//    QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent);
//    QModelIndex index6 = sourceModel()->index(sourceRow, 6, sourceParent);


//    return (index0.data().toString().contains(computerName)
//            && index1.data().toString().contains(workgroup)
//            && ( index2.data().toString().contains(ip) && index2.data().toString().contains(mac) )
//            && index3.data().toString().contains(userName)
//            && index4.data().toString().contains(os)
//            && index5.data().toString().contains(usbSD)
//            && index6.data().toString().contains(programs)

//            );

    return true;

}








} //namespace HEHUI
