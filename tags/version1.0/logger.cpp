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

#include <QDir>
#include <QDateTime>
#include <stdlib.h>
#include <QDebug>
#include "logger.h"

QTextStream Logger::ts_;
QFile *Logger::pOutFile_ = NULL;

Logger::Logger(const QString &fileLogName)
{
    if (NULL != pOutFile_)
    {
        delete pOutFile_;
    }
    pOutFile_ = new QFile(QDir::homePath()+QDir::separator()+
                          QDir::toNativeSeparators(fileLogName));
    pOutFile_->open(QIODevice::WriteOnly | QIODevice::Append);
    ts_.setDevice(pOutFile_);
    qInstallMsgHandler(debugMessageHandler);
    qDebug() << "\n\nSTART" << QDateTime::currentDateTime().toString(Qt::ISODate)
             << "START\n";
}

Logger::~Logger()
{
    delete pOutFile_;
    pOutFile_ = NULL;
}

void Logger::debugMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    ts_ << QDateTime::currentDateTime().toMSecsSinceEpoch()
       << " " << txt << endl;
}
