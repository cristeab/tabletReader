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
    id: container

    property string buttonName: "NOT SET"
    property string target: "NOT SET"
    property string text: "NOT SET"

    property string fontName: "Helvetica"
    property int fontSize: 14
    property color fontColor: "black"

    property bool active: false

    property string bgImage: "gfx/button.png"
    property string bgImagePressed: "gfx/button_pressed.png"
    property string bgImageActive: "gfx/button_active.png"

    // These Component properties can be used to modify the Button's LaF
    // from the calling component. They have to define a BorderImage component.
    property Component bg: defaultBackground
    property Component bgPressed: defaultPressedBackground
    property Component bgActive: defaultActiveBackground

    signal clicked(string target, string button)

    width: 140
    height: 60
    opacity: enabled ? 1.0 : 0.5    

    Loader {
        id: background
        sourceComponent: container.bg
        anchors.fill: parent
    }

    Component {
        id: defaultBackground
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: bgImage
        }
    }
    Component {
        id: defaultPressedBackground
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: bgImagePressed
        }
    }

    Component {
        id: defaultActiveBackground
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: bgImageActive
        }
    }

    Text {
        id: buttonLabel
        text: parent.text
        wrapMode: Text.WordWrap

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        font {
            family: container.fontName
            pointSize: container.fontSize
        }
        color: container.fontColor
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: { focus = true; container.clicked(container.target, container.buttonName) }
    }

    states: [
        State {
            name: 'pressed'; when: mouseArea.pressed
            PropertyChanges { target: background; sourceComponent: container.bgPressed }
        },
        State {
            name: 'active'; when: container.active
            PropertyChanges { target: background; sourceComponent: container.bgActive; }
        }
    ]
}
