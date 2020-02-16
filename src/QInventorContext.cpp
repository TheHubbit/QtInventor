/**
 * \file
 * \brief      QInventorContext class implementation.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


#include <QTimer>
#include "QInventorContext.h"
#include "QQuickInventorScene.h"
#include "QQuickInventorView.h"
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>


void QInventorContext::init()
{
    if (!SoDB::isInitialized())
    {
        // Initialize scene object database.
        SoDB::init();
        SoInteraction::init();

        // Register QML types for creating and viewing Inventor scenes.
        qmlRegisterType<QQuickInventorScene>("QtInventor", 1, 0, "InventorScene");
        qmlRegisterType<QQuickInventorView>("QtInventor", 1, 0, "InventorRenderer");
    }
}

QInventorContext::QInventorContext(QQuickWindow *window)
{
    init();
    attachToWindow(window);
}

QInventorContext::QInventorContext(QQmlApplicationEngine *engine)
{
    init();

    if (engine->rootObjects().length() > 0)
    {
        if (QQuickWindow* window = dynamic_cast<QQuickWindow*>(engine->rootObjects().first()))
        {
            attachToWindow(window);
        }
    }
    else
    {
        connect(engine, SIGNAL(objectCreated(QObject*, const QUrl)), this, SLOT(objectCreated(QObject*, const QUrl)));
    }
}

void QInventorContext::objectCreated(QObject *object, const QUrl &url)
{
    if (QQuickWindow* window = dynamic_cast<QQuickWindow*>(object))
    {
        disconnect(sender(), SIGNAL(objectCreated(QObject*, const QUrl)), this, SLOT(objectCreated(QObject*, const QUrl)));
        attachToWindow(window);
    }
}

void QInventorContext::attachToWindow(QQuickWindow *window)
{
    // Create a new OpenGL context to render Inventor scenes in Gui thread.
    window->setPersistentOpenGLContext(true);
    m_sharedOpenGLContext = new QOpenGLContext(this);
    m_sharedOpenGLContext->setFormat(window->requestedFormat());

    // Add shared OpenGL context property to window. This is used by QQuickInventor to render Inventor scenes.
    QVariant v = qVariantFromValue(m_sharedOpenGLContext);
    window->setProperty(QQuickInventorView::OpenGLContextProperty, v);

    // Share OpenGL resources once Quick SG context has been created.
    connect(window, SIGNAL(openglContextCreated(QOpenGLContext*)), this, SLOT(openglContextCreated(QOpenGLContext *)));

    // Start timer for queue processing.
    startTimer(10);
}

void QInventorContext::openglContextCreated(QOpenGLContext *context)
{
    m_sharedOpenGLContext->setShareContext(context);
    m_sharedOpenGLContext->create();
}

void QInventorContext::timerEvent(QTimerEvent *event)
{
    SoDB::getSensorManager()->processTimerQueue();
    SoDB::getSensorManager()->processDelayQueue(TRUE);
}
