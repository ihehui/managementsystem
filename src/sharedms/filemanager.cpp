/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "filemanager.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QTimerEvent>
#include <QCryptographicHash>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>


#ifndef FILE_PIECE_LENGTH
#define FILE_PIECE_LENGTH 524288 //512 KB
#endif

#define SUFFIX_TEMP_FILE ".tf"
#define SUFFIX_INFO_FILE ".fi"







FileManager::FileManager(QObject *parent)
    : QThread(parent)
{
    m_quit = false;
    readId = 0;
    //    startVerification = false;
    m_wokeUp = false;

    //qsrand(QDateTime::currentDateTime().toTime_t());

    //    qRegisterMetaType<FileManager::Error>("FileManager::Error");
}

FileManager::~FileManager()
{
    //    m_quit = true;
    //    cond.wakeOne();
    //    wait();

    foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
        metaInfo->file->close();
        delete metaInfo->file;
        //fileMetaInfoHash.remove(metaInfo->md5sum);
        delete metaInfo;
    }
    fileMetaInfoHash.clear();

    qDebug()<<"FileManager::~FileManager()";
}



int FileManager::readPiece(const QByteArray &fileMD5, int pieceIndex)
{
    ReadRequest request;
    //request.fileID = pieceIndex;
    request.fileMD5 = fileMD5;
    request.pieceIndex = pieceIndex;
    //request.length = length;

    QMutexLocker locker(&mutex);
    request.id = readId++;
    readRequests << request;

    if (!m_wokeUp) {
        m_wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }

    return request.id;
}

void FileManager::writePiece(const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM)
{
    WriteRequest request;
    //request.fileID = fileID;
    request.fileMD5 = fileMD5;
    request.pieceIndex = pieceIndex;
    request.data = data;
    request.dataSHA1SUM = dataSHA1SUM;

    QMutexLocker locker(&mutex);
    writeRequests << request;

    if (!m_wokeUp) {
        m_wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}

QList<int/*Piece Index*/> FileManager::completedPieces(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);

    QList<int> ret;
    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return ret;
    }

    for(int i=0; i< info->verifiedPieces.size(); i++){
        if(info->verifiedPieces.testBit(i)){
            ret << i;
        }
    }

    return ret;

}

QList<int/*Piece Index*/> FileManager::uncompletedPieces(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);

    QList<int> ret;
    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return ret;
    }

    for(int i=0; i< info->verifiedPieces.size(); i++){
        if(!info->verifiedPieces.testBit(i)){
            ret << i;
        }
    }

    return ret;

}

int FileManager::getOneUncompletedPiece(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);

    FileMetaInfo * info = fileMetaInfoHash.value(fileMD5);
    if(!info){
        return -1;
    }

    for(int i=0; i< info->verifiedPieces.size(); i++){
        if(!info->verifiedPieces.testBit(i)){
            return i;
        }
    }

    return -1;

}

void FileManager::stop(){
    qDebug()<<"--FileManager::stop() currentThreadId:"<<currentThreadId();

    {
        QMutexLocker locker(&quitMutex);
        m_quit = true;
    }

    {
        QMutexLocker locker(&mutex);
        cond.wakeOne();
    }

    wait();
}

void FileManager::run()
{

    do {
        {
            // Go to sleep if there's nothing to do.
            QMutexLocker locker(&mutex);
            if (!isAboutToQuit() && readRequests.isEmpty() && writeRequests.isEmpty() )
                cond.wait(&mutex);
        }

        // Read pending read requests
        mutex.lock();
        QList<ReadRequest> newReadRequests = readRequests;
        readRequests.clear();
        mutex.unlock();

        while (!newReadRequests.isEmpty()) {
            ReadRequest request = newReadRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
            if(!info){continue;}
            if(request.pieceIndex >= info->verifiedPieces.size()){continue;}
            QByteArray block = readBlock(request.id, info, request.pieceIndex);
            if(block.isEmpty()){continue;}
            if(!info->sha1Sums.contains(request.pieceIndex)){
                QByteArray dataSHA1SUM = QCryptographicHash::hash(block, QCryptographicHash::Sha1);
                info->sha1Sums.insert(request.pieceIndex, dataSHA1SUM);
            }
            //Q_ASSERT(dataSHA1SUM == info->sha1Sums.value(request.pieceIndex));

            emit dataRead(request.id, request.fileMD5, request.pieceIndex, block, info->sha1Sums.value(request.pieceIndex));

        }

        // Write pending write requests
        mutex.lock();
        QList<WriteRequest> newWriteRequests = writeRequests;
        writeRequests.clear();
        while (!isAboutToQuit() && !newWriteRequests.isEmpty()) {
            WriteRequest request = newWriteRequests.takeFirst();
            FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
            if(!info){continue;}
            writeBlock(info, request.pieceIndex, request.data, request.dataSHA1SUM);

        }

        mutex.unlock();

    } while (!isAboutToQuit());


    // Write pending write requests
    mutex.lock();
    QList<WriteRequest> newWriteRequests = writeRequests;
    writeRequests.clear();
    mutex.unlock();
    while (!newWriteRequests.isEmpty()) {
        WriteRequest request = newWriteRequests.takeFirst();
        FileMetaInfo *info = fileMetaInfoHash.value(request.fileMD5) ;
        if(!info){continue;}
        writeBlock(info, request.pieceIndex, request.data, request.dataSHA1SUM);

    }


}

