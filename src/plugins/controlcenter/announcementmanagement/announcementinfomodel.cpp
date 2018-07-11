#include "announcementinfomodel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "../../sharedms/global_shared.h"

namespace HEHUI
{



AnnouncementTargetModel::AnnouncementTargetModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_announcementID = 0;
}

AnnouncementTargetModel::~AnnouncementTargetModel()
{
    clear();
}

void AnnouncementTargetModel::setJsonData(const QByteArray &jsonData)
{

    clear();

    if(jsonData.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    m_announcementID = object["AnnouncementID"].toInt();
    QJsonArray jsonArray = object["AnnouncementTargets"].toArray();

    beginResetModel();

    for(int i = 0; i < jsonArray.size(); i++) {
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }


        AnnouncementTarget *info = new AnnouncementTarget();
        info->AnnouncementID = m_announcementID;

        int index = 0;
        info->ID = infoArray.at(index++).toString();
        info->AssetNO = infoArray.at(index++).toString();
        info->UserName = infoArray.at(index++).toString();
        info->Acknowledged = infoArray.at(index++).toString().toUShort();
        info->ACKTime = infoArray.at(index++).toString();

        infolist.append(info);

    }

    endResetModel();
}

void AnnouncementTargetModel::addComputerTargets(const QStringList &targets)
{
    if(targets.isEmpty()) {
        return;
    }

    beginResetModel();

    foreach (QString assetNO, targets) {
        AnnouncementTarget *info = new AnnouncementTarget();
        info->AssetNO = assetNO.trimmed();
        infolist.append(info);
    }

    endResetModel();
}

void AnnouncementTargetModel::addUserTargets(const QStringList &targets)
{

    if(targets.isEmpty()) {
        return;
    }

    beginResetModel();

    foreach (QString target, targets) {
        AnnouncementTarget *info = new AnnouncementTarget();
        QStringList list = target.split("\\");
        if(list.size() == 2) {
            info->AssetNO = list.at(0);
            info->UserName = list.at(1);
        } else {
            info->UserName = list.at(0);
        }

        infolist.append(info);
    }

    endResetModel();
}

QString AnnouncementTargetModel::getNewTargetsStringForSQL()
{
    QStringList targets;
    foreach (AnnouncementTarget *info, infolist) {
        if(!info->ID.isEmpty()) {
            continue;
        }
        QString assetNO = info->AssetNO, userName = info->UserName;
        assetNO = "\\'" + assetNO + "\\'";
        userName = "\\'" + userName + "\\'";
        targets.append(assetNO + "," + userName);
    }
    targets.removeDuplicates();
    return targets.join(";");
}

QString AnnouncementTargetModel::getDeletedTargetsStringForSQL()
{
    return deletedTargets.join(",");
}

void AnnouncementTargetModel::clear()
{
    beginResetModel();

    foreach (AnnouncementTarget *info, infolist) {
        delete info;
        info = 0;
    }
    infolist.clear();

    endResetModel();
}

AnnouncementTarget *AnnouncementTargetModel::getTarget(const QModelIndex &index)
{
    if(!index.isValid()) {
        return 0;
    }

    return infolist.at(index.data(Qt::UserRole).toInt());
}

void AnnouncementTargetModel::deleteTarget(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    beginResetModel();

    AnnouncementTarget *target = infolist.takeAt(index.data(Qt::UserRole).toInt());
    if(!target) {
        return;
    }
    if(!target->ID.isEmpty()) {
        deletedTargets.append(target->ID);
    }
    delete target;

    endResetModel();
}

QString AnnouncementTargetModel::getTargetID(const QModelIndex &index)
{
    if(!index.isValid()) {
        return "0";
    }

    return infolist.at(index.data(Qt::UserRole).toInt())->ID;
}

void AnnouncementTargetModel::switchToCloneMode()
{
    foreach (AnnouncementTarget *info, infolist) {
        info->ID = "";
    }
}

int AnnouncementTargetModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return infolist.size();

}

int	 AnnouncementTargetModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 4;

}

QVariant AnnouncementTargetModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= infolist.size())) {
        return QVariant();
    }

    AnnouncementTarget *info = static_cast<AnnouncementTarget *> (infolist.at(row));
    if(role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return info->AssetNO;
            break;

        case 1:
            return info->UserName;
            break;

        case 2:
            return info->Acknowledged ? tr("Yes") : tr("No");
            break;

        case 3:
            return info->ACKTime;
            break;


        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::UserRole) {
        return row;
    }

    return QVariant();

}

