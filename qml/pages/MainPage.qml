

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    objectName: "mainPage"
    allowedOrientations: Orientation.All

    PageHeader {
        objectName: "pageHeader"
        title: qsTr("SDL2 Example")
        extraContent.children: [
            IconButton {
                objectName: "aboutButton"
                icon.source: "image://theme/icon-m-about"
                anchors.verticalCenter: parent.verticalCenter

                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        ]
    }

    Column {
        anchors.centerIn: parent
        spacing: Theme.paddingLarge

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("SDL2 Integration Test")
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Check console output for SDL2 status")
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryColor
            wrapMode: Text.WordWrap
            width: parent.width - Theme.horizontalPageMargin * 2
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Test SDL2 Info")
            onClicked: {
                console.log("SDL2 test button clicked - check application log for SDL2 initialization info")
            }
        }
    }
}
