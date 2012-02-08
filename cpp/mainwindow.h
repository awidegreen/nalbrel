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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QStringToTString_( s ) TagLib::String(s.toUtf8().data(),TagLib::String::UTF8)


#include <QtGui/QMainWindow>
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QQueue>

#include "irenameworkinglistener.h"
#include "iqueuelistener.h"
#include "queuemanager.h"

namespace Ui
{
    class MainWindow;
}

class QStringList;

class MainWindow :
        public QMainWindow,
        public IQueueListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // inherit from IQueueListener
    void queueMessage(const QString message);
    void jobFinished(const int id);
    void itemsRemains(const int amount);
    void queueFinished();

private:
    static const QString Organisation;
    static const QString ProgramName;

    Ui::MainWindow *ui;
    QStringListModel *filesModel;
    QItemSelectionModel *mSelectionModel;
    QString mAddFileDefaultPath;
    QString mAddFolderDefaultPath;
    QueueManager mQueueManager;

    void addFileToListModel(const QString &file);
    void addFolderContentToListModel(const QStringList &folders);
    void startRenameJobs(const QStringList &files);
    void enableUiElements(bool enable);
    void logOutput(const QString& msg);

    void readSettings();
    void writeSettings();

private slots:
    void on_action_About_triggered();
    void on_action_Close_triggered();
    void on_renameSelectedButton_clicked();
    void on_deleteButton_clicked();
    void on_threadsSpinBox_editingFinished();
    void on_addFolderButton_clicked();
    void on_renameAllButton_clicked();
    void on_addButton_clicked();
    void updateUI();

};

#endif // MAINWINDOW_H
