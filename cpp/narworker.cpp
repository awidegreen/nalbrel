/****************************************************************************************
 * Copyright (c) 2009 Armin Widegreen <armin.widegreen@gmail.com>                       *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation, either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/ 

#include "narworker.h"

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QTime>
#include <QDir>

#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <tstring.h>

#include "album.h"
#include "queuemanager.h"
#include "filenotfoundexception.h"

NARWorker::NARWorker(int threadId,
                             QueueManager* queueManager,
                             const QString &file,
                             bool deleteRarFileAfterwards
                             ) :
        mExtractProcess(new QProcess),
        mRarFile(file),
        mDeleteRarFileAfterwards(deleteRarFileAfterwards),
        mThreadId(threadId),
        mQueueManager(queueManager)
{
    QFileInfo *fInfo;
    if (file.isNull() || file.isEmpty() || !(fInfo = new QFileInfo(file))->exists()) {
        delete fInfo;
        throw new FileNotFoundException(QString("The file: ").append(file).append("does not exists!"));
    }


    mExtractProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(&mThread,SIGNAL(started()), this, SLOT(start()));
    connect(this, SIGNAL(finished(int)), &mThread, SLOT(quit()));
    connect(this, SIGNAL(logMessage(int,QString)),
            mQueueManager, SLOT(logMessage(int,QString)));
    connect(this, SIGNAL(finished(int)), mQueueManager, SLOT(jobFinished(int)));

    moveToThread(&mThread);
    mExtractProcess->moveToThread(&mThread);
}

NARWorker::~NARWorker()
{
    mExtractProcess->close();
    delete mExtractProcess;
}


void NARWorker::start()
{
    QString tmpDirectoryName = QTime::currentTime().toString("hhmmsszzz_") + QString::number(mThreadId) + "/";
    int i = mRarFile.lastIndexOf("/");
    QString wd = mRarFile.left(i+1);                    // working directory with /

    QString tmpWd = wd + tmpDirectoryName;

    mExtractProcess->setWorkingDirectory(wd);

    // create tmp dir
    mExtractProcess->start(QString("mkdir ").append(tmpDirectoryName));
    mExtractProcess->waitForFinished();

    extract(tmpWd);

    renameDirectory(tmpWd, wd);

    // delete tmp folder
    mExtractProcess->start(QString("rm -fr ").append(tmpWd));
    mExtractProcess->waitForFinished();

    if (mDeleteRarFileAfterwards) {
        QFile::remove(mRarFile);
        //mExtractProcess->start(QString("rm -f ").append(mRarFile));
        //mExtractProcess->waitForFinished();
    }

    emit finished(mThreadId);
}

bool NARWorker::extract(QString &tmpDirectoryPath)
{
    // unrar file
    mExtractProcess->start("unrar", QStringList() << "x" << mRarFile << tmpDirectoryPath);
    mExtractProcess->waitForFinished();

    // extract finished
    log(QString("Finished <b>Extracting file</b>: %1").arg(mRarFile));

    return true;
}

bool NARWorker::renameDirectory(QString &path, QString &baseWorkingDir)
{
    if (!QFileInfo(path).isDir()) {
        log(QString("ERROR: the given string is not a dir: %1").arg(path));
        return false;
    }

    QDir directory(path);
    QFileInfoList list = directory.entryInfoList();
    foreach (QFileInfo fInfo, list) {
        if (fInfo.isDir()) {
            if (fInfo.fileName().startsWith("."))
                continue;
            else {

                // if extracted dir starts with www.new..., rename!
                QString albumDirName = fInfo.fileName();
                albumDirName.replace("www.NewAlbumReleases.net ","", Qt::CaseInsensitive);
                albumDirName.replace("www.NewAlbumReleases.net_","", Qt::CaseInsensitive);
                albumDirName.replace(QRegExp("\\(([0-9]{4})\\)"), "- \\1");
                QString albumPath(fInfo.path() + "/" + albumDirName);
                directory.rename(fInfo.absoluteFilePath(),
                                 albumPath);

                renameDirectoryContent(albumPath);

                directory.rename(albumDirName, baseWorkingDir + albumDirName);
            }
        }
    }

    log("Finished <b>rename</b>");
    return true;
}

bool NARWorker::renameDirectoryContent(QString &path)
{
    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();

    Album *album = new Album(path);


    foreach (QFileInfo fInfo, list) {
        if (fInfo.fileName().startsWith("."))
            continue;
 /*       if (fInfo.isDir()) {
            renameDirectoryContent(fInfo.absoluteFilePath());
            continue;
        }*/
        if (fInfo.fileName().endsWith(".url",Qt::CaseInsensitive)) {
            dir.remove(fInfo.absoluteFilePath());
            continue;
        }
        if (fInfo.fileName().endsWith(".mp3", Qt::CaseInsensitive)) {
            QString fPath = fInfo.path();
            QString fileName = fInfo.fileName();
            fileName.replace("www.NewAlbumReleases.net ","", Qt::CaseInsensitive);
            fileName.replace("www.NewAlbumReleases.net_","", Qt::CaseInsensitive);
            dir.rename(fInfo.absoluteFilePath(), fPath+"/"+fileName);
            fixId3Tags(fileName, album);
        }
    }
    log("Finished <b>Fixing ID3 Tags</b>");

    return true;
}

void NARWorker::fixId3Tags(QString &fileName, Album *album)
{
    QString seperator = " - ";
    QString fileExt = ".mp3";
    int sepPos = fileName.indexOf(seperator, 0);
    QString titleNumber = fileName.left(sepPos);
    // int fileExtPos = fileName.indexOf(fileExt, sepPos+seperator.length());

    int startAt = sepPos+seperator.length();
    QString titleName = fileName.mid(startAt, fileName.length()-(fileExt.length()+startAt));

    //qDebug() << fileName.length() << " - " << fileExt.length() << " - " << fileName.length()-fileExt.length();
    //qDebug() << titleNumber << " : " << titleName;

    QString qFilePath = QString("%1/%2").arg(album->dirPath()).arg(fileName);
    TagLib::FileRef fRef(qFilePath.toUtf8().data(), TagLib::String::UTF8);
    fRef.tag()->setArtist(QStringToTString_(album->albumArtist()));
    fRef.tag()->setAlbum(QStringToTString_(album->albumTitle()));
    fRef.tag()->setTrack(titleNumber.toUInt());
    fRef.tag()->setTitle(QStringToTString_(titleName));
    fRef.tag()->setYear(album->releaseYear().toUInt());
    fRef.tag()->setComment("");

    fRef.save();

}

void NARWorker::log(const QString &msg)
{
    emit logMessage(mThreadId, msg);
}