const FileManager::FileMetaInfo * FileManager::tryToSendFile( const QString &localSavePath, FileError *error/*, int pieceLength*/){

    Q_ASSERT(error);
    if(!error){
        qCritical()<<"ERROR! Invalid Pointer!";
        return 0;
    }

    {
        QMutexLocker locker(&mutex);
        foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            if(metaInfo->file->fileName() == localSavePath){
                return metaInfo;
            }
        }
    }

    if(!QFile::exists(localSavePath)){
        error->errorCode = ERROR_FILE_NOT_EXIST;
        error->errorString = tr("File '%1' does not exist!").arg(localSavePath);
        return 0;
    }

    QFile *file = new QFile(localSavePath);
    if (!file->open(QFile::ReadOnly)) {
        error->errorCode = FILE_READ_ERROR;
        error->errorString = tr("Failed to open file %1! %2").arg(localSavePath).arg(file->errorString());
        delete file;
        return 0;
    }

    QByteArray fileMD5Sum = QCryptographicHash::hash(file->readAll(), QCryptographicHash::Md5);
    file->seek(0);
    qint64 fileSize = file->size();

    int pieceCount = fileSize / FILE_PIECE_LENGTH;
    if(fileSize % FILE_PIECE_LENGTH){
        pieceCount++;
    }
    //qDebug()<<"----------------pieceCount:"<<pieceCount;

    FileMetaInfo *info = new FileMetaInfo();
    info->md5sum = fileMD5Sum;
    info->size = fileSize;
    info->verifiedPieces.resize(pieceCount);
    info->verifiedPieces.fill(true);
    info->file = file;
    file->close();

    {
        QMutexLocker locker(&mutex);
        fileMetaInfoHash.insert(fileMD5Sum, info);
        cond.wakeOne();
    }

    return info;

}

const FileManager::FileMetaInfo * FileManager::tryToReceiveFile(QByteArray fileMD5Sum, const QString &localSavePath, quint64 size, FileError *error/*, int pieceLength*/){

    Q_ASSERT(error);
    if(!error){
        qCritical()<<"ERROR! Invalid Pointer!";
        return 0;
    }

    QString tempFilePath = localSavePath + SUFFIX_TEMP_FILE;
    QString infoFilePath = localSavePath + SUFFIX_INFO_FILE;

    QFileInfo fi(localSavePath);
    if(fi.exists()){
        QFile file(localSavePath);
        if (!file.open(QFile::ReadOnly)) {
            error->errorCode = ERROR_FILE_EXIST_WITH_SAME_NAME;
            error->errorString = tr("File '%1' already exists but it can not be opened!").arg(localSavePath);
            return 0;
        }
        QByteArray md5Sum = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
        if(md5Sum == fileMD5Sum){
            error->errorCode = ERROR_FILE_EXIST_WITH_SAME_CONTENT_AND_NAME;
            error->errorString = tr("File '%1' already exists!").arg(localSavePath);
        }else{
            error->errorCode = ERROR_FILE_EXIST_WITH_SAME_NAME;
            error->errorString = tr("File '%1' already exists but it has different content!").arg(localSavePath);
        }
        return 0;

    }else{
        if(!fi.dir().mkpath(fi.absolutePath())){
            error->errorCode = FILE_WRITE_ERROR;
            error->errorString = tr("Failed to make path '%1'!").arg(fi.absolutePath());
            return 0;
        }
    }

    {
        QMutexLocker locker(&mutex);
        foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
            if(metaInfo->file->fileName() == tempFilePath){
                if(metaInfo->md5sum == fileMD5Sum ){
                    return metaInfo;
                }else{
                    error->errorCode = ERROR_FILE_IN_USE;
                    error->errorString = tr("File '%1' is in use!").arg(localSavePath);
                    return 0;
                }
            }
        }
    }

    QFile *file = new QFile(tempFilePath);
    if (!file->open(QFile::ReadWrite)) {
        error->errorCode = FILE_WRITE_ERROR;
        error->errorString = tr("Failed to open/create file %1: %2").arg(file->fileName()).arg(file->errorString());
        delete file;
        return 0;
    }


    QSettings settings(infoFilePath, QSettings::IniFormat);
    QByteArray oldMD5 = settings.value("MD5", QByteArray()).toByteArray();
    quint64 oldSize = settings.value("size", 0).toULongLong();
    if(oldMD5 != fileMD5Sum || oldSize != size || file->size() == 0){
        file->resize(0);
        if (!file->resize(size)) {
            error->errorCode = FILE_WRITE_ERROR;
            error->errorString = tr("Failed to resize file %1: %2").arg(file->fileName()).arg(file->errorString());
            delete file;
            return 0;
        }
    }
    file->close();


    QBitArray ba = settings.value("Pieces", QBitArray()).toBitArray();
    if(ba.isEmpty()){
        int pieces = size / FILE_PIECE_LENGTH;
        if(size % FILE_PIECE_LENGTH){
            pieces++;
        }
        ba.resize(pieces);
        ba.fill(false);
    }

    settings.setValue("MD5", fileMD5Sum);
    settings.setValue("size", size);
    settings.setValue("Pieces", ba);
    settings.sync();

    FileMetaInfo *info = new FileMetaInfo();

    info->md5sum = fileMD5Sum;
    info->size = size;
    //info->pieceLength = pieceLength;
    info->verifiedPieces = ba;
    //info->newFile = true;
    info->file = file;
    info->infoFileName = infoFilePath;

    {
        QMutexLocker locker(&mutex);
        fileMetaInfoHash.insert(fileMD5Sum, info);
        cond.wakeOne();
    }

    return info;

}

