import QtQuick 2.0
import DisplayCluster 1.0
import "qrc:/qml/core/."
import "qrc:/qml/core/style.js" as Style

DisplayGroup {
    id: dispGroup
    showFocusContext: false

    MultiTouchGestureArea {
        z: controlPanel.z - 1
        anchors.fill: parent
        onTapAndHold: view.backgroundTapAndHold(Qt.point(x, y))
    }

    controlPanel.buttonDelegate: Component {
        ControlPanelDelegate {
            id: touchControlPanel
            MultiTouchGestureArea {
                anchors.fill: parent
                onTapStarted: {
                    var action = touchControlPanel.ListView.view.model.get(index).action
                    var absPos = mapToItem(dispGroup, controlPanel.width
                                 + Style.panelsLeftOffset, 0)
                    var position = Qt.point(absPos.x, absPos.y)
                    cppcontrolpanel.processAction(action, position)
                }
            }
        }
    }
}
