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
import QtInventor 1.0


Item {
    width: 600
    height: 600

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(0, 600)
        gradient: Gradient {
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "black" }
        }
    }

    InventorRenderer {
        id: renderer
        anchors.fill: parent
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
        anchors.bottom: renderer.bottom
        anchors.left: renderer.left
        anchors.right: renderer.right
        anchors.margins: 20
        wrapMode: Text.WordWrap
        text: "The cube and rotate manipulator are part of an Open Inventor scene graph that is rendered in the main application thread. The resulting image is then rendered on the Qt Quick scene graph rendering thread using the QQuickInventor class, which is derived from QQuickFramebufferObject."
    }
}

