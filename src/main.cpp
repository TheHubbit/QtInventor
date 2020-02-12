/**
 * \file
 * \brief      Example main program.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include "QInventorContext.h"
#include "QQuickInventorView.h"


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    QQuickView view;
    QInventorContext inventorContext(&view);

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return app.exec();
}
