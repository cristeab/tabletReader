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

#include <QApplication>
#include <QTranslator>
#include "window.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    
#ifndef QT_NO_DEBUG_OUTPUT
    //in release mode the log file is not created
    Logger log("tabletReader.log");
#endif
    //translation object
    QTranslator translator;
    if (false == translator.load("tabletReader_" + QLocale::system().name()))
    {
        qDebug() << "cannot load translation file" << QLocale::system().name();
    }
    app.installTranslator(&translator);
    //main window
    Window wnd;
    wnd.show();
    return app.exec();
}
