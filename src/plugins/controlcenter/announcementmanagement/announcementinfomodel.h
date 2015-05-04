#ifndef ANNOUNCEMENTINFOMODEL_H
#define ANNOUNCEMENTINFOMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QObject>

#include "announcementinfo.h"


namespace HEHUI {

class AnnouncementTargetModel : public QAbstractTableModel
{
public:
    AnnouncementTargetModel(QObject *parent);
    ~AnnouncementTargetModel();

    void setJsonData(const QByteArray &jsonData);
    void clear();
    AnnouncementTarget *getInfo(const QModelIndex & index);
    QString getInfoID(const QModelIndex & index);

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    QString m_announcementID;
    QList<AnnouncementTarget *> infolist;

};

class AnnouncementInfoModel : public QAbstractTableModel
{
public:
    AnnouncementInfoModel(QObject *parent);
    ~AnnouncementInfoModel();

    void setJsonData(const QByteArray &jsonData);
    void clear();
    AnnouncementInfo *getInfo(const QModelIndex & index);
    QString getInfoID(const QModelIndex & index);

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    QList<AnnouncementInfo *> infolist;

};


class SortFilterProxyModel : public QSortFilterProxyModel{
    Q_OBJECT

public:
    SortFilterProxyModel(QObject *parent);

    void cleanFilters();
    void setFilters(const QRegExp &id, const QRegExp &keyword, const QRegExp &acknowledged);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QRegExp id;
    QRegExp keyword;
    QRegExp acknowledged;




};


} //namespace HEHUI

#endif // Announcement_H
