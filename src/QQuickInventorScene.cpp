/**
 * \file
 * \brief      QQuickInventorScene class implementation.
 * \author     Thomas Moeller
 * \details
 *
 * Copyright (C) the QtInventor contributors. All rights reserved.
 * This file is part of QtInventor, distributed under the BSD 3-Clause
 * License. For full terms see the included COPYING file.
 */


#include "QQuickInventorScene.h"

QQuickInventorScene::QQuickInventorScene(QQuickItem *parent) : QQuickItem(parent)
{
    m_rootNode = nullptr;
}

void QQuickInventorScene::releaseResources()
{
    if (m_rootNode)
    {
        m_rootNode->unref();
        m_rootNode = nullptr;
    }

    QQuickItem::releaseResources();
}

void QQuickInventorScene::setText(const QString &str)
{
    readAll(str.toUtf8());
    m_sceneString = str;
}

QString QQuickInventorScene::text() const
{
    return m_sceneString;
}

void QQuickInventorScene::setFile(const QString &str)
{
    QFile file(str);
    if (file.open(QFile::ReadOnly))
    {
        readAll(file.readAll());
        m_file = str;
    }
}

QString QQuickInventorScene::file() const
{
    return m_file;
}

void QQuickInventorScene::readAll(const QByteArray &buffer)
{
    SoInput in;
    in.setBuffer(buffer.data(), size_t(buffer.size()));
    if (in.isValidBuffer())
    {
        setScene(SoDB::readAll(&in));
        m_file = "";
        m_sceneString = "";
    }
}

void QQuickInventorScene::setScene(SoSeparator *scene)
{
    if (m_rootNode != scene)
    {
        if (m_rootNode)
        {
            m_rootNode->unref();
        }
        m_rootNode = scene;
        if (m_rootNode)
        {
            m_rootNode->ref();
        }

        emit sceneChanged();
    }
}

SoSeparator* QQuickInventorScene::scene() const
{
    return m_rootNode;
}
