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

// This rectangle acts as a dimming rect. It also captures key presses.
Rectangle {
    id: faderRect

    // Fader area should always be explicitely set from calling parent who knows
    // how big area has to be faded and where the touch events need to be captured.
    // By default, use the S³ portrait resolution.
    width: 360
    height: 640

    // Fading colour can be defined, if e.g. a "lighter" fade is required.
    color: "black"

    // Used to define how fast the fade in / fade out will be done (in milliseconds).
    property int transitionDuration: 300
    // Change this, if you want to have deeper/lighter fade.
    property double fadingOpacity: 0.7

    // Initially hidden. Not really meant to be modified directly.
    opacity: 0

    // This rectangle captures all of the key events so that underlying buttons
    // etc. can't be pressed.
    Rectangle {
        id: keyCapturer

        MouseArea {
            id: mouseArea
            width: faderRect.width
            height: faderRect.height
            z: 100 // Make sure that this is on top.
            onClicked: {
                Qt.quit()
            }
        }
    }

    states: [
        State {
            name: ""    // This could actually be implicit...
            PropertyChanges {
                target: faderRect
                opacity: 0
            }
        },
        State {
            name: "faded"
            PropertyChanges {
                target: faderRect
                opacity: fadingOpacity
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "faded"
            SequentialAnimation {
                NumberAnimation { properties: "opacity"; easing.type: Easing.Linear; duration: transitionDuration }
            }
        },
        Transition {
            from: "faded"
            to: ""
            NumberAnimation { properties: "opacity"; easing.type: Easing.Linear; duration: transitionDuration }
        }
    ]
}
