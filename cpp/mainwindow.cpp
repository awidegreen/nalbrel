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

#include <QModelIndexList>
#include <QModelIndex>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDir>

#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "album.h"


const QString MainWindow::Organisation = "widegreen.de";
const QString MainWindow::ProgramName  = "NewAlbumReleases Rename";

/*
 *-----------------------------------------------------------------------------------------------
 */

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->extractingProgressBar->reset();

    filesModel = new QStringListModel(this);
    QStringList list;
    filesModel->setStringList(list);
    ui->filesListView->setModel(filesModel);
    mSelectionModel = ui->filesListView->selectionModel();
    mQueueManager.addQueueListener(this);

    readSettings();
    mQueueManager.setAvaiableSlots(ui->threadsSpinBox->value());

    connect(ui->fileLoggingCheckBox,SIGNAL(clicked()),this, SLOT(updateUI()));
    connect(ui->moveToTargetDirectoryCheckBox,
            SIGNAL(clicked()), this, SLOT(updateUI()));
}

/*
 *-----------------------------------------------------------------------------------------------
 */

MainWindow::~MainWindow()
{
    writeSettings();
    delete filesModel;
    delete mSelectionModel;
    delete ui;
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_addButton_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Add New Album Release File"),
                                                mAddFileDefaultPath,
                                                tr("www.NewAlbumReleases.net-Rar-File (*.rar)"));
    if (!fileName.isNull() && !fileName.isEmpty()) {
        mAddFileDefaultPath = fileName.left(fileName.lastIndexOf("/"));
        qDebug() << "setted mAddFileDefaultPath to:" << mAddFileDefaultPath;
    }
    addFileToListModel(fileName);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_addFolderButton_clicked()
{
    QStringList dirNames;
    QFileDialog d(this, tr(""), mAddFolderDefaultPath, tr(""));
    d.setFileMode(QFileDialog::DirectoryOnly);
    if (d.exec())
        dirNames = d.selectedFiles();

    if (dirNames.size() > 0) {
       mAddFolderDefaultPath = dirNames.at(0);
       qDebug() << "setted mAddFolderDefaultPath to:" << mAddFolderDefaultPath;
    }

    addFolderContentToListModel(dirNames);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_renameAllButton_clicked()
{
    QStringList files = filesModel->stringList();

    startRenameJobs(files);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::logOutput(const QString& msg) {
    ui->logTextEdit->append(msg + "\n");
    qDebug() << msg;
}


/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::startRenameJobs(const QStringList &files)
{
    ui->extractingProgressBar->reset();
    foreach (QString s, files) {
        mQueueManager.addItem(s,ui->deleteRarCheckBox->isChecked());
    }

    if (!mQueueManager.start())
        return;
    ui->extractingProgressBar->setRange(0,mQueueManager.getNumberOfItems()-1);
    enableUiElements(false);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::jobFinished(const int id)
{
    int cnt = 0;
    bool fnd = false;
    QString value = mQueueManager.getValueForId(id);
    foreach (QString s, filesModel->stringList()) {
        if (s == value) {
            fnd = true;
            break;
        }
        cnt++;
    }

    if (fnd)
        filesModel->removeRow(cnt);
    else
        logOutput(QString("Could not remove enrty: %1").arg(value));

    int val = ui->extractingProgressBar->value()+1;
    ui->extractingProgressBar->setValue(val);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::queueMessage(const QString message)
{
    logOutput(QString("[LOG] %1").arg(message));
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::itemsRemains(const int amount)
{
    logOutput(QString("Items remaining: %1").arg(amount));
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::queueFinished()
{
    enableUiElements(true);
    QMessageBox::information(this,
                             "Jobs finsiehd",
                             "The jobs in the queue are done!");
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_threadsSpinBox_editingFinished()
{
    mQueueManager.setAvaiableSlots(ui->threadsSpinBox->value());
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::enableUiElements(bool enable)
{
    ui->renameAllButton->setEnabled(enable);
    ui->renameSelectedButton->setEnabled(enable);
    ui->deleteRarCheckBox->setEnabled(enable);
    ui->addButton->setEnabled(enable);
    ui->addFolderButton->setEnabled(enable);
    ui->deleteButton->setEnabled(enable);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_deleteButton_clicked()
{
    QModelIndexList indexes = mSelectionModel->selectedIndexes();
    QModelIndex index;

    foreach(index, indexes) {
        filesModel->removeRow(index.row());
    }
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::addFileToListModel(const QString &file)
{
    if (file.isNull() || file.isEmpty())
        return;

    if (file.contains("www.NewAlbumReleases.net", Qt::CaseInsensitive)) {
        QStringList list = filesModel->stringList();
        list.append(file);
        filesModel->setStringList(list);
    } else {
        QMessageBox::warning(this, tr("Selecting Error"),
                             tr("www.NewAlbumReleases.net rar-File selected"),
                             QMessageBox::Ok);
    }
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::addFolderContentToListModel(const QStringList &folders)
{
    foreach (QString dirName, folders) {
        QDir dir(dirName);
        QStringList filters("www.NewAlbumReleases.net*");
        QFileInfoList list = dir.entryInfoList(filters);
        foreach (QFileInfo fInfo, list) {
            if (fInfo.fileName().contains("part2.rar", Qt::CaseInsensitive))
                continue;
            if (fInfo.fileName().contains("part3.rar", Qt::CaseInsensitive))
                continue;
            if (fInfo.isDir())
                continue;
            addFileToListModel(fInfo.absoluteFilePath());
        }
    }

}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_renameSelectedButton_clicked()
{
    QModelIndexList indexes = mSelectionModel->selectedIndexes();
    QModelIndex index;

    QStringList selectedFiles;

    foreach(index, indexes) {
        selectedFiles.append(index.data().toString());
    }

    startRenameJobs(selectedFiles);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_action_Close_triggered()
{
    if (QMessageBox::question(this,
               "Exit program", "You are going to leave the program, right?",
                QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)
            == QMessageBox::Ok) {
        emit close();
    }
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this,
                       "About NewAlbumReleasesRename",
                       "Copyright by Armin Widegreen 2010");
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::readSettings()
{
    QSettings settings(MainWindow::Organisation, MainWindow::ProgramName);
    QPoint pos = settings.value("pos", QPoint(200,200)).toPoint();
    QSize size = settings.value("size", QSize(820,450)).toSize();
    QByteArray state = settings.value("state", QByteArray()).toByteArray();
    bool moveToTargetDirectoryEnabled =
            settings.value("moveToTargetDirectoryEnabled", QVariant(false)).toBool();
    QString moveToTargetDirectory =
            settings.value("moveToTargetDirectory", QString("~")).toString();
    bool fileLoggingEnabled =
            settings.value("fileLoggingEnabled", QVariant(false)).toBool();
    QString fileLoggingDirectory =
            settings.value("fileLoggingDirectory", QString("~")).toString();
    int amountWorkingThreads =
            settings.value("amountWorkingThreads", QVariant(4)).toInt();
    mAddFileDefaultPath =
            settings.value("mAddFileDefaultPath", QString("~")).toString();
    mAddFolderDefaultPath =
            settings.value("mAddFolderDefaultPath", QString("~")).toString();

    move(pos);
    resize(size);
    restoreState(state);
    ui->moveToTargetDirectoryCheckBox->setChecked(moveToTargetDirectoryEnabled);
    ui->targetDirectoryLineEdit->setText(moveToTargetDirectory);
    ui->fileLoggingCheckBox->setChecked(fileLoggingEnabled);
    ui->fileLoggingDirectoryLineEdit->setText(fileLoggingDirectory);
    ui->threadsSpinBox->setValue(amountWorkingThreads);

    updateUI();
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::writeSettings()
{
    QSettings settings(MainWindow::Organisation, MainWindow::ProgramName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.setValue("moveToTargetDirectoryEnabled", ui->moveToTargetDirectoryCheckBox->isChecked());
    settings.setValue("moveToTargetDirectory", ui->targetDirectoryLineEdit->text());
    settings.setValue("fileLoggingEnabled", ui->fileLoggingCheckBox->isChecked());
    settings.setValue("fileLoggingDirectory", ui->fileLoggingDirectoryLineEdit->text());
    settings.setValue("amountWorkingThreads", ui->threadsSpinBox->value());
    settings.setValue("mAddFileDefaultPath", mAddFileDefaultPath);
    settings.setValue("mAddFolderDefaultPath", mAddFolderDefaultPath);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

void MainWindow::updateUI()
{
    bool enalbed = ui->moveToTargetDirectoryCheckBox->isChecked();
    ui->targetDirectoryLineEdit->setEnabled(enalbed);

    enalbed = ui->fileLoggingCheckBox->isChecked();
    ui->fileLoggingDirectoryLineEdit->setEnabled(enalbed);
}

/*
 *-----------------------------------------------------------------------------------------------
 */

