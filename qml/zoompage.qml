import QtQuick 1.0
import "zoompage" as Comp

Rectangle {
    id: container

    width: 105
    height: 260
    color: "transparent"

    Rectangle {
        width: container.width
        height: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -48
        radius: 10
        smooth: true
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "darkgray" }
        }
        Text {
            id: titledarkblue
            width: container.width
            height: 16
            color:  "darkblue"
            text: "Zoom Factor"
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: 16
        }
    }

    Comp.ZoomReel {
        id: dateReel
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
    }
}
