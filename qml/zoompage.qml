import QtQuick 1.0
import "zoompage" as Comp
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

    Rectangle {
        id: container

        width: 105
        height: 260
        color: "transparent"
        anchors.centerIn: parent

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
            objectName: "zoomreel"
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
