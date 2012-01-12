import Qt 4.7
import "toolbar.js" as ToolbarJS

Item {
    width: parent.width
    height: 64

    // Make children of this Item to be the children of contentArea
    default property alias views: contentArea.children

    property int current: 0
    property int minimumItemHeight: toolbarImage.height
    property int minimumItemWidth: toolbarImage.height

    signal sendCommand(string cmd)

    // A black background for the item to prevent the white
    // background from showing in any situation
    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    BorderImage {
        id: toolbarImage
        source: ":/toolbar/qml/Toolbar/pics/toolbaritem.png"
        width: parent.width

        height: toolbar.height
        border.left: 10; border.top: 10
        border.right: 10; border.bottom: 10
    }

    // Flickable area for the toolbar items
    Flickable {
        id: toolbarItemArea
        anchors.bottom: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: toolbar.minimumItemHeight

        flickableDirection: "HorizontalFlick"
        contentWidth: toolbarItems.width
        contentHeight: toolbarItems.height
        boundsBehavior: Flickable.StopAtBounds

        Row {
            id: toolbarItems

            // Make toolbarItems fill all available horizontal space
            property int itemWidth: ToolbarJS.calcTabWidth()

            Repeater {
                // As many tabs as the parent has children
                id: toolbarItemList
                model: views.length

                delegate: ToolbarItem {
                    //width: toolbarItems.itemWidth > contentWidth ? toolbarItems.itemWidth : contentWidth
                    width: toolbarItems.itemWidth
                    height: toolbar.minimumItemHeight
                    onDoAction: {
                        sendCommand(btnText)
                    }
                }
            }
        }
    }

    // Child objects (i.e. toolbar view contents)
    Item {
        id: contentArea
        width: toolbar.width
        anchors.top: parent.top;
        anchors.bottom: toolbarItemArea.top
    }
}
