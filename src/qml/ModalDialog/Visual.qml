// Application visual style definition file. Used to define Button component LaF.
import QtQuick 1.1

Item {
    property string defaultFontFamily: "Tahoma"
    property int defaultFontSize: 14
    property color defaultFontColor: "#ffffff"
    property color defaultFontColorButton: "#000000"
    property color defaultBackgroundColor: "#d9d3b5"

    property Component buttonComponent: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button.png"
        }
    }

    property Component buttonPressedComponent: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "qrc:/qml/qml/zoompage/gfx/button_pressed.png"
        }
    }
}
