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

#ifndef ALBUM_H
#define ALBUM_H


// Qt-classes
class QString;

class Album
{
public:
    Album();
    Album(QString albumDirPath);

    void setdirPath(QString dirPath) { path = dirPath; }
    QString dirPath() { return path; }

    void setReleaseYear(QString releaseYear) { year = releaseYear; }
    QString releaseYear() { return year; }

    void setAmoutTitles(int amountTitles) { titles = amountTitles; }
    int amountTitles() { return titles; }

    void setMusicGenre(QString musciGenre) { genre = musciGenre; }
    QString musicGenre() { return genre; }

    void setAlbumTitle(QString albumTitle) { title = albumTitle; }
    QString albumTitle() { return title; }

    void setAlbumArtist(QString albumArtist) { artist = albumArtist; }
    QString albumArtist() { return artist; }

private:
    QString path;
    QString year;
    int titles;
    QString genre;
    QString title;
    QString artist;
};

#endif // ALBUM_H
