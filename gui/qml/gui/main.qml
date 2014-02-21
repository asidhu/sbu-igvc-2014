// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: rectangle2
    width: 1024
    height: 1024
    color: "#5e5e5e"
    radius: 5
    visible: true
    border.width: 15
    border.color: "#000000"
    clip: false

    Rectangle {
        id: rectangle1
        x: 10
        y: 9
        width: 386
        height: 346
        color: "#5a5a5a"
        border.color: "#ffffff"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 9

        Text {
            id: text1
            x: 5
            y: 6
            text: qsTr("Robot Info")
            style: Text.Raised
            font.pixelSize: 15
            anchors.left: rectangle1.right
            anchors.leftMargin: -381
            anchors.top: parent.top
            anchors.topMargin: 6
    }

    Text {
        id: voltageLabel
        objectName: "voltageLabel"
        x: 5
        y: 29
        text: qsTr("Voltage: 24V")
        font.pixelSize: 12
    }

    Text {
        id: liveLabel
        objectName: "liveLabel"
        x: 5
        y: 53
        text: qsTr("Uptime: 0s")
        font.pixelSize: 12
    }

    Text {
        id: cpuLabel
        objectName: "cpuLabel"
        x: 5
        y: 76
        text: qsTr("CPU:")
        font.pixelSize: 17
    }

    ListView {
        id: cpuList
        objectName: "cpuList"
        x: 5
        y: 93
        width: 91
        height: 64
        boundsBehavior: Flickable.StopAtBounds
        interactive: true
        delegate: Item {
            x: 0
            height: 15
            Row {
                id: row1
                spacing: 10

                Text {
                    text: name
                   // font.bold: true
                }
            }
        }
        model: ListModel {
            ListElement {
                name: "Grey"
            }

            ListElement {
                name: "Red"
            }

            ListElement {
                name: "Blue"
            }

            ListElement {
                name: "Green"
            }
        }
    }
    }
}
