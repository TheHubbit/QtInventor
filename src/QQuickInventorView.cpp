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
#include "QInventorContext.h"
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
#include <Inventor/nodes/SoCamera.h>


QQuickInventorView::QQuickInventorView(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    m_frameBufferObject = nullptr;
    m_scene = nullptr;

    m_sceneManager.setRenderCallback(renderCBFunc, this);

    QFile file(":/ExampleScene.iv");
    if (file.open(QFile::ReadOnly))
    {
        QByteArray sceneBuffer = file.readAll();

        SoInput in;
        in.setBuffer(sceneBuffer.data(), size_t(sceneBuffer.size()));
        m_sceneManager.setSceneGraph(SoDB::readAll(&in));
        m_sceneManager.scheduleRedraw();
    }

    connect(this, SIGNAL(widthChanged()), this, SLOT(onResize()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(onResize()));
}

void QQuickInventorView::onResize()
{
    const SbVec2s sizeVec2s(short(size().width()), short(size().height()));
    if (m_sceneManager.getSize() != sizeVec2s)
    {
        m_sceneManager.setWindowSize(sizeVec2s);
        m_sceneManager.setSize(sizeVec2s);
        renderIvScene();
    }
}

QQuickFramebufferObject::Renderer *QQuickInventorView::createRenderer() const
{
    return new InventorRenderer();
}

QOpenGLFramebufferObject *QQuickInventorView::fbo() const
{
    return m_frameBufferObject;
}

QReadWriteLock *QQuickInventorView::fboLock()
{
    return &m_frameBufferObjectLock;
}

void QQuickInventorView::renderCBFunc(void *userdata, SoSceneManager *)
{
    QQuickInventorView* thisPtr = static_cast<QQuickInventorView*>(userdata);
    thisPtr->renderIvScene();
}

void QQuickInventorView::createAndBindFramebufferObject(const QSize &size)
{
    if (m_frameBufferObject)
    {
        if (m_frameBufferObject->size() == size)
        {
            // Keep using already created framebuffer. Just bind it.
            m_frameBufferObject->bind();
            return;
        }

        delete m_frameBufferObject;
    }

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);

    m_frameBufferObject = new QOpenGLFramebufferObject(size, format);
    m_frameBufferObject->bind();

    // initializeOpenGLFunctions() checks if already initialized.
    initializeOpenGLFunctions();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void QQuickInventorView::renderIvScene()
{
    if (window())
    {
        QVariant v = window()->property(OpenGLContextProperty);
        if (QOpenGLContext *context = v.value<QOpenGLContext*>())
        {
            if (context->makeCurrent(window()))
            {
                QWriteLocker locker(&m_frameBufferObjectLock);
                createAndBindFramebufferObject(QSize(m_sceneManager.getSize()[0], m_sceneManager.getSize()[1]));
                m_sceneManager.render();
                m_frameBufferObject->release();
                context->doneCurrent();
            }

            // Trigger display update
            update();
        }
    }
}

void QQuickInventorView::mousePressEvent(QMouseEvent *event)
{
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
    SoLocation2Event moveEvent;
    moveEvent.setTime(SbTime::getTimeOfDay());
    moveEvent.setPosition(SbVec2s(short(event->x()), short(event->y())));
    m_sceneManager.processEvent(&moveEvent);
}

void QQuickInventorView::setScene(QQuickInventorScene *scene)
{
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
    SoSearchAction searchAction;
    searchAction.setType(SoCamera::getClassTypeId());
    searchAction.apply(m_sceneManager.getSceneGraph());
    if (searchAction.getPath())
    {
        SoCamera* camera = (SoCamera*) searchAction.getPath()->getTail();
        camera->viewAll(m_sceneManager.getSceneGraph(), m_sceneManager.getViewportRegion());
    }
}


QQuickInventorView::InventorRenderer::InventorRenderer()
{
    m_syncFramebufferObject = nullptr;
}

void QQuickInventorView::InventorRenderer::render()
{
    // Framebuffer content already updated in synchronize or createFramebufferObject.
}

void QQuickInventorView::InventorRenderer::synchronize(QQuickFramebufferObject *quickObject)
{
    QQuickInventorView *view = dynamic_cast<QQuickInventorView *>(quickObject);
    if (view)
    {
        // Update content during synchronize call when main thread is blocked.
        copyFramebufferContent(view->fbo(), framebufferObject());

        // Remember source so createFramebufferObject() can copy again in case of resize.
        m_syncFramebufferObject = view->fbo();
        m_syncFramebufferObjectLock = view->fboLock();
    }
}

QOpenGLFramebufferObject *QQuickInventorView::InventorRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject *fbo = new QOpenGLFramebufferObject(size, format);

    // Copy last content into newly created frmaebuffer (typically after resizing).
    if (m_syncFramebufferObjectLock && m_syncFramebufferObject)
    {
        QReadLocker locker(m_syncFramebufferObjectLock);
        copyFramebufferContent(m_syncFramebufferObject, fbo);
    }

    return fbo;
}

void QQuickInventorView::InventorRenderer::copyFramebufferContent(QOpenGLFramebufferObject *source, QOpenGLFramebufferObject *target)
{
    if (source && target)
    {
        QRect rect(0, 0, source->width(), source->height());
        QOpenGLFramebufferObject::blitFramebuffer(target, rect, source, rect, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
