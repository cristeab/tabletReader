import QtQuick 1.0
import "ModalDialog" as Comp

Rectangle {
    id: container

    width: 360
    height: 640
    color: "transparent"

    signal closeAbout

    // Visual is an item that defines some constants for the application
    // look and feel, e.g. the font sizes & colors etc.
    Comp.Visual {
        id: visual
    }

    // Example on using ModalDialog component.
    Comp.ModalDialog {
        id: dialog
        objectName: "aboutDialog"

        // Shown text can be set by modifying "text" property.
        text: "NOT SET"
        // Always remember to define the size for the dialog.
        anchors.fill: parent

        // Demonstrating how one could keep application wide variables saved
        // in one place and use them conveniently all around the application.
        fontName: visual.defaultFontFamily
        fontColor: visual.defaultFontColor
        fontColorButton: visual.defaultFontColorButton
        fontSize: visual.defaultFontSize
        showCancelButton: false
        state: "show"

        onAccepted: {
            closeAbout()
        }
    }
}
