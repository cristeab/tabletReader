/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QApplication>
#include <signal.h>
#include "window.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

static QTextStream ts;
void debugMessageHandler(QtMsgType type, const char *msg)
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
        ts << QDateTime::currentDateTime().toMSecsSinceEpoch()
           << " " << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile outFile(QDir::homePath()+
                  QDir::toNativeSeparators("/tabletReader.log"));
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    ts.setDevice(&outFile);
    qInstallMsgHandler(debugMessageHandler);
    qDebug() << "\n\nSTART" << QDateTime::currentDateTime().toString(Qt::ISODate)
             << "START\n";

    Window wnd;
    wnd.show();
    return app.exec();
}
