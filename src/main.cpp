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

#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "QInventorContext.h"
#include "QQuickInventorView.h"


int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

#if 0
    // Using QQuickView
    QQuickView view;
    QInventorContext inventorContext(&view);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();
#else
    // Using QQmlApplicationEngine
    QQmlApplicationEngine engine;
    QInventorContext inventorContext(&engine);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
#endif

    return app.exec();
}
