/*
 ****************************************************************************
 * serviceinfomodel.cpp
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


#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "serviceinfomodel.h"



namespace HEHUI {


ServiceInfoModel::ServiceInfoModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ServiceInfoModel::~ServiceInfoModel() {
    qDebug()<<"--ServiceInfoModel::~ServiceInfoModel()";

    clearServicesList();

}

void ServiceInfoModel::setJsonData(const QByteArray &data){
    clearServicesList();

    if(data.isEmpty()){return;}

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    if(object.isEmpty()){return;}

    beginResetModel();

    QJsonArray array = object["Service"].toArray();

    for(int i=0;i<array.size();i++){
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 9){continue;}

        ServiceInfo *info = new ServiceInfo();
        info->serviceName = infoArray.at(0).toString();
        info->displayName = infoArray.at(1).toString();
//        qDebug()<<info->displayName;
        info->processID = infoArray.at(2).toString().toUInt();
        info->description = infoArray.at(3).toString();
        info->startType = infoArray.at(4).toString().toUInt();
        info->account = infoArray.at(5).toString();
        info->dependencies = infoArray.at(6).toString();
        info->binaryPath = infoArray.at(7).toString();
        info->serviceType = infoArray.at(8).toString().toUInt();

        servicesList.append(info);
    }

    endResetModel();
}

ServiceInfo * ServiceInfoModel::getServiceInfo(const QString &serviceName){
    ServiceInfo *sinfo = 0;
    foreach (ServiceInfo *info, servicesList) {
        if(info->serviceName == serviceName){
            sinfo = info;
            break;
        }
    }
    if(!sinfo){
        qCritical()<<QString("ERROR! Service '%1' Not Found!").arg(serviceName);
    }

    return sinfo;
}

bool ServiceInfoModel::updateServiceInfo(const QString &serviceName, quint64 processID, quint64 startupType){

    ServiceInfo *sinfo = getServiceInfo(serviceName);
    if(!sinfo){
        qCritical()<<QString("ERROR! Service '%1' Not Found!").arg(serviceName);
        return false;
    }

    beginResetModel();

    sinfo->processID = processID;
    sinfo->startType = startupType;

    endResetModel();

    return true;

}

int ServiceInfoModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return servicesList.size();

}

int	 ServiceInfoModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }
    return 8;
}

QVariant ServiceInfoModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= servicesList.size())){
        return QVariant();
    }

    ServiceInfo *info = static_cast<ServiceInfo *> (servicesList.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        switch (index.column()) {
        case 0:
            return info->serviceName;
            break;
        case 1:
            return info->displayName;
            break;
        case 2:
            return QString::number(info->processID);
            break;
        case 3:
            return info->description;
            break;
        case 4:
        {
            return getStartTypeString(info->startType);
        }
            break;
        case 5:
            return info->account;
            break;
        case 6:
            return info->dependencies;
            break;
        case 7:
            return info->binaryPath;
            break;
        case 8:
            return getServiceTypeString(info->serviceType);
            break;


        default:
            return QVariant();
            break;
        }
    }
    if(role == Qt::UserRole){
        return info->serviceName;
    }

    return QVariant();

}

QVariant ServiceInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal){
        switch (section) {
        case 0:
            return QString(tr("Service Name"));
            break;
        case 1:
            return QString(tr("Display Name"));
            break;
        case 2:
            return QString(tr("Process ID"));
            break;
        case 3:
            return QString(tr("Description"));
            break;
        case 4:
            return QString(tr("Startup Type"));
            break;
        case 5:
            return QString(tr("Account"));
            break;
        case 6:
            return QString(tr("Dependencies"));
            break;
        case 7:
            return QString(tr("Binary Path"));
            break;
        case 8:
            return QString(tr("Service Type"));
            break;

        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}

QString ServiceInfoModel::getStartTypeString(unsigned long startType) const{

    QString str;
    switch (startType) {
    case 0x00000000:
        str = tr("BOOT");
        break;
    case 0x00000001:
        str = tr("SYSTEM");
        break;
    case 0x00000002:
        str = tr("AUTO");
        break;
    case 0x00000003:
        str = tr("MANUAL");
        break;
    case 0x00000004:
        str = tr("DISABLED");
        break;
    default:
        str = tr("Unknown");
        break;
    }

    return str;
}

QString ServiceInfoModel::getServiceTypeString(unsigned long serviceType) const{

    QString str;
    switch (serviceType) {
    case 0x00000001:
    case 0x00000002:
    case 0x00000008:
        str = tr("Driver Service");
        break;
    case 0x00000010:
    case 0x00000020:
        str = tr("WIN32 Service");
        break;
    case 0x00000100:
        str = tr("Interactive Service");
        break;
    default:
        str = tr("Service");
        break;
    }

    return str;

}

void ServiceInfoModel::clearServicesList(){

    beginResetModel();

    foreach (ServiceInfo *info, servicesList) {
        delete info;
        info = 0;
    }

    servicesList.clear();

    endResetModel();

}


////////////////////////////////////////////////////////////

ServiceInfoSortFilterProxyModel::ServiceInfoSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

    serviceName = QRegExp(".*", Qt::CaseInsensitive);
    serviceDisplayName = QRegExp(".*", Qt::CaseInsensitive);
    serviceType = QRegExp(".*", Qt::CaseInsensitive);

}

void ServiceInfoSortFilterProxyModel::cleanFilters(){

    serviceName = QRegExp(".*", Qt::CaseInsensitive);
    serviceDisplayName = QRegExp(".*", Qt::CaseInsensitive);
    serviceType = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void ServiceInfoSortFilterProxyModel::setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup){

    this->serviceName = computerName;
    this->serviceDisplayName = userName;
    this->serviceType = workgroup;

    invalidateFilter();

}

bool ServiceInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    //QModelIndex index8 = sourceModel()->index(sourceRow, 8, sourceParent);

    return (index0.data().toString().contains(serviceName)
            && index1.data().toString().contains(serviceDisplayName)
            //&& index8.data().toString().contains(serviceType)

            );

}








} //namespace HEHUI
