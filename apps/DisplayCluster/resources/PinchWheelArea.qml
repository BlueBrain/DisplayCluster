import QtQuick 2.0
import DisplayCluster 1.0

Item {
    anchors.fill: parent

    signal pinchChanged(real px, real py, real pixelDelta)

    PinchArea {
        anchors.fill: parent

        property real startPixelSize: 0.0
        property real oldPixelSize: 0.0

        onPinchStarted: {
            startPixelSize = Math.sqrt((pinch.point1.x - pinch.point2.x) *
                                      (pinch.point1.x - pinch.point2.x) +
                                      (pinch.point1.y - pinch.point2.y) *
                                      (pinch.point1.y - pinch.point2.y))
            oldPixelSize = startPixelSize
        }

        onPinchUpdated: {
            var pixelSize = startPixelSize * pinch.scale
            pinchChanged(pinch.center.x, pinch.center.y, pixelSize - oldPixelSize )
            oldPixelSize = pixelSize
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton

        onWheel: pinchChanged( wheel.x, wheel.y, wheel.angleDelta.y / 10 )
    }

}
