import QtQuick 1.1

Rectangle {
    id: window
    width: 360
    height: 360
    color: "#000000"

    property real time: 0

    NumberAnimation {
           id: timeAnimation
           target: window
           properties: "time"
           from: 0
           to: 1.0
           loops: Animation.Infinite
           duration: 60000
           running: true
      }

    Rectangle {
        id: aspectRatioRect
        color: "#000000"

        property double ratio: 1.3197665
        width: Math.min(parent.width, parent.height * ratio)
        height: Math.min(parent.height, parent.width / ratio)

        Image {
            id: background
            source: "graphic/hill_background.svg"
            anchors.fill: parent
        }

        Moon {
            id: moon
            x: window.time*100;
            y:0;
        }

        Image {
            id: foreground
            source: "graphic/hill_foreground.svg"
            x: 0
            anchors.fill: parent
        }

        TextInput {
            x:10;
            y:10;
            width: 100;
            height: 100;
        }
    }



}
