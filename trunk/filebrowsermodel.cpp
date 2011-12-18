/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2, or (at your option)  any later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
** Street - Fifth Floor, Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include <QMainWindow>
#include <QDir>
#include "filebrowsermodel.h"
#include <qdebug.h>

FileBrowserModel::FileBrowserModel(QObject *parent) :
    QAbstractListModel(parent),
    _parent(parent)
{
    QHash<int, QByteArray> roles;
    roles[TITLE] = "title";
    roles[IMAGE] = "image";
    roles[IS_FILE] = "file";
    roles[PATH] = "path";
    setRoleNames(roles);

    _currentDir = QDir::homePath();
}

void FileBrowserModel::changeCurrentDir(int index)
{
    if (index >= _dirs.count()) {
        return;
    }
    if (index == 0 && _dirs[index].startsWith(tr("Go"))) {
        _currentDir += "/..";
        QDir dir = QDir(_currentDir);
        _currentDir = dir.absolutePath();
    } else {
        _currentDir += "/" +  _dirs[index];
    }
    qDebug() << _currentDir;
    searchPdfFiles();
    reset();
}

void FileBrowserModel::searchPdfFiles()
{
    _pdfFiles.clear();
    _dirs.clear();

    QDir directory = QDir(_currentDir, "*.pdf *.PDF *.Pdf *.pDf *.pdF *.PDf *.PdF *.pDF",
                          QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

    //fill file list
    directory.setFilter(QDir::Files);
    foreach (QString file, directory.entryList()) {
        _pdfFiles.append(directory.absoluteFilePath(file));
    }
    _pdfFiles.append(tr(CLOSE_FILE_BROWSER_TEXT));

    //fill folder list
    directory.setFilter(QDir::AllDirs);
    foreach (QString file, directory.entryList()) {
        QString dirToAdd;
        QString absPath = directory.absoluteFilePath(file);
        if (file == "." || !QDir(absPath).isReadable())
            continue;
        if (file == "..") {
            QDir dir = QDir(absPath);
            if (!dir.isRoot()) {
                dirToAdd =
                        tr("Go Back To '") + QDir(dir.canonicalPath()).dirName() + "'";
            } else {
                dirToAdd = tr("Go Back To /");
            }
        } else {
            dirToAdd = QDir(absPath).dirName();
        }
        _dirs.append(dirToAdd);
    }
}

int FileBrowserModel::rowCount(const QModelIndex&) const
{
    return (_dirs.count() + _pdfFiles.count());
}

QVariant FileBrowserModel::data(const QModelIndex &index, int role) const
{

    int dirRow =  index.row();

    bool isFile = false;
    if (dirRow >=  _dirs.count()) {
        isFile = true;
    }

    if (isFile) {
        int fileRow = index.row() - _dirs.count();
        switch (role) {
        case TITLE:
            return QDir(_pdfFiles[fileRow]).dirName();
        case IMAGE:
            if (((fileRow+1) == _pdfFiles.count()) &&
                    (tr(CLOSE_FILE_BROWSER_TEXT) == _pdfFiles[fileRow]))
            {
                return QString(":/filebrowser/icons/Apps-session-quit-icon.png");
            } else
            {
                return QString(":/filebrowser/icons/Adobe-PDF-Document-icon.png");
            }
        case IS_FILE:
            return 1;
        case PATH:
            return _pdfFiles[fileRow];
        }
    } else {
        switch (role) {
        case TITLE:
            return _dirs[dirRow];
        case IMAGE:
            if (dirRow == 0 && _dirs[dirRow].startsWith(tr("Go"))) {
                return QString(":/filebrowser/icons/Button-Upload-icon.png");
            } else {
                return QString(":/filebrowser/icons/My-Ebooks-icon.png");
            }
        case IS_FILE:
            return 0;
        case PATH:
            return _dirs[dirRow];
        }
    }

    return QVariant();
}
