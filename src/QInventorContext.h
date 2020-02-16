/**
 * \file
 * \brief      QInventorContext class declaration.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


#ifndef QInventorContext_H
#define QInventorContext_H

#include <QtGui/QOpenGLContext>
#include <QQuickView>
#include <QQmlApplicationEngine>


class QInventorContext : public QObject
{
Q_OBJECT
public:
    QInventorContext(QQuickWindow *window);
    QInventorContext(QQmlApplicationEngine *engine);

    static void init();

protected slots:
    void openglContextCreated(QOpenGLContext *context);
    void objectCreated(QObject *object, const QUrl &url);

protected:
    void attachToWindow(QQuickWindow *window);
    void timerEvent(QTimerEvent *event) override;

    QOpenGLContext *m_sharedOpenGLContext;
};


#endif
