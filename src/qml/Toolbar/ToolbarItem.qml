import QtQuick 1.1

Rectangle {
  id: toolbarButton
    property int margin: 20
    property int contentWidth: toolbarItemText.width + margin

    height: parent.minimumItemHeight
    color: "transparent"

    signal doAction(string btnText)

    // Selection highlight
    BorderImage {
        id: toolbarItemPressedImage
        source: "qrc:/toolbar/qml/Toolbar/pics/toolbaritem_p.svg"
        width: parent.width
        height: parent.height
        border.left: 10; border.top: 10
        border.right: 10; border.bottom: 10

        opacity: 0
    }

    // Icon and title
    Column {
        anchors.centerIn: parent
        Image {
            id: toolbarItemIcon
            width: 40
            height: 40

            property string iconSource: toolbar.views[index].toolbarItemIcon

            source: iconSource
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: toolbarItemText

            anchors.horizontalCenter: parent.horizontalCenter
            text: toolbar.views[index].toolbarItemTitle
            color: "black"
            style: "Normal"
            font.bold: true
            font.pointSize: 11
            width: toolbarButton.width
            horizontalAlignment: Text.AlignHCenter
            clip: true
            elide: Text.ElideRight
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: toolbarItemPressedImage
        onClicked: {
            doAction(toolbarItemText.text)
        }
    }

    states: [
        State {
            name: 'pressed'; when: mouseArea.pressed
            PropertyChanges { target: toolbarItemPressedImage; opacity: 1 }
        }
    ]
}
