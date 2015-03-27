#ifndef WINUSERINFOMODEL_H
#define WINUSERINFOMODEL_H

#include <QObject>
#include <QStringList>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>


#include "winuserinfo.h"

namespace HEHUI {

class WinUserInfoModel : public QAbstractTableModel {
    Q_OBJECT

public:
    WinUserInfoModel(QObject *parent = 0);
    virtual ~WinUserInfoModel();

    void setJsonData(const QByteArray &data);
    WinUserInfo * getUser(const QModelIndex & index);


    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    void clearUsers();


private:
    QList<WinUserInfo *> usersList;


};

class WinUserInfoSortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    WinUserInfoSortFilterProxyModel(QObject *parent);

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
#endif // WINUSERINFOMODEL_H
