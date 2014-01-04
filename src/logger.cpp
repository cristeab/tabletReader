/****************************************************************************
**
** Copyright (C) 2013, Bogdan Cristea. All rights reserved.
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
#include <QThread>
#include <QMutex>
#include <iostream>
#include "logger.h"

Logger* Logger::instance(const QString &fileLogName)
{
  static Logger instance(fileLogName);
  return &instance;
}

Logger::Logger(const QString &fileLogName) :
	outFile_(new QFile(QDir::homePath() + QDir::separator() +
	QDir::toNativeSeparators(fileLogName)))
	, loggerMutex_(new QMutex())
{
  if(false == outFile_->open(QIODevice::WriteOnly | QIODevice::Append)) {
    std::cerr << "Cannot open log file " << std::endl;
	return;
  }
  ts_.setDevice(outFile_);
#ifndef QT5
  qInstallMsgHandler(debugMessageHandler);
#else
  qInstallMessageHandler(debugMessageHandler);
#endif
  ts_ << "\n\n\t\t\t\tSTART on " << QDateTime::currentDateTime().toString("dd/MM/yyyy") << "\n\n";
}

Logger::~Logger()
{
  delete outFile_;
  delete loggerMutex_;
}
#ifndef QT5
void Logger::debugMessageHandler(QtMsgType type, const char *msg)
#else
void Logger::debugMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
#endif
{
  QMutexLocker locker(Logger::instance("")->loggerMutex_);
  QString txt;
  switch(type) {
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
  Logger::instance("")->ts_ << QDateTime::currentDateTime().toString("hh:mm:ss.zzz")
      << "\tTID: 0x" << QString::number(reinterpret_cast<qulonglong>(QThread::currentThreadId()), 16)
      << "\t" << txt << endl;
}
