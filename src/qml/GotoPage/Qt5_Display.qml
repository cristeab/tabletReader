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

import QtQuick 2.0

BorderImage {
    id: image

    signal setPage(string value)
    property alias text : displayText.text

    source: "qrc:/qml/qml/GotoPage/images/display.png"
    border { left: 10; top: 10; right: 10; bottom: 10 }

    Text {
        id: displayText
        anchors {
            right: parent.right; verticalCenter: parent.verticalCenter; verticalCenterOffset: -1
            rightMargin: 6; left: parent.left
        }
        font.pixelSize: parent.height * .6; text: "0"; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight
        color: "#343434"; smooth: true; font.bold: true
    }
}
