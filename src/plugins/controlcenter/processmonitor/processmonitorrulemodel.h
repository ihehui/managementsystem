#ifndef PROCESSMONITORRULEINFOMODEL_H
#define PROCESSMONITORRULEINFOMODEL_H

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QJsonArray>


#include "../../sharedms/processmonitorrule.h"

namespace HEHUI {

class ProcessMonitorRuleModel : public QAbstractTableModel {
    Q_OBJECT

public:
    ProcessMonitorRuleModel(QObject *parent = 0);
    virtual ~ProcessMonitorRuleModel();

    void setJsonData(const QJsonArray &array);
    QJsonArray getLocalRulesJsonData();

    void addLocalRule(const QString &ruleString, const QString &comment, bool  hashRule, bool blacklistRule);
    void deleteRule(const QString &ruleString);
    void deleteRule(const QModelIndex & index);

    ProcessMonitorRule * getRule(const QModelIndex & index);
    bool ruleExists(const QString &ruleString);



    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    void clearRules();


private:
    QList<ProcessMonitorRule *> rulesList;


};

class ProcessMonitorRuleSortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    ProcessMonitorRuleSortFilterProxyModel(QObject *parent);

//    void cleanFilters();
//    void setFilters(const QRegExp &computerName, const QRegExp &userName, const QRegExp &workgroup, const QRegExp &usbSD, const QRegExp &mac, const QRegExp &ip, const QRegExp &os, const QRegExp &programs);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
//    QRegExp computerName;
//    QRegExp userName;
//    QRegExp workgroup;
//    QRegExp usbSD;
//    QRegExp mac;
//    QRegExp ip;
//    QRegExp os;
//    QRegExp programs;



};



}
#endif // ProcessMonitorRuleMODEL_H
