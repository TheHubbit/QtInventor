/**
 * \file
 * \brief      QQuickInventor class implementation.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


#include "QQuickInventorView.h"
#include <QOpenGLFunctions>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <QReadLocker>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoCamera.h>


QQuickInventorView::QQuickInventorView(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    m_scene = nullptr;

    m_sceneManager.setRenderCallback(renderCBFunc, this);

    connect(this, SIGNAL(widthChanged()), this, SLOT(onResize()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(onResize()));

    // Initialize size fo scene manager.
    onResize();
}

void QQuickInventorView::onResize()
{
    const SbVec2s sizeVec2s(short(size().width()), short(size().height()));
    if (m_sceneManager.getSize() != sizeVec2s)
    {
        m_sceneManager.setWindowSize(sizeVec2s);
        m_sceneManager.setSize(sizeVec2s);
    }
}

QQuickFramebufferObject::Renderer *QQuickInventorView::createRenderer() const
{
    return new InventorRenderer((QQuickInventorView*) this);
}

void QQuickInventorView::renderCBFunc(void *userdata, SoSceneManager *)
{
    QQuickInventorView* thisPtr = static_cast<QQuickInventorView*>(userdata);
    thisPtr->update();
}

void QQuickInventorView::mousePressEvent(QMouseEvent *event)
{
    QWriteLocker locker(QQuickInventorScene::mutex());

    SoMouseButtonEvent::Button button = event->button() & Qt::LeftButton ? SoMouseButtonEvent::BUTTON1 : SoMouseButtonEvent::BUTTON3;
    SoMouseButtonEvent buttonEvent;
    buttonEvent.setTime(SbTime::getTimeOfDay());
    buttonEvent.setPosition(SbVec2s(short(event->x()), short(event->y())));
    buttonEvent.setButton(button);
    buttonEvent.setState(SoMouseButtonEvent::DOWN);
    m_sceneManager.processEvent(&buttonEvent);
}

void QQuickInventorView::mouseReleaseEvent(QMouseEvent *event)
{
    QWriteLocker locker(QQuickInventorScene::mutex());

    SoMouseButtonEvent::Button button = event->button() & Qt::LeftButton ? SoMouseButtonEvent::BUTTON1 : SoMouseButtonEvent::BUTTON3;
    SoMouseButtonEvent buttonEvent;
    buttonEvent.setTime(SbTime::getTimeOfDay());
    buttonEvent.setPosition(SbVec2s(short(event->x()), short(event->y())));
    buttonEvent.setButton(button);
    buttonEvent.setState(SoMouseButtonEvent::UP);
    m_sceneManager.processEvent(&buttonEvent);
}

void QQuickInventorView::mouseMoveEvent(QMouseEvent *event)
{
    QWriteLocker locker(QQuickInventorScene::mutex());

    SoLocation2Event moveEvent;
    moveEvent.setTime(SbTime::getTimeOfDay());
    moveEvent.setPosition(SbVec2s(short(event->x()), short(event->y())));
    m_sceneManager.processEvent(&moveEvent);
}

void QQuickInventorView::setScene(QQuickInventorScene *scene)
{
    QWriteLocker locker(QQuickInventorScene::mutex());

    m_scene = scene;

    m_sceneManager.setSceneGraph(m_scene->scene());
    m_sceneManager.scheduleRedraw();
}

QQuickInventorScene* QQuickInventorView::scene() const
{
    return m_scene;
}

void QQuickInventorView::viewAll()
{
    QWriteLocker locker(QQuickInventorScene::mutex());

    SoSearchAction searchAction;
    searchAction.setType(SoCamera::getClassTypeId());
    searchAction.apply(m_sceneManager.getSceneGraph());
    if (searchAction.getPath())
    {
        SoCamera* camera = (SoCamera*) searchAction.getPath()->getTail();
        camera->viewAll(m_sceneManager.getSceneGraph(), m_sceneManager.getViewportRegion());
    }
}


QQuickInventorView::InventorRenderer::InventorRenderer(QQuickInventorView *item)
{
    m_quickItem = item;
}

void QQuickInventorView::InventorRenderer::render()
{
    // Make sure that Inventor scene isn't changes while being rendered.
    QWriteLocker locker(QQuickInventorScene::mutex());

    // This call keeps track if already initialized.
    initializeOpenGLFunctions();

    // Need to unbind any current shader program. Save and restore
    // just to make sure not interferring with Qt's rendering.
    GLint currentShaderPRogram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShaderPRogram);

    // Setup OpenGL state to what Inventor expects.
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);

    SoSceneManager &sm = m_quickItem->m_sceneManager;
    // Don't expect current OpenGL state matches Inventor state after last traversal.
    sm.getGLRenderAction()->invalidateState();
    sm.render();

    glUseProgram(currentShaderPRogram);

    // see: https://www.qt.io/blog/2015/05/11/integrating-custom-opengl-rendering-with-qt-quick-via-qquickframebufferobject
    m_quickItem->window()->resetOpenGLState();
}

QOpenGLFramebufferObject *QQuickInventorView::InventorRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);

    return new QOpenGLFramebufferObject(size, format);
}
