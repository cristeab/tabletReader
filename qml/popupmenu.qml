import QtQuick 1.0
import "PopupMenu" as Comp

Item {
    id: container
    width: 200
    height: 325
   Comp.PopupList {
        id: list
        anchors.fill: parent
        objectName: "popuplist"
    }
}
