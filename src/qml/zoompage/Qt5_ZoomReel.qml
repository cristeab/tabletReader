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

    // Zoom item dimensions
    property int zoomWidth: width
    property int zoomHeight: height
    // Font properties
    property string fontName: 'Helvetica'
    property int fontSize: 22
    property color fontColor: "#666666"
    // Spacing between items
    property int spacing: 8
    // zoom index
    property int zoomIndex: 0
    //model for the reel element
    property alias zoomModel: zoom.model

    signal setZoomFactor(int index)

    property Component itemBackground: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button.png"
        }
    }
    property Component itemBackgroundPressed: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button_pressed.png"
        }
    }

    width: 240
    height: 60

    Component {
      id: zoomDelegate
      Qt5_Button {
          width: container.zoomWidth
          height: container.zoomHeight
          text: number
          fontColor: container.fontColor
          fontName: container.fontName
          fontSize: container.fontSize
          bg: itemBackground
          bgPressed: itemBackgroundPressed
          onClicked: {
              zoom.index = index
              zoom.toggle()
              if (true == zoom.clip) {
                setZoomFactor(index)
              }
          }
      }
    }

    Qt5_Reel {
        id: zoom
        width: container.zoomWidth
        height: container.zoomHeight
        //model: zoomfactors
        delegate: zoomDelegate
        autoClose: false
        index:  zoomIndex
    }
}
