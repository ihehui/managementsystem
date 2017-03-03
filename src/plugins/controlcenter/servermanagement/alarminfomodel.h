#ifndef ALARMINFOMODEL_H
#define ALARMINFOMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QObject>

#include "../../sharedms/alarminfo.h"


namespace HEHUI
{


class AlarmInfoModel : public QAbstractTableModel
{
public:
    AlarmInfoModel(QObject *parent);
    ~AlarmInfoModel();

    void setJsonData(const QByteArray &jsonData);
    void clear();
    AlarmInfo *getAlarmInfo(const QModelIndex &index);
    QString getAlarmInfoID(const QModelIndex &index);

    int rowCount ( const QModelIndex &parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    QList<AlarmInfo *> infolist;

};


class AlarmInfoSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AlarmInfoSortFilterProxyModel(QObject *parent);

    void cleanFilters();
    void setFilters(const QRegExp &assetNO, const QRegExp &type, const QRegExp &acknowledged);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QRegExp assetNO;
    QRegExp type;
    QRegExp acknowledged;




};


} //namespace HEHUI

#endif // ALARMINFOMODEL_H