QVariant AnnouncementTargetModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("Asset NO."));
            break;

        case 1:
            return QString(tr("User Name"));
            break;

        case 2:
            return QString(tr("Acknowledged"));
            break;

        case 3:
            return QString(tr("ACK Time"));
            break;


        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}

//////////////////////////////////////////////////////



//////////////////////////////////////////////////////

AnnouncementInfoModel::AnnouncementInfoModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

AnnouncementInfoModel::~AnnouncementInfoModel()
{
    clear();
    qDeleteAll(replies);
}

void AnnouncementInfoModel::setJsonData(const QByteArray &jsonData)
{

    clear();

    if(jsonData.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();
    QJsonArray jsonArray = object["Announcements"].toArray();

    beginResetModel();

    for(int i = 0; i < jsonArray.size(); i++) {
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }

        AnnouncementInfo *info = new AnnouncementInfo();
        int index = 0;
        info->ID = infoArray.at(index++).toInt();
        info->Type = infoArray.at(index++).toString().toUShort();
        info->Content = infoArray.at(index++).toString();
        info->ACKRequired = infoArray.at(index++).toString().toUShort();
        info->Admin = infoArray.at(index++).toString();
        info->PublishDate = infoArray.at(index++).toString();
        info->ValidityPeriod = infoArray.at(index++).toString().toUInt();
        info->TargetType = infoArray.at(index++).toString().toUShort();
        info->DisplayTimes = infoArray.at(index++).toString().toUInt();
        info->Active = infoArray.at(index++).toString().toUInt();

        infolist.append(info);

    }

    endResetModel();
}

void AnnouncementInfoModel::clear()
{
    beginResetModel();

    foreach (AnnouncementInfo *info, infolist) {
        delete info;
        info = 0;
    }
    infolist.clear();

    endResetModel();
}

AnnouncementInfo *AnnouncementInfoModel::getInfo(const QModelIndex &index)
{
    if(!index.isValid()) {
        return 0;
    }

    return infolist.at(index.data(Qt::UserRole).toInt());
}

QString AnnouncementInfoModel::getInfoID(const QModelIndex &index)
{
    if(!index.isValid()) {
        return "0";
    }

    return infolist.at(index.data(Qt::UserRole).toInt())->ID;
}

void AnnouncementInfoModel::setAnnouncementRepliesData(const QByteArray &jsonData)
{

    if(jsonData.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if(error.error != QJsonParseError::NoError) {
        qCritical() << error.errorString();
        return;
    }
    QJsonObject object = doc.object();

    QString announcementID = object["AnnouncementID"].toString();
    AnnouncementInfo *info = 0;
    foreach (AnnouncementInfo *info, infolist) {
        if(info->ID == announcementID) {
            break;
        };
    }

    QJsonArray jsonArray = object["AnnouncementReplies"].toArray();
    for(int i = 0; i < jsonArray.size(); i++) {
        QJsonArray infoArray = jsonArray.at(i).toArray();
        if(infoArray.size() != 10) {
            qCritical() << "ERROR! Invalid JSON array.";
            continue;
        }

        int index = 0;
        QString id = infoArray.at(index++).toString();
        AnnouncementReply *replyinfo = 0;
        if(replies.contains(id)) {
            replyinfo = replies.value(id);
            if(info) {
                info->Replies = "";
            }
        } else {
            replyinfo = new AnnouncementReply();
            replyinfo->ID = id;
            replies.insert(id, replyinfo);
        }
        replyinfo->AnnouncementID = infoArray.at(index++).toString().toUShort();
        replyinfo->Sender = infoArray.at(index++).toString();
        replyinfo->SendersAssetNO = infoArray.at(index++).toString();
        replyinfo->Receiver = infoArray.at(index++).toString();
        replyinfo->ReceiversAssetNO = infoArray.at(index++).toString();
        replyinfo->Message = infoArray.at(index++).toString();
        replyinfo->PublishTime = infoArray.at(index++).toString();

        if(info) {
            //URL: Reply://sender
            QString title = QString("<span><a title=\"%1\" href=\"%2://%1:%3\">%1</a> %4</span>").arg(replyinfo->Sender).arg(URLScheme_Reply).arg(replyinfo->SendersAssetNO).arg(replyinfo->PublishTime);
            //QString title = QString(" <p align=\"left\"><span style=\" font-size:9pt;color:#068ec8;\">%1 %2</span></p> ").arg(replyinfo->Sender).arg(replyinfo->PublishTime);
            info->Replies += title + replyinfo->Message;
        }

    }

}

QList<AnnouncementReply *> AnnouncementInfoModel::getAnnouncementReplies(const QString &announcementID) const
{
    QList<AnnouncementReply *> results;
    foreach (AnnouncementReply *reply, replies.values()) {
        if(reply->AnnouncementID == announcementID) {
            results.append(reply);
        }
    }
    return results;
}

AnnouncementReply *AnnouncementInfoModel::getReply(const QString &replyID) const
{
    return replies.value(replyID);
}


int AnnouncementInfoModel::rowCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return infolist.size();

}

