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

Rectangle {
    id: reel    

    property alias interactive: path.interactive
    property int index: 0
    property bool moving: false
    property ListModel model    
    property Component delegate
    property int itemsShown: 4
    property bool autoClose: true
    property alias closingDelay: clippingTimer.interval
    property int closingTimeout: 1000

    function open() { focus = true; clip = false }
    function close() { clip = true }
    function toggle() { clip ? open() : close() }
    function shiftZ(obj, delta) {
        if (typeof obj.z != 'undefined') obj.z += delta
        if (obj.parent) shiftZ(obj.parent, delta) // Set z recursively to parent
    }

    width: 100
    height: 100
    color: "transparent"
    clip: true
    // Close reel when the focus is lost
    onFocusChanged: if (!focus) close()
    // Bring to front if not clipped
    onClipChanged:  { clip ? shiftZ(reel, -500) : shiftZ(reel, 500) }

    onIndexChanged: path.currentIndex = reel.index

    PathView {
        id: path
        width: parent.width
        height: (pathItemCount-1)*parent.height
        pathItemCount: parent.itemsShown+1
        clip: true
        anchors.centerIn: parent
        model: parent.model
        delegate: reel.delegate

        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        highlightRangeMode: PathView.StrictlyEnforceRange
        focus: false

        path: Path {
            startX: path.x+path.width/2; startY: 1-reel.height/2
            PathLine {x: path.x+path.width/2; y: path.height+reel.height/2-1}
        }
        onMovementStarted: { reel.moving = true; clippingTimer.stop(); reel.open()}
        onMovementEnded: {
            if (reel.autoClose) {
                clippingTimer.restart();
            }
            reel.index = path.currentIndex;
            reel.moving = false;
        }

        Timer {
            id: clippingTimer
            repeat: false; interval: reel.closingTimeout;
            triggeredOnStart: false; onTriggered: reel.close()
        }        
    }
}