void FileManager::closeFile(const QString &filePath){

    QMutexLocker locker(&mutex);
    foreach (FileMetaInfo *metaInfo, fileMetaInfoHash) {
        if(metaInfo->file->fileName() == filePath){
            metaInfo->file->close();
            delete metaInfo->file;
            fileMetaInfoHash.remove(metaInfo->md5sum);
            delete metaInfo;
            return;
        }
    }

    //errString = tr("File '%1' does not exist!").arg(filePath);

}

void FileManager::closeFile(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);
    FileMetaInfo *metaInfo = fileMetaInfoHash.take(fileMD5);
    if(metaInfo){
        metaInfo->file->close();
        delete metaInfo->file;
    }

    delete metaInfo;

}

QString FileManager::getFileLocalSavePath(const QByteArray &fileMD5){

    QMutexLocker locker(&mutex);
    FileMetaInfo *metaInfo = fileMetaInfoHash.value(fileMD5);
    if(metaInfo){
        return metaInfo->file->fileName();
    }

    return "";
}

QByteArray FileManager::readBlock(int requestID, FileMetaInfo *info, int pieceIndex){
    //qDebug()<<"--FileManager::readBlock(...) "<<" pieceIndex:"<<pieceIndex;

    QByteArray block;

    if(!info){
        QString errString = tr("Failed to read file info!");
        emit error(requestID, info->md5sum, quint8(ERROR_UNKNOWN), errString);
        return block;
    }


    qint64 startReadIndex = (quint64(pieceIndex) * FILE_PIECE_LENGTH);

    QFile *file = info->file;
    qint64 currentFileSize = info->size;
    if (!file->isOpen()) {
        if (!file->open(QFile::ReadOnly)) {
            QString errString = tr("Failed to open file '%1'! %2").arg(file->fileName()).arg(file->errorString());
            emit error(requestID, info->md5sum, quint8(FILE_READ_ERROR), errString);
            return block;
        }
    }

    file->seek(startReadIndex);
    qint64 sizeToRead = qMin<qint64>(FILE_PIECE_LENGTH, currentFileSize - file->pos());
    block = file->read(sizeToRead);
    file->close();

    if (block.size() != sizeToRead) {
        QString errString = tr("Failed to read data from file '%1'! %2").arg(file->fileName()).arg(file->errorString());
        emit error(requestID, info->md5sum, quint8(FILE_READ_ERROR), errString);
        return QByteArray();
    }

    return block;
}

