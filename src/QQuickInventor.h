/**
 * \file
 * \brief      QQuickInventor class declaration.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */

#ifndef QQuickInventorView_H
#define QQuickInventorView_H

#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickView>
#include <QWriteLocker>
#include <Inventor/SoSceneManager.h>


class QQuickInventor : public QQuickFramebufferObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    static constexpr const char* OpenGLContextProperty = "InventorOpenGLContext";

    QQuickInventor(QQuickItem *parent = 0);
    Renderer *createRenderer() const override;
    QOpenGLFramebufferObject *fbo() const;
    QReadWriteLock *fboLock();

protected slots:
    void onResize();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    static void renderCBFunc(void *userdata, SoSceneManager *mgr);
    virtual void renderIvScene();
    virtual void createAndBindFramebufferObject(const QSize &size);

    QOpenGLFramebufferObject *frameBufferObject;
    SoSceneManager sceneManager;
    QReadWriteLock frameBufferObjectLock;

    class InventorRenderer : public QQuickFramebufferObject::Renderer
    {
    public:
        InventorRenderer();

        // No additional work for render(). Framebuffer is copied from scene graph in synchronize().
        void render() override;
        void synchronize(QQuickFramebufferObject *quickObject) override;
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    protected:
        virtual void copyFramebufferContent(QOpenGLFramebufferObject *source, QOpenGLFramebufferObject *target);
        QOpenGLFramebufferObject *syncFramebufferObject;
        QReadWriteLock *syncFramebufferObjectLock;
    };
};

#endif
