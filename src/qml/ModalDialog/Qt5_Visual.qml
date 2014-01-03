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

Item {
    property string defaultFontFamily: "Tahoma"
    property int defaultFontSize: 14
    property color defaultFontColor: "#ffffff"
    property color defaultFontColorButton: "#000000"
    property color defaultBackgroundColor: "#d9d3b5"

    property Component buttonComponent: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button.png"
        }
    }

    property Component buttonPressedComponent: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button_pressed.png"
        }
    }
}
