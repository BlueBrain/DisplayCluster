import QtQuick 1.1
import DisplayCluster 1.0
import DisplayClusterApp 1.0
import "qrc:/qml/core/."
import "qrc:/qml/core/style.js" as Style

BaseContentWindow {
    objectName: "MasterContentWindow"
    color: "#80000000"
    border.width: 10

    function closeWindow() {
        displaygroup.removeWindowLater(contentwindow.id)
    }

    function toggleControlsVisibility() {
        contentwindow.controlsVisible = !contentwindow.controlsVisible
    }

    function toggleFocusMode() {
        if(contentwindow.label === "Dock")
            return
        if(contentwindow.focused)
            displaygroup.unfocus(contentwindow.id)
        else
            displaygroup.focus(contentwindow.id)
    }

    TouchMouseArea {
        id: windowMoveAndResizeArea

        visible: !contentInteractionArea.visible
        anchors.fill: parent

        onDoubleTap: toggleFocusMode()
        onTouchBegin: displaygroup.moveContentWindowToFront(contentwindow.id)
        onTouchEnd: contentwindow.state = ContentWindow.NONE
        onTap: toggleControlsVisibility()
        onPan: {
            contentwindow.state = ContentWindow.MOVING
            contentwindow.controller.moveTo(Qt.point(contentwindow.x + delta.x,
                                                     contentwindow.y + delta.y))
        }
        onPinch: {
            contentwindow.state = ContentWindow.RESIZING
            contentwindow.controller.scale(position, pixelDelta)
        }
    }

    TouchMouseArea {
        id: contentInteractionArea

        visible: contentwindow.state === ContentWindow.SELECTED
        focus: true // to receive key events

        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height - (titleBar.visible ? titleBar.height : 0)

        function removeOffset(position) {
            // C++ interaction delegates don't have any knowledge of the title
            // bar offset applied to this contentInteractionArea. Gesture
            // positions must be passed without the offset.
            return Qt.point(position.x, position.y - parent.yOffset);
        }

        onDoubleTap: toggleFocusMode()
        onTouchBegin: {
            displaygroup.moveContentWindowToFront(contentwindow.id)
            contentwindow.delegate.touchBegin(removeOffset(position))
        }
        onTouchEnd: contentwindow.delegate.touchEnd(removeOffset(position))
        onTap: contentwindow.delegate.tap(removeOffset(position))
        onTapAndHold: contentwindow.delegate.tapAndHold(removeOffset(position))
        onPan: contentwindow.delegate.pan(removeOffset(position), delta)
        onPinch: contentwindow.delegate.pinch(removeOffset(position), pixelDelta)
        onSwipeLeft:contentwindow.delegate.swipeLeft()
        onSwipeRight: contentwindow.delegate.swipeRight()
        onSwipeUp: contentwindow.delegate.swipeUp()
        onSwipeDown: contentwindow.delegate.swipeDown()
        onKeyPress: contentwindow.delegate.keyPress(key, modifiers, text)
        onKeyRelease: contentwindow.delegate.keyRelease(key, modifiers, text)
    }

    WindowBorders {
        borderDelegate: touchBorderDelegate

        Component {
            id: touchBorderDelegate
            BorderRectangle {
                TouchArea {
                    anchors.fill: parent
                    // Ideally a double tap should emit touchEnd but this changes behaviour
                    // for pixelstreamer.
                    onDoubleTap: {
                        contentwindow.border = ContentWindow.NOBORDER
                        contentwindow.state = ContentWindow.NONE
                    }
                    onTouchBegin: {
                        contentwindow.border = parent.border
                        contentwindow.state = ContentWindow.RESIZING
                    }
                    onPan: {
                        contentwindow.controller.resizeRelative(delta)
                    }
                    onTouchEnd: {
                        contentwindow.border = ContentWindow.NOBORDER
                        contentwindow.state = ContentWindow.NONE
                    }
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
                    TouchMouseArea {
                        anchors.fill: parent
                        onTap: closeWindow()
                    }
                }
                OneToOneControlButton {
                    TouchMouseArea {
                        anchors.fill: parent
                        onTap: contentwindow.controller.adjustSizeOneToOne()
                    }
                }
                FocusControlButton {
                    TouchMouseArea {
                        anchors.fill: parent
                        onTap: toggleFocusMode()
                    }
                }
            }
        }

        Component {
            id: buttonDelegate
            WindowControlsDelegate {
                TouchMouseArea {
                    anchors.fill: parent
                    onTap: action.trigger()
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
    }

    ContentWindowButton {
        source: "qrc:///img/master/resize.svg"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        visible: contentwindow.state !== ContentWindow.SELECTED

        property variant startMousePos
        property variant startSize
        mousearea.onPressed: {
            startMousePos = Qt.point(mouse.x, mouse.y)
            startSize = Qt.size(contentwindow.width, contentwindow.height)
            contentwindow.state = ContentWindow.RESIZING
        }
        mousearea.onPositionChanged: {
            var newSize = Qt.size(mouse.x - startMousePos.x + startSize.width,
                                  mouse.y - startMousePos.y + startSize.height)
            contentwindow.controller.resize(newSize)
        }
        mousearea.onReleased: contentwindow.state = ContentWindow.NONE
    }
}