int	 AnnouncementInfoModel::columnCount ( const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return 9;

}

QVariant AnnouncementInfoModel::data ( const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if((row < 0) || (row >= infolist.size())) {
        return QVariant();
    }

    AnnouncementInfo *info = static_cast<AnnouncementInfo *> (infolist.at(row));
    if(role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return info->ID;
            break;

        case 1: {
            switch (info->Type) {
            case quint8(MS::ANNOUNCEMENT_NORMAL):
                return QString(tr("Normal"));
                break;

            case quint8(MS::ANNOUNCEMENT_CRITICAL):
                return QString(tr("Critical"));
                break;

            default:
                return QString(tr("Unknown"));
                break;
            }

        }
        break;

        case 2:
            return info->ACKRequired ? tr("Yes") : tr("No");
            break;

        case 3:
            return info->Admin;
            break;

        case 4:
            return info->PublishDate;
            break;

        case 5:
            return info->ValidityPeriod;
            break;

        case 6: {
            switch (info->TargetType) {
            case quint8(MS::ANNOUNCEMENT_TARGET_EVERYONE):
                return QString(tr("All"));
                break;

            case quint8(MS::ANNOUNCEMENT_TARGET_SPECIFIC):
                return QString(tr("Specific"));
                break;

            default:
                return QString(tr("All"));
                break;
            }

        }
        break;

        case 7:
            return info->Active ? tr("Yes") : tr("No");
            break;

        case 8:
            return info->Content;
            break;

        default:
            return QVariant();
            break;
        }
    }

    if(role == Qt::EditRole) {
        if(index.column() == 1) {
            return QString::number(info->Type);
        }

        if(index.column() == 2) {
            return info->ACKRequired ? "1" : "0";
        }

        if(index.column() == 6) {
            return QString::number(info->TargetType);
        }

        return index.data(Qt::DisplayRole);
    }

    if(role == Qt::UserRole) {
        return row;
    }

    return QVariant();

}

QVariant AnnouncementInfoModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation ==  Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString(tr("ID"));
            break;

        case 1:
            return QString(tr("Type"));
            break;

        case 2:
            return QString(tr("ACK Required"));
            break;

        case 3:
            return QString(tr("Admin"));
            break;

        case 4:
            return QString(tr("Publish Date"));
            break;

        case 5:
            return QString(tr("Validity Period"));
            break;

        case 6:
            return QString(tr("Target"));
            break;

        case 7:
            return QString(tr("Active"));
            break;

        case 8:
            return QString(tr("Content"));
            break;


        default:
            return QVariant();
            break;
        }

    }

    return QVariant();

}




////////////////////////////////////////////////////////////

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

    id = QRegExp(".*", Qt::CaseInsensitive);
    keyword = QRegExp(".*", Qt::CaseInsensitive);
    acknowledged = QRegExp(".*", Qt::CaseInsensitive);

}

void SortFilterProxyModel::cleanFilters()
{

    id = QRegExp(".*", Qt::CaseInsensitive);
    keyword = QRegExp(".*", Qt::CaseInsensitive);
    acknowledged = QRegExp(".*", Qt::CaseInsensitive);

    invalidateFilter();
}

void SortFilterProxyModel::setFilters(const QRegExp &id, const QRegExp &keyword, const QRegExp &acknowledged)
{

    this->id = id;
    this->keyword = keyword;
    this->acknowledged = acknowledged;

    invalidateFilter();

}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex indexKeyword = sourceModel()->index(sourceRow, 7, sourceParent);
    QModelIndex indexAcknowledged = sourceModel()->index(sourceRow, 4, sourceParent);

    return (index0.data().toString().contains(id)
            && indexKeyword.data(Qt::EditRole).toString().contains(keyword)
            && indexAcknowledged.data(Qt::EditRole).toString().contains(acknowledged)

           );

}







} //namespace HEHUI
