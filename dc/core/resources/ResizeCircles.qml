import QtQuick 2.0
import DisplayCluster 1.0
import "style.js" as Style

Rectangle {
    anchors.fill: parent
    color: "transparent"

    property alias resizeCirclesDelegate: repeater.delegate

    visible: !contentwindow.isPanel
             && contentwindow.controlsVisible
             && contentwindow.state !== ContentWindow.SELECTED
    opacity: Style.resizeCircleOpacity

    Repeater {
        id: repeater
        model: [ContentWindow.TOP_LEFT, ContentWindow.TOP,
                ContentWindow.TOP_RIGHT, ContentWindow.RIGHT,
                ContentWindow.BOTTOM_RIGHT, ContentWindow.BOTTOM,
                ContentWindow.BOTTOM_LEFT, ContentWindow.LEFT]
        delegate: ResizeCircle {
        }
    }
}
