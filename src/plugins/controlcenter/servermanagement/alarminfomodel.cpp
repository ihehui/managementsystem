#include "alarminfomodel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "../../sharedms/global_shared.h"

namespace HEHUI {


AlarmInfoModel::AlarmInfoModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

AlarmInfoModel::~AlarmInfoModel()
{
    clear();
}

void AlarmInfoModel::setJsonData(const QByteArray &jsonData)
{

    clear();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError){
        qCritical()<<error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    QJsonArray jsonArray = object["Alarms"].toArray();

    beginResetModel();

    for(int i=0;i<jsonArray.size(); i++){
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 8){
            qCritical()<<"ERROR! Invalid JSON array.";
            continue;
        }

        AlarmInfo *info = new AlarmInfo();
        int index = 0;
        info->ID = infoArray.at(index++).toString();
        info->AssetNO = infoArray.at(index++).toString();
        info->AlarmType = infoArray.at(index++).toString().toUShort();
        info->Message = infoArray.at(index++).toString();
        info->UpdateTime = infoArray.at(index++).toString();
        info->Acknowledged = infoArray.at(index++).toString().toUShort();
        info->AcknowledgedBy = infoArray.at(index++).toString();
        info->AcknowledgedTime = infoArray.at(index++).toString();

        infolist.append(info);

    }

    endResetModel();
}

void AlarmInfoModel::clear(){
    beginResetModel();

    foreach (AlarmInfo *info, infolist) {
        delete info;
        info = 0;
    }
    infolist.clear();

    endResetModel();
}

AlarmInfo * AlarmInfoModel::getAlarmInfo(const QModelIndex &index){
    if(!index.isValid()){
        return 0;
    }

    return infolist.at(index.data(Qt::UserRole).toInt());
}

QString AlarmInfoModel::getAlarmInfoID(const QModelIndex & index){
    if(!index.isValid()){
        return "0";
    }

    return infolist.at(index.data(Qt::UserRole).toInt())->ID;
}

int AlarmInfoModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return infolist.size();

}

int	 AlarmInfoModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }

    return 7;

}

QVariant AlarmInfoModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= infolist.size())){
        return QVariant();
    }

    AlarmInfo *info = static_cast<AlarmInfo *> (infolist.at(row));
    if(role == Qt::DisplayRole){
        switch (index.column()) {
        case 0:
            return info->AssetNO;
            break;

        case 1:
        {
            switch (info->AlarmType) {
            case quint8(MS::ALARM_HARDWARECHANGE):
                return QString(tr("Hardware Changed"));
                break;

            case quint8(MS::ALARM_PROCESSMONITOR):
                return QString(tr("Process Monitor"));
                break;

            default:
                return QString(tr("Unknown"));
                break;
            }

        }
            break;

        case 2:
            return info->Message;
            break;

        case 3:
            return info->UpdateTime;
            break;

        case 4:
            return info->Acknowledged?tr("Yes"):tr("No");
            break;

        case 5:
            return info->AcknowledgedBy;
            break;

        case 6:
            return info->AcknowledgedTime;
            break;

        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::EditRole){
        if(index.column() == 1){
            return QString::number(info->AlarmType);
        }

        if(index.column() == 4){
            return info->Acknowledged?"1":"0";
        }

        return index.data(Qt::DisplayRole);
    }

    if(role == Qt::UserRole){
        return row;
    }

    return QVariant();

}

QVariant AlarmInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal){
        switch (section) {
        case 0:
            return QString(tr("Asset NO."));
            break;

        case 1:
            return QString(tr("Alarm Type"));
            break;

        case 2:
            return QString(tr("Message"));
            break;

        case 3:
            return QString(tr("Time"));
            break;

        case 4:
            return QString(tr("Acknowledged"));
            break;

        case 5:
            return QString(tr("Acknowledged By"));
            break;

        case 6:
            return QString(tr("Acknowledged At"));
            break;


        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}




////////////////////////////////////////////////////////////

AlarmInfoSortFilterProxyModel::AlarmInfoSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

    assetNO = QRegExp(".*", Qt::CaseInsensitive);
    type = QRegExp(".*", Qt::CaseInsensitive);
    acknowledged = QRegExp(".*", Qt::CaseInsensitive);

}

void AlarmInfoSortFilterProxyModel::cleanFilters(){

    assetNO = QRegExp(".*", Qt::CaseInsensitive);
    type = QRegExp(".*", Qt::CaseInsensitive);
    acknowledged = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void AlarmInfoSortFilterProxyModel::setFilters(const QRegExp &assetNO, const QRegExp &type, const QRegExp &acknowledged){

    this->assetNO = assetNO;
    this->type = type;
    this->acknowledged = acknowledged;

    invalidateFilter();

}

bool AlarmInfoSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex indexType = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex indexAcknowledged = sourceModel()->index(sourceRow, 4, sourceParent);

    return (index0.data().toString().contains(assetNO)
            && indexType.data(Qt::EditRole).toString().contains(type)
            && indexAcknowledged.data(Qt::EditRole).toString().contains(acknowledged)

            );

}







} //namespace HEHUI
