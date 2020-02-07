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


class QInventorContext : public QObject
{
Q_OBJECT
public:
    QInventorContext(QQuickView *window);
    static void init();
    static QOpenGLContext *sharedOpenGLContext();

protected slots:
    void onSceneGraphInitialized();

protected:
    void timerEvent(QTimerEvent *event) override;

    static QOpenGLContext *theSharedOpenGLContext;
    static bool isShared;
    QQuickView *quickView;
};


#endif
