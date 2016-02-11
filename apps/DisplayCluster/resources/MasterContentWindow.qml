import QtQuick 2.0
import DisplayCluster 1.0
import "qrc:/qml/core/."
import "qrc:/qml/core/style.js" as Style

BaseContentWindow {
    id: windowRect
    color: "#80000000"

    function closeWindow() {
        displaygroup.removeWindowLater(contentwindow.id)
    }

    function toggleControlsVisibility() {
        if(contentwindow.isPanel)
            return
        contentwindow.controlsVisible = !contentwindow.controlsVisible
    }

    function toggleFocusMode() {
        if(contentwindow.isPanel)
            return
        if(contentwindow.focused)
            displaygroup.unfocus(contentwindow.id)
        else
            displaygroup.focus(contentwindow.id)
    }

    MultiTouchGestureArea
    {
        id: windowMoveAndResizeArea

        onTapStarted: displaygroup.moveContentWindowToFront(contentwindow.id)

        onTapEnded: {
            contentwindow.state = ContentWindow.NONE
            toggleControlsVisibility()
        }

        onPan: {
            contentwindow.state = ContentWindow.MOVING
            contentwindow.controller.moveTo(Qt.point(contentwindow.x + dx,
                                                     contentwindow.y + dy))
        }

        onPanEnded: contentwindow.state = ContentWindow.NONE

        onDoubleTap: toggleFocusMode()

        PinchWheelArea{
            onPinchChanged: {
                contentwindow.state = ContentWindow.RESIZING
                contentwindow.controller.scale(Qt.point(px,py),pixelDelta)
            }
        }
    }

    MultiTouchGestureArea {
        id: contentInteractionArea

        visible: contentwindow.state === ContentWindow.SELECTED
        focus: true // to receive key events

        anchors.bottom: parent.bottom
        anchors.bottomMargin: windowRect.border.width
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - windowRect.widthOffset
        height: parent.height - windowRect.heightOffset

        function removeOffset(position) {
            // C++ interaction delegates don't have any knowledge of the title
            // bar offset applied to this contentInteractionArea. Gesture
            // positions must be passed without the offset.
            return Qt.point(position.x, position.y - parent.yOffset);
        }

        onDoubleTap: toggleFocusMode()
        onTapStarted: {
            displaygroup.moveContentWindowToFront(contentwindow.id)
            contentwindow.delegate.touchBegin(removeOffset(Qt.point(x,y)))
        }
        onTapAndHold: contentwindow.delegate.tapAndHold(removeOffset(Qt.point(x,y)))
        onPan: contentwindow.delegate.pan(Qt.point(x,y), Qt.point( dx, dy))

        PinchWheelArea{
            onPinchChanged: contentwindow.delegate.pinch( parent.removeOffset(
                                                             Qt.point(px,py)),
                                                             pixelDelta)
        }
    }

    ContentWindowButton {
        source: "qrc:///img/master/close.svg"
        anchors.top: parent.top
        anchors.right: parent.right
        mousearea.onClicked: closeWindow()
    }

    ContentWindowButton {
        source: "qrc:///img/master/maximize.svg"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        mousearea.onClicked: toggleFocusMode()
        visible: !contentwindow.isPanel
    }

    ContentWindowButton {
        source: "qrc:///img/master/resize.svg"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        visible: !contentwindow.isPanel && !contentwindow.focused

        property variant startMousePos
        property variant startSize
        mousearea.onPressed: {
            startMousePos = Qt.point(mouse.x, mouse.y)
            startSize = Qt.size(contentwindow.width, contentwindow.height)
            contentwindow.state = ContentWindow.RESIZING
        }
        mousearea.onPositionChanged: {
            if(!mousearea.pressed)
                return
            var newSize = Qt.size(mouse.x - startMousePos.x + startSize.width,
                                  mouse.y - startMousePos.y + startSize.height)
            contentwindow.controller.resize(newSize)
        }
        mousearea.onReleased: contentwindow.state = ContentWindow.NONE
    }

    WindowBorders {
        borderDelegate: touchBorderDelegate

        Component {
            id: touchBorderDelegate
            BorderRectangle {
                MultiTouchGestureArea
                {
                    onTapStarted: {
                        contentwindow.border = parent.border
                        contentwindow.state = ContentWindow.RESIZING
                    }

                    onPanEnded: {
                        contentwindow.border = ContentWindow.NOBORDER
                        contentwindow.state = ContentWindow.NONE
                    }

                    onTapEnded: {
                        contentwindow.border = ContentWindow.NOBORDER
                        contentwindow.state = ContentWindow.NONE
                    }

                    onPan: contentwindow.controller.resizeRelative(Qt.point(dx, dy))
                }
            }
        }
    }

    WindowControls {
        listview.delegate: buttonDelegate
        listview.header: fixedButtonsDelegate

        Component {
            id: fixedButtonsDelegate
            Column {
                CloseControlButton {
                    MultiTouchGestureArea
                    {
                        anchors.fill: parent
                        onTapStarted: closeWindow()
                    }
                }
                OneToOneControlButton {
                    MultiTouchGestureArea
                    {
                        anchors.fill: parent
                        onTapStarted: contentwindow.controller.adjustSizeOneToOne()
                    }
                }
                FocusControlButton {
                    MultiTouchGestureArea
                    {
                        anchors.fill: parent
                        onTapStarted: toggleFocusMode()
                    }
                }
            }
        }

        Component {
            id: buttonDelegate
            WindowControlsDelegate {
                MultiTouchGestureArea
                {
                    anchors.fill: parent
                    onTapStarted: action.trigger()
                }
            }
        }
    }

    Text {
        visible: !parent.titleBar.visible
        text: contentwindow.label
        font.pixelSize: 48
        width: Math.min(paintedWidth, parent.width)
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 10
    }
}
