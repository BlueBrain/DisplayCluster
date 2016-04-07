/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.4
import Qt.labs.folderlistmodel 2.0

Rectangle {
    id: fileBrowser
    anchors.fill: parent
    anchors.margins: 5

    color: "red"

    property string folder
    property variant nameFilters
    property int buttonHeight: parent.height / 12
    property int itemHeight: parent.height / 6

    signal itemSelected(string file)

    function selectFile( file )
    {
        if (file !== "")
            itemSelected(file)
    }

    Rectangle
    {
        id: inner
        anchors.fill: parent
        color: "black"

        property bool showFocusHighlight: false
        property color textColor: "white"

        function down(path)
        {
            folders.folder = path;
        }

        function up()
        {
            var path = folders.parentFolder;
            if (path.toString().length === 0 || path.toString() === 'file:')
                return;
            folders.folder = path;
        }

        FolderListModel
        {
            id: folders
            nameFilters: fileBrowser.nameFilters
            folder: fileBrowser.folder
            showDirsFirst: true
            showDotAndDotDot: false
        }

        SystemPalette
        {
            id: palette
        }

        Component
        {
            id: fileDelegate
            Rectangle
            {
                id: wrapper
                width: folderImage.width
                height: folderImage.height
                color: "transparent"

                Item {
                    id: folderImage
                    width: itemHeight
                    height: itemHeight

                    Image {
                        id: folderPicture
                        source: "image://folderimages/" + filePath
                        width: itemHeight * 0.9
                        height: itemHeight * 0.9
                        fillMode: Image.PreserveAspectFit
                        anchors.horizontalCenter: folderImage.horizontalCenter
                        anchors.verticalCenter: folderImage.verticalCenter
                    }
                }

                Text
                {
                    id: nameText
                    text: fileName.length > 15 ? fileName.substr(0,15) + "..." : fileName
                    font.bold: true
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: folderImage.horizontalCenter
                    color: inner.textColor
                    font.pixelSize: itemHeight / 12
                }

                MouseArea
                {
                    id: mouseRegion
                    anchors.fill: parent
                    onPressed:
                    {
                        wrapper.GridView.view.currentIndex = index;
                    }
                    onClicked:
                    {
                        wrapper.color = "blue"
                        animateColor.start()
                        var path = "file://";
                        if (filePath.length > 2 && filePath[1] === ':')
                            path += '/';

                        path += filePath;

                        if (folders.isFolder(index))
                            inner.down(path);
                        else
                            fileBrowser.selectFile(path.replace("file://", ""))

                    }
                }
                PropertyAnimation { id: animateColor;
                                    target: wrapper;
                                    easing.type: Easing.Linear
                                    properties: "color";
                                    to: "transparent";
                                    duration: 1000 }
            }
        }

        GridView
        {
            id: view
            anchors.top: titleBar.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: parent.width
            cellWidth: itemHeight * 1.2
            cellHeight: itemHeight * 1.2
            model: folders
            delegate: fileDelegate
            focus: true
        }

        Rectangle
        {
            id: titleBar
            width: parent.width
            height: buttonHeight + 10
            anchors.top: parent.top
            color: "black"

            Rectangle
            {
                width: parent.width;
                height: buttonHeight
                color: "#212121"
                anchors.margins: 5
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                radius: buttonHeight / 15

                Rectangle
                {
                    id: upButton
                    width: buttonHeight
                    height: buttonHeight
                    color: "transparent"
                    Image
                    {
                        width: buttonHeight
                        height: buttonHeight
                        anchors.centerIn: parent
                        source: "qrc:/staticimages/images/up.png"
                    }
                    MouseArea
                    {   id: upRegion;
                        anchors.centerIn: parent
                        width: buttonHeight
                        height: buttonHeight
                        onClicked: inner.up()
                    }
                    states: [
                        State
                        {
                            name: "pressed"
                            when: upRegion.pressed
                            PropertyChanges { target: upButton; color: palette.highlight }
                        }
                    ]
                }

                Text
                {
                    id: titleText
                    anchors.left: upButton.right; anchors.right: parent.right; height: parent.height
                    anchors.leftMargin: 5; anchors.rightMargin: 5
                    color: "white"
                    text: folders.folder.toString().replace("file://", "")
                    font.pixelSize: buttonHeight / 4
                    horizontalAlignment: Text.AlignHLeft;
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
