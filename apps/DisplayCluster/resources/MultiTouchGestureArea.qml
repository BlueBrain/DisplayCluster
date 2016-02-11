import QtQuick 2.0
import DisplayCluster 1.0
import "qrc:/qml/core/style.js" as Style

MultiPointTouchArea {

    signal tapAndHold(real x, real y)
    signal tapStarted(real x, real y)
    signal tapEnded()
    signal pan(real x, real y, real dx, real dy)
    signal panEnded()
    signal doubleTap()

    anchors.fill: parent

    minimumTouchPoints: 0
    maximumTouchPoints: 1
    mouseEnabled: true

    touchPoints: TouchPoint { id: p0; }

    property bool panning: false
    property bool secondRelease: false
    property int numberOfTap: 0

    property real startX: 0
    property real startY: 0
    property real previousOffsetX: 0
    property real previousOffsetY: 0
    // Needed because of QTBUG-44370. The "release" mouse event is sent 2 times.
    property bool pressed: false

    property QtObject obj2: Timer {
        id: tapAndHoldTimer
        interval: Style.tapAndHoldTiming
        running: false
        repeat: false
        onTriggered: {
            if(manhattanLengthCheck())
                tapAndHold(p0.x, p0.y)
        }
    }
    property QtObject obj: Timer {
        id: doubleTapTimer
        interval: Style.doubleTapTiming
        running: false
        repeat: false
        onTriggered: {
            numberOfTap = 0
        }
    }

    function manhattanLengthCheck()
    {
        return Math.abs(p0.startX - p0.x) + Math.abs(p0.startY - p0.y) < Style.touchPixelDelta
    }

    onPressed: {
        pressed = true

        startX = p0.x
        startY = p0.y

        tapStarted(p0.x, p0.y)
        tapAndHoldTimer.restart()
        if(numberOfTap == 0)
            doubleTapTimer.restart()

        numberOfTap += 1
    }
    onReleased: {
        if(!pressed)
            return
        pressed = false
        tapAndHoldTimer.stop()

        if(numberOfTap == 2)
        {
            doubleTapTimer.stop()
            numberOfTap = 0
            doubleTap()
        }
        else
        {
            if(panning)
            {
                previousOffsetX = 0
                previousOffsetY = 0
                panEnded()
            }
            else
                tapEnded()
        }
        panning = false
        previousOffsetX = 0
        previousOffsetY = 0
    }
    onUpdated: {
        panning = true

        //This check is needed because p0.startx is not initialized/updated for mouse events
        var offsetX = p0.startX !== 0.0 ? p0.x - p0.startX : p0.x - (startX - previousOffsetX)
        var offsetY = p0.startY !== 0.0 ? p0.y - p0.startY : p0.y - (startY - previousOffsetY)

        var dx = offsetX - previousOffsetX
        var dy = offsetY - previousOffsetY

        previousOffsetX = offsetX
        previousOffsetY = offsetY

        pan(p0.x, p0.y, dx, dy)
    }
}
