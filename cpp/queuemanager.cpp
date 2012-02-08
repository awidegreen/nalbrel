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

#include "queuemanager.h"

#include <QDebug>

#include <narworker.h>

/*
 *-----------------------------------------------------------------------------------------------
 */

QueueManager::QueueManager() :
        mRunning(false),
        mAvaiableSlots(0)
{
}

/*
 *-----------------------------------------------------------------------------------------------
 */

QueueManager::~QueueManager()
{

}

/*
 *-----------------------------------------------------------------------------------------------
 */


void QueueManager::addItem(NARWorker* item)
{
    mItemsQueue.enqueue(item);
}

/*
 *-----------------------------------------------------------------------------------------------
 */


void QueueManager::addItem(const QString &fileName, const bool deleteAfterWork)
{
    int id = qrand() % 10000;
    NARWorker* item = new NARWorker(id, this, fileName, deleteAfterWork);
    mItemsQueue.enqueue(item);
    mItemsModel.insert(id, fileName);
}

/*
 *-----------------------------------------------------------------------------------------------
 */


void QueueManager::addQueueListener(IQueueListener* listener)
{
    mListeners.append(listener);
}

/*
 *-----------------------------------------------------------------------------------------------
 */


bool QueueManager::start()
{
    if (mAvaiableSlots == 0 || mItemsQueue.empty())
    {
        return false;
    }

    mRunning = true;

    notifyListener("Queue Start");
    while (mItemsRunning.size() < mAvaiableSlots && !mItemsQueue.empty())
    {
        NARWorker* item = mItemsQueue.dequeue();
        int id = item->getId();
        mItemsRunning.insert(id, item);
        item->startJob();
    }

    foreach (IQueueListener* listener, mListeners)
    {
        listener->itemsRemains(mItemsQueue.size());
    }

    return true;
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::stop()
{
    notifyListener("Going to stop all running jobs");
    foreach (NARWorker* item, mItemsRunning.values())
    {
        //item->stopWorking();
    }

    mRunning = false;
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::reset()
{
    mItemsModel.clear();
    mItemsQueue.clear();
    mItemsRunning.clear();
    mRunning = false;
    mAvaiableSlots = 0;

    notifyListener("QueueManager reseted!");
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::notifyListener(const QString& msg) const
{
    foreach (IQueueListener* listener, mListeners)
    {
        listener->queueMessage(msg);
    }
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::setAvaiableSlots(const int avaiableSlots)
{
    mAvaiableSlots = avaiableSlots;
    if (mRunning) start();
}

/*
 *-----------------------------------------------------------------------------------------------
 */

const QString QueueManager::getValueForId(const int id) const
{
    return mItemsModel.value(id, "");
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::logMessage(const int id, const QString message)
{
    QString s = QString("Job %1: %2").arg(id).arg(message);
    notifyListener(s);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void QueueManager::jobFinished(const int id)
{
    logMessage(id, "<b>Finished<b/> Job!");
    mItemsRunning.remove(id);

    foreach (IQueueListener* listener, mListeners)
    {
        listener->jobFinished(id);
    }

    if (mItemsQueue.size() == 0 && mItemsRunning.size() == 0) {
        mRunning = false;
        foreach (IQueueListener* listener, mListeners)
        {
            listener->queueFinished();
        }
    } else {
        if (mRunning)
            start();
    }
}

