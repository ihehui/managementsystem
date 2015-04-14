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

namespace HEHUI {

ClientInfoModel::ClientInfoModel(QObject *parent)
	:QAbstractTableModel(parent)
{
	// TODO Auto-generated constructor stub

}

ClientInfoModel::~ClientInfoModel() {
    clear();
}

void ClientInfoModel::setClientList(QList<ClientInfo *> &clientsList)
{

    clear();

    beginResetModel();

    this->clientsList = clientsList;

    endResetModel();
}

void ClientInfoModel::addClientInfo(ClientInfo *clientInfo){
    
    beginResetModel();

    if(!clientsList.contains(clientInfo)){
        this->clientsList.append(clientInfo);
    }
    
    endResetModel();
    
}

ClientInfo* ClientInfoModel::getClientInfo(const QModelIndex & index){

    if(!index.isValid()){
        return 0;
    }

    return clientsList.at(index.data(Qt::UserRole).toInt());

}

ClientInfo * ClientInfoModel::getClientInfo(const QString &assetNO){

    foreach (ClientInfo *info, clientsList) {
        if(info->getAssetNO() == assetNO){
            return info;
        }
    }

    return 0;
}

void ClientInfoModel::clear(){
    beginResetModel();

    foreach (ClientInfo *info, clientsList) {
        delete info;
        info = 0;
    }

    clientsList.clear();

    endResetModel();
}

int ClientInfoModel::rowCount ( const QModelIndex & parent) const {
	if(parent.isValid()){
		return 0;
	}
	return clientsList.size();

}

int	 ClientInfoModel::columnCount ( const QModelIndex & parent) const{
	if(parent.isValid()){
		return 0;
	}

    return 13;


}

QVariant ClientInfoModel::data ( const QModelIndex & index, int role) const{
	if(!index.isValid()){
		return QVariant();
	}

	int row = index.row();
	if((row < 0) || (row >= clientsList.size())){
		return QVariant();
	}

	if(role == Qt::DisplayRole || role == Qt::EditRole){
            ClientInfo *info = static_cast<ClientInfo *> (clientsList.at(row));
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
                return info->isJoinedToDomain()?tr("Yes"):tr("No");
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
                return info->isProcessMonitorEnabled()?tr("Enabled"):tr("Disabled");
                break;
            case 12:
                return info->getLastOnlineTime().toString("yyyy.MM.dd hh:mm:ss");
                break;

            default:
                return QVariant();
                break;
            }
	}

    if(role == Qt::UserRole){
        return row;
    }


	return QVariant();

}

QVariant ClientInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole){
		return QVariant();
	}

	if(orientation ==  Qt::Horizontal){
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
                return QString(tr("Last Online Time"));
                break;
                
            default:
                return QVariant();
                break;
            }
            
	}

	return QVariant();

}

////////////////////////////////////////////////////////////

ClientInfoSortFilterProxyModel::ClientInfoSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

    computerName = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    mac = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);

}

void ClientInfoSortFilterProxyModel::cleanFilters(){

    computerName = QRegExp(".*", Qt::CaseInsensitive);
    userName = QRegExp(".*", Qt::CaseInsensitive);
    workgroup = QRegExp(".*", Qt::CaseInsensitive);
    usbSD = QRegExp(".*", Qt::CaseInsensitive);
    mac = QRegExp(".*", Qt::CaseInsensitive);
    ip = QRegExp(".*", Qt::CaseInsensitive);
    os = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void ClientInfoSortFilterProxyModel::setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs){

    this->computerName = computerName;
    this->userName = userName;
    this->workgroup = workgroup;
    this->usbSD = usbSD;
    this->mac = mac;
    this->ip = ip;
    this->os = os;
    this->programs = programs;

    invalidateFilter();

}

bool ClientInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
    QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);
    QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent);
    QModelIndex index6 = sourceModel()->index(sourceRow, 6, sourceParent);


//    return (sourceModel()->data(index0).toString().contains(computerName)
//            && sourceModel()->data(index1).toString().contains(workgroup)
//            && ( sourceModel()->data(index2).toString().contains(ip) && sourceModel()->data(index2).toString().contains(mac) )
//            && sourceModel()->data(index3).toString().contains(userName)
//            && sourceModel()->data(index4).toString().contains(os)
//            && sourceModel()->data(index5).toString().contains(usbSD)
//            && sourceModel()->data(index6).toString().contains(programs)

//            );

    return (index0.data().toString().contains(computerName)
            && index1.data().toString().contains(workgroup)
            && ( index2.data().toString().contains(ip) && index2.data().toString().contains(mac) )
            && index3.data().toString().contains(userName)
            && index4.data().toString().contains(os)
            && index5.data().toString().contains(usbSD)
            && index6.data().toString().contains(programs)

            );

}








} //namespace HEHUI
