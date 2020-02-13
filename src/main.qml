/**
 * \file
 * \brief      QtInventor example application QML.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtInventor 1.0


Item {
    id: root
    width: 800
    height: 400

    InventorScene {
        id: cubeScene
        text: "
#Inventor V2.1 ascii

Separator {
  DirectionalLight {
  }
  OrthographicCamera {
    position 0 0 3.1500392
    nearDistance 0
    farDistance 6.3000784
    focalDistance 3.1500392
    height 6.3000784
  }
  TrackballManip {
  }
  ShapeKit {
    appearance
    AppearanceKit {
      material
      Material {
        diffuseColor 1.0 0.2 0.2
      }
    }
  }
}
"
    }

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(0, root.height)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "black" }
        }
    }

    GridLayout {
        id: grid
        columns: 2
        width: parent.width
        height: parent.height

        InventorRenderer {
            id: renderer1
            Layout.fillWidth: true
            Layout.fillHeight: true
            scene: cubeScene
        }
        InventorRenderer {
            id: renderer2
            Layout.fillWidth: true
            Layout.fillHeight: true
            scene: cubeScene
        }
    }

    Rectangle {
        id: labelFrame
        anchors.margins: -10
        radius: 5
        color: "white"
        border.color: "black"
        opacity: 0.8
        anchors.fill: label
    }

    Text {
        id: label
        anchors.bottom: grid.bottom
        anchors.left: grid.left
        anchors.right: grid.right
        anchors.margins: 20
        wrapMode: Text.WordWrap
        text: "The cube and rotate manipulator are part of an Open Inventor scene graph that is rendered in the main application thread. The resulting image is then rendered on the Qt Quick scene graph rendering thread using the QQuickInventor class, which is derived from QQuickFramebufferObject."
    }
}

