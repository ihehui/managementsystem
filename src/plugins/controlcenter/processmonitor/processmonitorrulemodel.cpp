#include "processmonitorrulemodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace HEHUI {

ProcessMonitorRuleModel::ProcessMonitorRuleModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ProcessMonitorRuleModel::~ProcessMonitorRuleModel() {
    clearRules();
}

void ProcessMonitorRuleModel::setJsonData(const QJsonArray &array){
    qDebug()<<"--ProcessMonitorRuleModel::setJsonData(...)";

    clearRules();

    beginResetModel();

    for(int i=0;i<array.size();i++){
        QJsonArray infoArray = array.at(i).toArray();
        if(infoArray.size() != 5){
            qCritical()<<"ERROR! Invalid JSON array.";
            continue;
        }

        ProcessMonitorRule *rule = new ProcessMonitorRule();
        int index = 0;
        rule->ruleString = infoArray.at(index++).toString();
        rule->comment = infoArray.at(index++).toString();
        rule->hashRule = infoArray.at(index++).toString().toUInt();
        rule->blacklistRule = infoArray.at(index++).toString().toUInt();

        rule->globalRule = infoArray.at(index++).toString().toUInt();

        rulesList.append(rule);
    }

    endResetModel();
}

QJsonArray ProcessMonitorRuleModel::getLocalRulesJsonData(){
    QJsonArray array;

    foreach (ProcessMonitorRule *rule, rulesList) {
        if(rule->globalRule){continue;}
        QJsonArray infoArray;
        infoArray.append(rule->ruleString);
        infoArray.append(rule->comment);
        infoArray.append(QString::number(rule->hashRule));
        infoArray.append(QString::number(rule->blacklistRule));

        array.append(infoArray);
    }

    return array;

}

void ProcessMonitorRuleModel::addLocalRule(const QString &ruleString, const QString &comment, bool  hashRule, bool blacklistRule){

    if(ruleExists(ruleString)){return;}

    beginResetModel();

    ProcessMonitorRule *rule = new ProcessMonitorRule();
    rule->ruleString = ruleString;
    rule->comment = comment;
    rule->hashRule = hashRule;
    rule->blacklistRule = blacklistRule;

    rule->globalRule = false;

    endResetModel();

}

void ProcessMonitorRuleModel::deleteRule(const QString &ruleString){

    beginResetModel();

    foreach (ProcessMonitorRule *rule, rulesList) {
        if(rule->ruleString.toLower() == ruleString.toLower()){
            rulesList.removeAll(rule);
            delete rule;
            return;
        }
    }

    endResetModel();

}

void ProcessMonitorRuleModel::deleteRule(const QModelIndex & index){
    if(!index.isValid()){
        return;
    }

    beginResetModel();

    rulesList.removeAt(index.data(Qt::UserRole).toInt());

    endResetModel();

}

ProcessMonitorRule * ProcessMonitorRuleModel::getRule(const QModelIndex & index){
    if(!index.isValid()){
        return 0;
    }

    return rulesList.at(index.data(Qt::UserRole).toInt());
}

bool ProcessMonitorRuleModel::ruleExists(const QString &ruleString){

    foreach (ProcessMonitorRule *rule, rulesList) {
        if(rule->ruleString.toLower() == ruleString.toLower()){
            return true;
        }
    }

    return false;

}

int ProcessMonitorRuleModel::rowCount ( const QModelIndex & parent) const {
    if(parent.isValid()){
        return 0;
    }
    return rulesList.size();

}

int	 ProcessMonitorRuleModel::columnCount ( const QModelIndex & parent) const{
    if(parent.isValid()){
        return 0;
    }

    return 4;

}

QVariant ProcessMonitorRuleModel::data ( const QModelIndex & index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= rulesList.size())){
        return QVariant();
    }

    ProcessMonitorRule *info = static_cast<ProcessMonitorRule *> (rulesList.at(row));
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        switch (index.column()) {
        case 0:
            return info->ruleString;
            break;

        case 1:
            return info->globalRule?tr("Global"):tr("Local");
            break;

        case 2:
            return info->hashRule?tr("Hash"):tr("Path");
            break;

        case 3:
            return info->comment;
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

QVariant ProcessMonitorRuleModel::headerData ( int section, Qt::Orientation orientation, int role) const{
    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal){
        switch (section) {
        case 0:
            return QString(tr("Rule"));
            break;

        case 1:
            return QString(tr("Global"));
            break;

        case 2:
            return QString(tr("Type"));
            break;

        case 3:
            return QString(tr("Comment"));
            break;


        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}


void ProcessMonitorRuleModel::clearRules(){

    if(rulesList.isEmpty()){
        return;
    }

    beginResetModel();


    foreach (ProcessMonitorRule *rule, rulesList) {
        delete rule;
        rule = 0;
    }
    rulesList.clear();

    endResetModel();

}



////////////////////////////////////////////////////////////

ProcessMonitorRuleSortFilterProxyModel::ProcessMonitorRuleSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
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

bool ProcessMonitorRuleSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
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
