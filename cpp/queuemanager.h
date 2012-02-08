/****************************************************************************************
 * Copyright (c) 2011 Armin Widegreen <armin.widegreen@gmail.com>                       *
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

#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QHash>
#include <QString>

#include <iqueuelistener.h>


class NARWorker;

class QueueManager : public QObject
{
    Q_OBJECT

public:
    QueueManager();
    ~QueueManager();

    void addItem(NARWorker* item);
    void addItem(const QString& fileName, const bool deleteAfterWork);
    void addQueueListener(IQueueListener* listener);
    bool start();
    void stop();
    void reset();

    int getNumberOfItems() const
    {
        return mItemsModel.size();
    }

    const QString getValueForId(const int id) const;

    // Setter
    void setAvaiableSlots(const int avaiableSlots);

private:
    QVector<IQueueListener*> mListeners;
    QQueue<NARWorker*> mItemsQueue;
    QHash<int, NARWorker*> mItemsRunning;
    QHash<int, QString> mItemsModel;
    bool mRunning;
    int mAvaiableSlots;

    void notifyListener(const QString& msg) const;

public slots:
    void logMessage(const int id, const QString message);
    void jobFinished(const int id);

};

#endif // QUEUEMANAGER_H
