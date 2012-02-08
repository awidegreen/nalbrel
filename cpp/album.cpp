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

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>

#include "album.h"

Album::Album()
{
}

Album::Album(QString albumDirPath) :
        path(albumDirPath)
{
    int lastSlash = path.lastIndexOf("/");
    QString str = path.right(path.length()-lastSlash-1);

    int startAt = 0;
    int counter = 1;
    int end = 0;
    bool isFinished = false;
    QString seperator = " - ";

    while (!isFinished)
    {
        end = str.indexOf(seperator, startAt);
        if (end == -1) {
            end = str.length();
            isFinished = true;
        }

        QString token = str.mid(startAt, end-startAt);
        switch (counter) {
            case 1:  // artist
            artist = token;
            break;

            case 2:  // ablum-title
            title = token;
            break;

            case 3:  // release-year
            year = token;
            break;

            default:
            break;
        }

        startAt = end+seperator.length();
        counter++;
    }

    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();
    titles = 0;
    foreach (QFileInfo fInfo, list) {
        if (fInfo.fileName().startsWith("."))
            continue;
        if (fInfo.isDir()) {
            continue;
        }
        if (fInfo.fileName().endsWith(".url",Qt::CaseInsensitive)) {
            continue;
        }
        titles++;
    }

    qDebug() << artist << " : " << title << " : " << year << " : " << titles;
}
