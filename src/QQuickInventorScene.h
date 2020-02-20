/**
 * \file
 * \brief      QQuickInventorScene class declaration.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


#ifndef QQUICKINVENTORSCENE_H
#define QQUICKINVENTORSCENE_H

#include <QObject>
#include <QQuickItem>
#include <QWriteLocker>
#include <Inventor/nodes/SoSeparator.h>


class QQuickInventorScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString file READ file WRITE setFile)
    Q_PROPERTY(SoSeparator* scene READ scene WRITE setScene)

public:
    explicit QQuickInventorScene(QQuickItem *parent = nullptr);

    static QReadWriteLock* mutex();

    void setFile(const QString &str);
    QString file() const;

    void setText(const QString &str);
    QString text() const;

    void readAll(const QByteArray &buffer);

    void setScene(SoSeparator *scene);
    SoSeparator* scene() const;

signals:
    void sceneChanged();

protected:
    virtual void initInventor();

    void timerEvent(QTimerEvent *event) override;
    void releaseResources() override;

    SoSeparator *m_rootNode;
    QString m_sceneString;
    QString m_file;
};

#endif // QQUICKINVENTORSCENE_H
