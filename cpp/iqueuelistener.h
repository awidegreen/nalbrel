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

#ifndef IQUEUELISTENER_H
#define IQUEUELISTENER_H

#include <QString>

class IQueueListener
{
public:

    virtual ~IQueueListener() {}

    virtual void queueMessage(const QString message) = 0;
    virtual void jobFinished(const int id) = 0;
    virtual void itemsRemains(const int amount) = 0;
    virtual void queueFinished() = 0;
protected:
    IQueueListener() {}

};


#endif // IQUEUELISTENER_H
