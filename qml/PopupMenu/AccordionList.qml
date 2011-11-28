import QtQuick 1.0

// Accordion list
Item {
    id: container

    // Default width
    width: 360
    // Default height
    height: 640
    // Subitem expansion duration
    property int animationDuration: 100
    // Subitem indentation
    property int indent: 20
    // Scrollbar width
    property int scrollBarWidth: 8
    // Background for list item
    property string bgImage: './gfx/list_item.png'
    // Background image for pressed list item
    property string bgImagePressed: './gfx/list_item_pressed.png'
    // Background image for active list item (currently not used)
    property string bgImageActive: './gfx/list_item_active.png'
    // Font properties for top level items
    property string headerItemFontName: "Helvetica"
    property int headerItemFontSize: 12
    property color headerItemFontColor: "black"

    signal itemClicked(string itemTitle)

    AccordionListModel {
        id: mainModel
    }

    ListView {
        id: listView
        height: parent.height
        anchors {
            left: parent.left
            right: parent.right
        }
        model: mainModel
        delegate: listViewDelegate
        focus: true
        spacing: 0
    }

    Component {
        id: listViewDelegate
        Item {
            id: delegate
            // Modify appearance from these properties
            property int itemHeight: 64

            // Flag to indicate if this delegate is expanded
            property bool expanded: false

            x: 0; y: 0;
            width: container.width
            height: headerItemRect.height

            // Top level list item.
            ListItem {
                id: headerItemRect
                x: 0; y: 0
                width: parent.width
                height: parent.itemHeight
                text: itemTitle
                onClicked: {
                    console.debug("Clicked: "+itemTitle)
                    itemClicked(itemTitle)
                }

                bgImage: container.bgImage
                bgImagePressed: container.bgImagePressed
                bgImageActive: container.bgImageActive
                fontName: container.headerItemFontName
                fontSize: container.headerItemFontSize
                fontColor: container.headerItemFontColor
                fontBold: true
            }
        }
    }
}