bool FileManager::writeBlock(FileMetaInfo *info, int pieceIndex, const QByteArray &data, const QByteArray &dataSHA1SUM){
    //qDebug()<<"--FileManager::writeBlock(...) "<<"pieceIndex:"<<pieceIndex<<" data size:"<<data.size()<<" MD5:"<<dataSHA1SUM.toBase64();

    if(!info){
        QString errString = tr("Failed to read file info!");
        emit error(0, info->md5sum, quint8(ERROR_UNKNOWN), errString);
        return false;
    }

    QByteArray sha1Sum = QCryptographicHash::hash(data, QCryptographicHash::Sha1);
    if(sha1Sum != dataSHA1SUM){
        emit pieceVerified(info->md5sum, pieceIndex, false, 0);
        return false;
    }


    qint64 startWriteIndex = (qint64(pieceIndex) * FILE_PIECE_LENGTH);
    int bytesToWrite = data.size();
    //int bytesToWrite = qMin<qint64>(data.size(), info->size - startWriteIndex);

    QFile *file = info->file;
    //qint64 currentFileSize = info->size;

    if (!file->isOpen()) {
        if (!file->open(QFile::ReadWrite)) {
            QString errString = tr("Failed to open file '%1'! %2").arg(file->fileName()).arg(file->errorString());
            emit error(0, info->md5sum, quint8(FILE_READ_ERROR), errString);
            return false;
        }
    }

    file->seek(startWriteIndex);
    qint64 bytesWritten = file->write(data.constData(), bytesToWrite);
    file->flush();
    file->close();

    if (bytesWritten != bytesToWrite) {
        QString errString = tr("Failed to write data to file '%1'! %2").arg(file->fileName()).arg(file->errorString());
        emit error(0, info->md5sum, quint8(FILE_WRITE_ERROR), errString);
        return false;
    }

    info->sha1Sums.insert(pieceIndex, dataSHA1SUM);


    ////////////////////////////////////////

    info->verifiedPieces.setBit(pieceIndex);

    if((pieceIndex % 25) == 0){
        QSettings settings(info->infoFileName, QSettings::IniFormat);
        settings.setValue("Pieces", info->verifiedPieces);
        settings.sync();
    }

    int verificationProgress = (info->verifiedPieces.count(true) * 100) / info->verifiedPieces.size();
    Q_ASSERT(verificationProgress >= 0 && verificationProgress <= 100);

    if(verificationProgress == 100){
        QString name = info->file->fileName();
        Q_ASSERT(name.endsWith(SUFFIX_TEMP_FILE));
        name.remove(SUFFIX_TEMP_FILE);
        if(!info->file->rename(name)){
            qCritical()<<"ERROR! Failed to rename file!";
        }else{
            QFile::remove(info->infoFileName);
            //qWarning()<<"File Received!";
        }
    }

    emit pieceVerified(info->md5sum, pieceIndex, true, verificationProgress);

    return true;


    //return verifySinglePiece(info, pieceIndex);


}

bool FileManager::verifySinglePiece(FileMetaInfo *info, int pieceIndex)
{

    if(!info){
        return false;
    }

    //qint64 startReadIndex = (quint64(pieceIndex) * FILE_PIECE_LENGTH);

    QByteArray block = readBlock(0, info, pieceIndex);
    QByteArray sha1Sum = QCryptographicHash::hash(block, QCryptographicHash::Sha1);

    bool verified = false;
    if (sha1Sum != info->sha1Sums.value(pieceIndex)){
        verified = false;
    }else{
        info->verifiedPieces.setBit(pieceIndex);
        verified = true;
    }

    if((pieceIndex % 10) == 0){
        QSettings settings(info->infoFileName, QSettings::IniFormat);
        settings.setValue("Pieces", info->verifiedPieces);
        settings.sync();
    }

    int verificationProgress = (info->verifiedPieces.count(true) * 100) / info->verifiedPieces.size();
    Q_ASSERT(verificationProgress >= 0 && verificationProgress <= 100);

    if(verificationProgress == 100){
        QString name = info->file->fileName();
        Q_ASSERT(name.endsWith(SUFFIX_TEMP_FILE));
        name.remove(SUFFIX_TEMP_FILE);
        if(!info->file->rename(name)){
            qCritical()<<"ERROR! Failed to rename file!";
        }else{
            //QString cfgFile = name + SUFFIX_INFO_FILE;
            QFile::remove(info->infoFileName);
            QFile::remove(info->infoFileName);
            //qWarning()<<"File Received!";
        }
    }

    emit pieceVerified(info->md5sum, pieceIndex, verified, verificationProgress);

    return verified;
}

void FileManager::wakeUp()
{
    //qDebug()<<"--FileManager::wakeUp:"<<currentThreadId();

    QMutexLocker locker(&mutex);
    m_wokeUp = false;
    cond.wakeOne();
}

bool FileManager::isAboutToQuit(){
    //qDebug()<<"--FileManager::isAboutToQuit:"<<currentThreadId();

    QMutexLocker locker(&quitMutex);
    return m_quit;
}
