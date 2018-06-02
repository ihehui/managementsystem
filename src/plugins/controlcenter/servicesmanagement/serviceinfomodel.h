/*
 ****************************************************************************
 * ServiceInfomodel.h
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

#ifndef SERVICEINFOMODEL_H_
#define SERVICEINFOMODEL_H_


#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#ifdef Q_OS_WIN
    #include "HHSharedSystemUtilities/WinUtilities"
#else
    #include "HHSharedSystemUtilities/UnixUtilities"
#endif



namespace HEHUI
{



class ServiceInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ServiceInfoModel(QObject *parent = 0);
    virtual ~ServiceInfoModel();

    void setJsonData(const QByteArray &data, unsigned long *errorCode = 0, QString *errorMessage = 0);

    ServiceInfo *getServiceInfo(const QString &serviceName);
    bool updateServiceInfo(const QString &serviceName, quint64 processID, quint64 startupType);

    int rowCount ( const QModelIndex &parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


private:
    QString getStartTypeString(unsigned long startType) const;
    QString getServiceTypeString(unsigned long serviceType) const;

    void clearServicesList();

private:
    QList<ServiceInfo *> servicesList;


};


class ServiceInfoSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ServiceInfoSortFilterProxyModel(QObject *parent);

    void cleanFilters();
    void setFilters(const QRegExp &serviceName, const QRegExp &serviceDisplayName, const QRegExp &serviceType);


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QRegExp serviceName;
    QRegExp serviceDisplayName;
    QRegExp serviceType;




};



}

#endif /* SERVICEINFOMODEL_H_ */
