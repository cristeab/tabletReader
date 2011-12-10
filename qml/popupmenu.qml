import QtQuick 1.0
import "PopupMenu" as Comp
import "ModalDialog" as Modal

Rectangle
{
    id: mainframe
    width: 800
    height: 600
    color: "transparent"

    Modal.Fader {
        id: fadeRect
        anchors.fill: parent
        state: "faded"
    }

    Item {
        id: container
        width: 200
        height: 325
        anchors.centerIn: parent
        Comp.PopupList {
            id: list
            anchors.fill: parent
            objectName: "popuplist"
        }
    }
}
