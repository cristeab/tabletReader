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

import "../ModalDialog" as Modal

// This item provides waiting indicator, should some long -taking operation be in
// progress. Effectively WaitIndicator just shows some graphics, which rolls on the UI.
//
// Usage: Bind whatever model.loading -property etc. with the rotator's show property.
Item {
    id: rotator

    // The width and height are meant to be replaced by the parent, e.g. by
    // using anchors.fill. So remember to modify these when instantiating
    // a ModalNote. This size defines also the dimming & key capture area.
    // If no dimensions are defined, it'll just use S³ default resolution.
    width: 360
    height: 640

    // Defines whether or not the RoTaToR is shown.
    property bool show: false
    // Default WaitIndicator image, user should set the correct one set when
    // instantiating the WaitIndicator.
    property string image: "gfx/rotation_icon.png"
    // Defines the size of the rolling graphics.
    property int imageWidth: 128
    property int imageHeight: 128
    // Defines how long to wait before showing the wait indicator.
    property int delay: 1000

    // Use opacity to hide/show the indicator. Also animate the enterance/exit
    // by using Behaviour on opacity.
    opacity: 0
    Behavior on opacity { PropertyAnimation { duration: 250 } }

    // Use the Fader to dim the background a bit and bind it also to
    // WaitIndicator's "show" -property.
    Modal.Qt5_Fader {
        fadingOpacity: 0.4
        // Bind our visibility status with Fader show/hide states.
        state: rotator.opacity == 1 ? "faded" : ""
        transitionDuration: 250
	// Use the whole available area for fading & capturing key clicks.
        anchors.fill: parent
        // Use "light" fading color.
        color: "#f5f5f5"
    }

    // Nice, symmetric graphics that roll on the screen.
    Image {
        id: rotatorImg
        source: image
        width: imageWidth
        height: imageHeight
        anchors.centerIn: parent
        smooth: true

        NumberAnimation on rotation {
              running: rotator.show
              from: 0; to: 360
              loops: Animation.Infinite;
              duration: 1000
          }
    }
    // Allow a small delay before displaying the rotator.
    Timer {
        interval: rotator.delay
        running: rotator.show
        repeat: false
        onTriggered: opacity = 1
    }

    onShowChanged: {
        // Handle rotator exit.
        if(show == false) opacity = 0
    }
}
