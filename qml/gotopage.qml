/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import "Core"
import "Core/calculator.js" as CalcEngine

Rectangle {
    id: window

    width: 240; height: 300
    color: "transparent"

    function doOp(operation) { CalcEngine.doOperation(operation) }

    Item {
        id: main

        width: parent.width; height: parent.height; anchors.centerIn: parent

        Column {
            id: box; spacing: 8

            anchors { fill: parent; topMargin: 6; bottomMargin: 6; leftMargin: 6; rightMargin: 6 }

            Rectangle {
                width: box.width-3
                height: 20
                radius: 10
                smooth: true
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "white" }
                    GradientStop { position: 1.0; color: "darkgray" }
                }
                Text {
                    id: titledarkblue
                    width: box.width
                    height: 16
                    color:  "darkblue"
                    text: "GoTo Page"
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    font.pixelSize: 16
                }
            }

            Display {
                id: display
                objectName: "disp"
                width: box.width-3
                height: 56
                text: ""
            }

            Column {
                id: column; spacing: 6

                property real h: ((box.height - 90) / 4) - ((spacing * (4 - 1)) / 4)
                property real w: (box.width / 4) - ((spacing * (4 - 1)) / 4)

                Grid {
                    id: grid; rows: 4; columns: 3; spacing: 6

                    property real w: (box.width / columns) - ((spacing * (columns - 1)) / columns)

                    Button { width: grid.w; height: column.h; operation: "1"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "2"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "3"; color: 'blue' }

                    Button { width: grid.w; height: column.h; operation: "4"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "5"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "6"; color: 'blue' }

                    Button { width: grid.w; height: column.h; operation: "7"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "8"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "9"; color: 'blue' }

                    Button { width: grid.w; height: column.h; operation: "OK"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "0"; color: 'blue' }
                    Button { width: grid.w; height: column.h; operation: "DEL"; color: 'blue' }
                }
            }
        }
    }
}
