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


void QInventorContext::init()
{
    // Register QML type for viewing Inventor scenes.
    qmlRegisterType<QQuickInventorView>("QtInventor", 1, 0, "InventorRenderer");

    // Initialize scene object database.
    SoDB::init();
    SoInteraction::init();
}

QInventorContext::QInventorContext(QQuickView *window)
{
    init();
    m_quickView = window;

    // Create a new OpenGL context to render Inventor scenes in Gui thread.
    m_quickView->setPersistentOpenGLContext(true);
    m_sharedOpenGLContext = new QOpenGLContext(this);
    m_sharedOpenGLContext->setFormat(m_quickView->requestedFormat());
    m_sharedOpenGLContext->create();

    // Add shared OpenGL context property to window. This is used by QQuickInventor to render Inventor scenes.
    QVariant v = qVariantFromValue(m_sharedOpenGLContext);
    m_quickView->setProperty(QQuickInventorView::OpenGLContextProperty, v);

    // Share OpenGL resources once Quick SG is initialized.
    // Use direct connection to ensure that we are called from QSGRenderThread (required for making context current).
    connect(m_quickView, SIGNAL(sceneGraphInitialized()), this, SLOT(onSceneGraphInitialized()), Qt::DirectConnection);

    // Start timer for queue processing.
    startTimer(10);
}

void QInventorContext::onSceneGraphInitialized()
{
    // source: https://forum.qt.io/topic/24701/solved-creating-a-qquickview-with-a-shared-opengl-context
    m_quickView->setPersistentOpenGLContext(true);
    m_quickView->openglContext()->blockSignals(true);
    m_quickView->openglContext()->doneCurrent();
    m_quickView->openglContext()->setShareContext(m_sharedOpenGLContext);
    m_quickView->openglContext()->setFormat(m_quickView->requestedFormat());
    m_quickView->openglContext()->create();
    m_quickView->openglContext()->makeCurrent(m_quickView);
    m_quickView->openglContext()->blockSignals(false);
}

void QInventorContext::timerEvent(QTimerEvent *event)
{
    SoDB::getSensorManager()->processTimerQueue();
    SoDB::getSensorManager()->processDelayQueue(TRUE);
}
