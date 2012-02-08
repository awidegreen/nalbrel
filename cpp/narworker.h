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

#ifndef WORKINGTHREAD_H
#define WORKINGTHREAD_H

#define QStringToTString_( s ) TagLib::String(s.toUtf8().data(),TagLib::String::UTF8)

#include <QThread>

// foward declaration
class QString;
class QProcess;
class Album;
class QueueManager;

class NARWorker : public QObject
{
    Q_OBJECT

private:
    QProcess *mExtractProcess;
    Album *mAlbum;
    QString mRarFile;
    bool mDeleteRarFileAfterwards;
    const int mThreadId;
    QueueManager* mQueueManager;
    QThread mThread;

    void log(const QString &msg);

protected:
    bool extract(QString &tmpDirectoryPath);
    bool renameDirectory(QString &path, QString &baseWorkingDir);
    bool renameDirectoryContent(QString &path);
    void fixId3Tags(QString &fileName, Album *album);

public:
    NARWorker(int threadId,
                  QueueManager* queueManager,
                  const QString &file,
                  bool deleteRarFileAfterwards);
    ~NARWorker();

    QString getRarFile() { return mRarFile; }


    int getId() const { return mThreadId; }
    void startJob() { mThread.start(); }

private slots:
    void start();

signals:
    void logMessage(int,QString);
    void finished(int);

};

#endif // WORKINGTHREAD_H
