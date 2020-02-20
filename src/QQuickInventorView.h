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
#include "QQuickInventorScene.h"


class QQuickInventorView : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickInventorScene* scene READ scene WRITE setScene USER true)

public:
    QQuickInventorView(QQuickItem *parent = 0);
    Renderer *createRenderer() const override;

    void setScene(QQuickInventorScene *s);
    QQuickInventorScene* scene() const;

    Q_INVOKABLE void viewAll();

signals:
    void sceneChanged();

protected slots:
    void onResize();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    static void renderCBFunc(void *userdata, SoSceneManager *mgr);

    SoSceneManager m_sceneManager;
    QQuickInventorScene *m_scene;

    class InventorRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
    {
    public:
        InventorRenderer(QQuickInventorView *item);

        void render() override;
        QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    protected:
        QQuickInventorView *m_quickItem;
    };
};

#endif
