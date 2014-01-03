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

import "Toolbar"

Rectangle {
  id: mainframe

  width: 800
  height: 64
  focus: true
  signal sendBtnCommand(string cmd)

  Qt5_Toolbar {
      id: toolbar
      objectName: "toolbar"
      anchors.centerIn: parent
      onSendCommand: {
        sendBtnCommand(cmd)
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Open")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-document-open-folder-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Favorites")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-bookmark-new-list-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
        toolbarItemTitle: (true == mediator.fullScr)?qsTr("Full Screen"):qsTr("Normal Screen")
        toolbarItemIcon: (true == mediator.fullScr)?"qrc:/toolbar/qml/Toolbar/pics/Actions-view-fullscreen-icon.svg":"qrc:/toolbar/qml/Toolbar/pics/Actions-view-close-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Go To Page")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-go-jump-locationbar-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Zoom")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-page-zoom-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Properties")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-document-properties-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: (true == mediator.hlpBck)?qsTr("Help"):qsTr("Back")
          toolbarItemIcon: (true == mediator.hlpBck)?"qrc:/toolbar/qml/Toolbar/pics/Actions-help-contents-icon.svg":"qrc:/toolbar/qml/Toolbar/pics/Actions-go-previous-view-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("About")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-help-about-icon.svg"
          clip: true
      }

      Qt5_ToolbarView {
          toolbarItemTitle: qsTr("Quit")
          toolbarItemIcon: "qrc:/toolbar/qml/Toolbar/pics/Actions-application-exit-icon.svg"
          clip: true
      }
  }
}
