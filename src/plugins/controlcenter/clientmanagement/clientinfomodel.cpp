/*
 ****************************************************************************
 * clientinfomodel.cpp
 *
 * Created On: 2010-5-24
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
 * Last Modified On: 2010-5-24
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#include "clientinfomodel.h"

#include <QDebug>



namespace HEHUI
{

ClientInfoModel::ClientInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ClientInfoModel::~ClientInfoModel()
{
    clear();
}

void ClientInfoModel::setClientList(QList<ClientInfo *> &clientsList)
{

    clear();

    beginResetModel();

    this->clientsList = clientsList;

    endResetModel();
}

void ClientInfoModel::addClientInfo(ClientInfo *clientInfo)
{

    if(!clientsList.contains(clientInfo)) {
        beginResetModel();
        this->clientsList.append(clientInfo);
        endResetModel();
    }

}

void ClientInfoModel::updateClientInfo(ClientInfo *clientInfo)
{
    if(!clientsList.contains(clientInfo)) {
        return;
    }

    beginResetModel();

    endResetModel();
}

ClientInfo *ClientInfoModel::getClientInfo(const QModelIndex &index)
{

    if(!index.isValid()) {
        return 0;
    }

    return clientsList.at(index.data(Qt::UserRole).toInt());

}

ClientInfo *ClientInfoModel::getClientInfo(const QString &assetNO)
{

    foreach (ClientInfo *info, clientsList) {
        if(info->getAssetNO() == assetNO) {
            return info;
        }
    }

    return 0;
}

void ClientInfoModel::clear()
{
    beginResetModel();

    foreach (ClientInfo *info, clientsList) {
        delete info;
        info = 0;
    }

    clientsList.clear();

    endResetModel();
}

int ClientInfoModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return clientsList.size();

}

int	 ClientInfoModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 14;
}

QVariant ClientInfoModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= clientsList.size())) {
        return QVariant();
    }

    ClientInfo *info = static_cast<ClientInfo *> (clientsList.at(row));
    Q_ASSERT(info);

    if(role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return info->getAssetNO();
            break;

        case 1:
            return info->getComputerName();
            break;

        case 2:
            return info->getOSVersion();
            break;

        case 3:
            return info->getInstallationDate();
            break;

        case 4:
            return info->getOsKey();
            break;

        case 5:
            return info->getWorkgroup();
            break;

        case 6:
            return info->isJoinedToDomain() ? tr("Yes") : tr("No");
            break;

        case 7:
            return info->getUsers();
            break;

        case 8:
            return info->getAdministrators();
            break;

        case 9:
            return info->getIP();
            break;

        case 10:
            return info->getClientVersion();
            break;

        case 11:
            return info->isProcessMonitorEnabled() ? tr("Enabled") : tr("Disabled");
            break;

        case 12:
            return getUsbSDStatusString(quint8(info->getUsbSDStatus()));
            break;

        case 13:
            return info->getLastOnlineTime().toString("yyyy.MM.dd hh:mm:ss");
            break;

        case 14:
            return info->getExternalIPInfo();
            break;

        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::UserRole) {
        return row;
    }

    if(role == Qt::EditRole) {
        if(index.column() == 11) {
            return info->isProcessMonitorEnabled() ? "1" : "0";
        }

        if(index.column() == 12) {
            return QString::number(quint8(info->getUsbSDStatus()));
        }

        return index.data(Qt::DisplayRole);
    }


    return QVariant();

}

QVariant ClientInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("Asset NO."));
            break;

        case 1:
            return QString(tr("Computer Name"));
            break;

        case 2:
            return QString(tr("OS Version"));
            break;

        case 3:
            return QString(tr("Installation Date"));
            break;

        case 4:
            return QString(tr("Product Key"));
            break;

        case 5:
            return QString(tr("Workgroup"));
            break;
        case 6:
            return QString(tr("Joined To Domain"));
            break;

        case 7:
            return QString(tr("Users"));
            break;

        case 8:
            return QString(tr("Administrators"));
            break;

        case 9:
            return QString(tr("IP"));
            break;

        case 10:
            return QString(tr("ClientVersion"));
            break;

        case 11:
            return QString(tr("Process Monitor"));
            break;

        case 12:
            return QString(tr("USB"));
            break;

        case 13:
            return QString(tr("Last Online Time"));
            break;

        case 14:
            return QString(tr("Last Online IP"));
            break;

        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}

QString ClientInfoModel::getUsbSDStatusString(quint8 status) const
{

    switch (status) {
    case quint8(MS::USBSTOR_ReadWrite):
        return tr("Read-Write");
        break;

    case quint8(MS::USBSTOR_ReadOnly):
        return tr("Read-Only");
        break;

    case quint8(MS::USBSTOR_Disabled):
        return tr("Disabled");
        break;

    default:
        break;
    }

    return tr("Unknown");

}



////////////////////////////////////////////////////////////

ClientInfoSortFilterProxyModel::ClientInfoSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

    assetNO = QRegExp(".*", Qt::CaseInsensitive);
    computerName = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    procMon = QRegExp(".*", Qt::CaseInsensitive);
}

void ClientInfoSortFilterProxyModel::cleanFilters()
{

    assetNO = QRegExp(".*", Qt::CaseInsensitive);
    computerName = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    procMon = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void ClientInfoSortFilterProxyModel::setFilters(const QRegExp &assetNO, const QRegExp &computerName, const QRegExp &os, const QRegExp &workgroup, const QRegExp &userName, const QRegExp &ip, const QRegExp &usbSD, const QRegExp &procMon)
{

    this->assetNO = assetNO;
    this->computerName = computerName;
    this->os = os;
    this->workgroup = workgroup;
    this->userName = userName;
    this->ip = ip;
    this->usbSD = usbSD;
    this->procMon = procMon;

    invalidateFilter();

}

bool ClientInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex index3 = sourceModel()->index(sourceRow, 5, sourceParent);
    QModelIndex index4 = sourceModel()->index(sourceRow, 7, sourceParent);
    QModelIndex index5 = sourceModel()->index(sourceRow, 9, sourceParent);
    QModelIndex indexProcMon = sourceModel()->index(sourceRow, 11, sourceParent);
    QModelIndex indexUSB = sourceModel()->index(sourceRow, 12, sourceParent);


    return (index0.data().toString().contains(assetNO)
            && index1.data().toString().contains(computerName)
            && index2.data().toString().contains(os)
            && index3.data().toString().contains(workgroup)
            && index4.data().toString().contains(userName)
            && index5.data().toString().contains(ip)
            && indexProcMon.data(Qt::EditRole).toString().contains(procMon)
            && indexUSB.data(Qt::EditRole).toString().contains(procMon)
           );

}








} //namespace HEHUI
