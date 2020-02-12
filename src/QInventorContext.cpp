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
#include "QQuickInventorView.h"
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>


QOpenGLContext *QInventorContext::theSharedOpenGLContext = 0;
bool QInventorContext::isShared = false;


void QInventorContext::init()
{
    // Register QML type for viewing Inventor scenes.
    qmlRegisterType<QQuickInventorView>("QtInventor", 1, 0, "InventorRenderer");

    // Initialize scene object database.
    SoDB::init();
    SoInteraction::init();
}

QOpenGLContext *QInventorContext::sharedOpenGLContext()
{
    return isShared ? theSharedOpenGLContext : 0;
}

QInventorContext::QInventorContext(QQuickView *window)
{
    init();
    quickView = window;

    // Create a new OpenGL context to render Coin scenes in Gui thread.
    quickView->setPersistentOpenGLContext(true);
    theSharedOpenGLContext = new QOpenGLContext(this);
    theSharedOpenGLContext->setFormat(quickView->requestedFormat());
    theSharedOpenGLContext->create();

    // Add shared OpenGL context property to window. This is used by QQuickCoinView to render Inventor scenes.
    QVariant v = qVariantFromValue(theSharedOpenGLContext);
    m_quickView->setProperty(QQuickInventorView::OpenGLContextProperty, v);

    // Share OpenGL resources once Quick SG is initialized.
    // Use direct connection to ensure that we are called from QSGRenderThread (required for making context current).
    connect(quickView, SIGNAL(sceneGraphInitialized()), this, SLOT(onSceneGraphInitialized()), Qt::DirectConnection);

    // Start timer for queue processing.
    startTimer(10);
}

void QInventorContext::onSceneGraphInitialized()
{
    // source: https://forum.qt.io/topic/24701/solved-creating-a-qquickview-with-a-shared-opengl-context
    quickView->setPersistentOpenGLContext(true);
    quickView->openglContext()->blockSignals(true);
    quickView->openglContext()->doneCurrent();
    quickView->openglContext()->setShareContext(theSharedOpenGLContext);
    quickView->openglContext()->setFormat(quickView->requestedFormat());
    quickView->openglContext()->create();
    quickView->openglContext()->makeCurrent(quickView);
    quickView->openglContext()->blockSignals(false);

    isShared = true;
}

void QInventorContext::timerEvent(QTimerEvent *event)
{
    SoDB::getSensorManager()->processTimerQueue();
    SoDB::getSensorManager()->processDelayQueue(TRUE);
}
