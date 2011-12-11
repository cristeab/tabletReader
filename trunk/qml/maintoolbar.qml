/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
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

import Qt 4.7

import "Toolbar"

Rectangle {
    width: 360
    height: 64

    Toolbar {
        id: toolbar
        objectName: "toolbar"

        ToolbarView {
            toolbarItemTitle: "Open"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Folder-Blue-Documents-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "Full Screen"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/window-full-screen-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "Go To Page"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/goto-page-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "Zoom"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/zoom-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "Help"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/help-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "About"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/About-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: "Exit"
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Button-Close-icon.png"
            clip: true
        }
    }
}
