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

#ifndef RENAMEWORKINGWRAPPER_H
#define RENAMEWORKINGWRAPPER_H

#include <QObject>

#include "irenameworkinglistener.h"

class RenameWorkingWrapper :
        public QObject,
        public IRenameWorkingListener
{
    Q_OBJECT
public:
    RenameWorkingWrapper(IRenameWorkingListener *renameListener,
                         QObject *parent = 0);

public slots:
    void extractFinished(QString message)
        { renameWrapperObj->extractFinished(message); }
    void renameFinished(QString message)
        { renameWrapperObj->renameFinished(message); }
    void fixId3TagFinished(QString message)
        { renameWrapperObj->fixId3TagFinished(message); }
    void workingFinished()
        { renameWrapperObj->workingFinished(); }

private:
    IRenameWorkingListener *renameWrapperObj;
};

#endif // RENAMEWORKINGWRAPPER_H
