#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QWidget>
#include <QObject>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QFileIconProvider>
#include <QList>
#include <QDragEnterEvent>
#include <QMap>
#include <QMessageBox>

#include "ui_filemanagement.h"

#include "../../sharedms/global_shared.h"

#include "networkmanager/controlcenterpacketsparser.h"


namespace HEHUI
{


class FileSystemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileSystemModel(QFileIconProvider *fileIconProvider, QObject *parent = 0);
    virtual ~FileSystemModel();

//    void setFileItems(QList<FileItemInfo> fileItems);
    void addFileItem(const QString &name, const QString &size, quint8 type, const QString &dateModified);
    void deleteFileItem(const QString &name);
    void deleteFileItem(const QModelIndex &index);


    int rowCount ( const QModelIndex &parent = QModelIndex() ) const ;
    int	columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const ;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);

    bool isDrive(const QModelIndex &index);
    bool isDir(const QModelIndex &index);
    QString absoluteFilePath(const QModelIndex &index);
    QString fileName(const QModelIndex &index);

    void changePath(const QString &newPath);

    QString currentDirPath() const;
    QStringList drives() const;

private:
    struct FileItemInfo {
        QString name;
        QString size;
        quint8 type;
        QString dateModified;

    };

    QList<FileItemInfo *> fileItems;
    QString m_currentDirPath;
    QFileIconProvider *m_fileIconProvider;

    QStringList m_drives;

};



class FileManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileManagementWidget(QWidget *parent = 0);
    ~FileManagementWidget();

    void setPacketsParser(ControlCenterPacketsParser *parser);
    void setPeerSocket(SOCKETID peerSocket);


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void keyReleaseEvent(QKeyEvent *keyEvent);


signals:
//    void signalShowRemoteFiles(const QString &path);
//    void signalUploadFilesToRemote(const QStringList &localFiles, const QString &remoteDir);
//    void signalDownloadFileFromRemote(const QStringList &remoteFiles, const QString &localDir);

public slots:
    void peerDisconnected(bool normalClose);

    ///////////////////

    void processFileTransferPacket(const FileTransferPacket &packet);

    void requestFileSystemInfo(const QString &parentDirPath);
    void fileSystemInfoReceived(SOCKETID socketID, const QString &parentDirPath, const QByteArray &fileSystemInfoData);

    void requestUploadFilesToRemote(const QString &localBaseDir, const QStringList &localFiles, const QString &remoteDir);
    void requestDownloadFileFromRemote(const QString &remoteBaseDir, const QStringList &remoteFiles, const QString &localDir);

    //File TX
    void startFileManager();
    void fileDownloadRequestAccepted(SOCKETID socketID, const QString &remoteFileName, const QByteArray &fileMD5Sum, quint64 size, const QString &pathToSaveFile);
    void fileDownloadRequestDenied(SOCKETID socketID, const QString &remoteFileName, quint8 errorCode);
    void fileUploadRequestResponsed(SOCKETID socketID, const QByteArray &fileMD5Sum, bool accepted, const QString &message);

    void processFileDataRequestPacket(SOCKETID socketID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void processFileDataReceivedPacket(SOCKETID socketID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void processFileTXStatusChangedPacket(SOCKETID socketID, const QByteArray &fileMD5, quint8 status);
    void processFileTXErrorFromPeer(SOCKETID socketID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorMessage);

    void fileDataRead(int requestID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM);
    void fileTXError(int requestID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);
    void pieceVerified(const QByteArray &fileMD5, int pieceIndex, bool verified, int verificationProgress);

    void processPeerRequestUploadFilePacket(SOCKETID socketID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void processPeerRequestDownloadFilePacket(SOCKETID socketID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);

    void processFileDeletingResult(const QString &baseDirPath, const QStringList &failedFiles);
    void processFileRenamingResult(const QString &baseDirPath, const QString &oldFileName, const QString &newFileName, bool renamed, const QString &message);

private slots:

    void on_groupBoxLocal_toggled( bool on );
    void on_toolButtonShowLocalFiles_clicked();
    void comboBoxLocalPathCurrentIndexChanged(int index);
    void localFileItemDoubleClicked(const QModelIndex &index);
    void changeLocalFilePath(const QString &newPath);

    void on_groupBoxRemote_toggled( bool on );
    void on_toolButtonShowRemoteFiles_clicked();
    void comboBoxRemotePathIndexChanged(int index);
    void tableViewRemoteFileItemDoubleClicked(const QModelIndex &index);
    void changeRemoteFilePath(const QString &newPath);

    bool getLocalFilesInfo(const QString &parentDirPath, QByteArray *result, QString *errorMessage);
    bool parseRemoteFilesInfo(const QString &remoteParentDirPath, const QByteArray &data);

    void on_pushButtonUploadToRemote_clicked();
    void on_pushButtonDownloadToLocal_clicked();

    void deleteFiles(bool deleteRemoteFiles);
    void deleteLocalFiles(const QString &path, QStringList *failedFiles = 0, const QStringList &nameFilters = QStringList(), const QStringList &ignoredFiles = QStringList(), const QStringList &ignoredDirs = QStringList());

    void renameFile(bool renameRemoteFile);

    void appendMessage(const QString &info, QtMsgType msgType = QtDebugMsg, bool showMsgBox = false, const QString &extraMsg = "");

private:
    Ui::FileManagement ui;

    ControlCenterPacketsParser *controlCenterPacketsParser;

    QFileSystemModel *localFileSystemModel;
    QCompleter *localFilesCompleter;
    QString m_localCurrentDir;

    FileSystemModel *m_remoteFileSystemModel;
    QSortFilterProxyModel *m_proxyModel;

    unsigned int m_peerSocket;

    FileManager *m_fileManager;
    QList<int/*File TX Request ID*/> fileTXRequestList;
    QList<QByteArray/*File MD5*/> filesList;


};

}

#endif // FILEMANAGEMENT_H
