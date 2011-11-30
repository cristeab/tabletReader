import Qt 4.7
//import "Lists"
import "Toolbar"
//import "Scrollable"
//import "text.js" as TextJS

Rectangle {
    width: 360
    height: 64

    Toolbar {
        id: toolbar

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
