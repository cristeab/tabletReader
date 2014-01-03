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
import "WaitDialog" as Comp

Rectangle {
    id: container

    width: 360
    height: 640
    color: "transparent"

    // Wait indicator is visible by default
    Comp.Qt5_WaitIndicator {
        id: waitIndicator

        // Always define the size for the WaitIndicator.
        anchors.fill: parent

        // Start showing the WaitIndicator immediately.
        // Otherwise it'll wait for one second before starting the indicator.
        delay: 0

        // Make sure it's on top of everything, so that the
        // Fader will capture key clicks
        z: 120

        // In real application, the WaitIndicator could be bound to
        // some "loading" appState variable etc.
        show: true
    }
}
