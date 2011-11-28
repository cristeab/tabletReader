import QtQuick 1.0
import "PopupMenu" as Comp

Item {
    id: container
    width: 200
    height: 450 
   Comp.PopupList {
        id: list
        anchors.fill: parent
    }
}
