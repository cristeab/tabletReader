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

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>

class QFile;
class QMutex;

class Logger
{
public:
  static Logger* instance(const QString &fileLogName);
private:
  Logger(const QString &fileLogName);
  ~Logger();
  Q_DISABLE_COPY(Logger);
#ifndef QT5
  static void debugMessageHandler(QtMsgType type, const char *msg);
#else
  static void debugMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
#endif
  QTextStream ts_;
  QFile *outFile_;
  QMutex *loggerMutex_;
};

#endif // LOGGER_H
