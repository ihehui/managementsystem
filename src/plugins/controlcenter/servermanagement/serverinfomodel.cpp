/*
 ****************************************************************************
 * serverinfomodel.cpp
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

#include "serverinfomodel.h"

namespace HEHUI
{

ServerInfoModel::ServerInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // TODO Auto-generated constructor stub

}

ServerInfoModel::~ServerInfoModel()
{
    serversList.clear();
}

void ServerInfoModel::setServersList(QList<ServerInfo *> serversList)
{
    beginResetModel();
    this->serversList = serversList;
    endResetModel();
}

int ServerInfoModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return serversList.size();

}

int	 ServerInfoModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 5;


}

QVariant ServerInfoModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= serversList.size())) {
        return QVariant();
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        ServerInfo *info = static_cast<ServerInfo *> (serversList.at(row));
        switch (index.column()) {
        case 0:
            return info->serverIP;
            break;

        case 1:
            return info->serverPort;
            break;

        case 2: {
            switch (info->currentState) {
            case ServerInfo::NotTested:
                return QString(tr("Unknown"));
                break;
            case ServerInfo::Testing :
                return QString(tr("Testing"));
                break;
            case ServerInfo::TestOK :
                return QString(tr("OK"));
                break;
            case ServerInfo::TestFailed :
                return QString(tr("Failed"));
                break;
            default:
                return QString(tr("Unknown"));
                break;
            }
        }
        break;

        case 3:
            return info->version;
            break;

        case 4:
            return info->serverName;
            break;

        default:
            return QVariant();
            break;
        }
    }


    return QVariant();

}

QVariant ServerInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("IP"));
            break;

        case 1:
            return QString(tr("Port"));
            break;

        case 2:
            return QString(tr("State"));
            break;

        case 3:
            return QString(tr("Version"));
            break;

        case 4:
            return QString(tr("Name"));
            break;


        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}








}
