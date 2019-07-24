/***
 * Copyright (C) Falko Axmann. All rights reserved.
 * Licensed under the MIT license.
 * See LICENSE.txt file in the project root for full license information.
 ****/

#include "QtScene.h"

#include <Scene/Qt/QtItem.h>
#include <Scene/Qt/QtItemTools.h>
#include <Spix/Data/ItemPath.h>

#include <QGuiApplication>
#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QtDebug>

namespace {

QQuickWindow* getQQuickWindowWithName(const std::string& name)
{
    QString qtName = QString::fromStdString(name);
    QQuickWindow* foundWindow = nullptr;

    auto windows = QGuiApplication::topLevelWindows();
    for (const auto& window : windows) {
        QQuickWindow* qquickWindow = qobject_cast<QQuickWindow*>(window);
        if (qquickWindow && (qquickWindow->objectName() == qtName)) {
            foundWindow = qquickWindow;
            break;
        }
    }

    return foundWindow;
}

static bool isInstanceOf(QObject* object, const QString& className)
{
    const QMetaObject* metaObject = object->metaObject();

    while (metaObject != nullptr) {
        if (metaObject->className() == className)
            return true;
        metaObject = metaObject->superClass();
    }

    return false;
}

QQuickItem* getQQuickItemWithRoot(const spix::ItemPath& path, QObject* root)
{
    if (path.length() == 0) {
        return nullptr;
    }
    if (!root) {
        return nullptr;
    }

    auto itemName = path.rootComponent();
    QObject* subItem = nullptr;

    if (itemName.compare(0, 1, ".") == 0) {
        QVariant propValue = root->property(itemName.c_str() + 1);
        if (propValue.isValid())
            subItem = propValue.value<QObject*>();
    } else {
        if (isInstanceOf(root, spix::qt::repeater_class_name)) {
            QQuickItem* repeater = static_cast<QQuickItem*>(root);
            subItem = spix::qt::RepeaterChildWithName(repeater, QString::fromStdString(itemName));
        } else if (isInstanceOf(root, spix::qt::listview_class_name) || isInstanceOf(root, spix::qt::gridview_class_name)) {
            QQuickItem* list = static_cast<QQuickItem*>(root);
            subItem = spix::qt::ListViewChildWithName(list, QString::fromStdString(itemName));
        } else {
            subItem = root->findChild<QObject*>(itemName.c_str());
        }
    }

    if (path.length() == 1) {
        return qobject_cast<QQuickItem*>(subItem);
    }
    
    /*
    if (!subItem)
    {
    	qDebug() << "Cannot find subitem: " << itemName.c_str();
    	for (QObject* child : root->children())
    	{
    		if (!child->objectName().isEmpty())
	    		qDebug() << "Did you mean: " << child->objectName();
    	}
    }
    */

    return getQQuickItemWithRoot(path.subPath(1), subItem);
}

QQuickItem* getQQuickItemAtPath(const spix::ItemPath& path)
{
    auto windowName = path.rootComponent();
    QQuickWindow* itemWindow = getQQuickWindowWithName(windowName);
    QQuickItem* item = nullptr;

    if (!itemWindow) {
        return nullptr;
    }

    if (path.length() > 1)
        item = getQQuickItemWithRoot(path.subPath(1), itemWindow);
    else
        item = itemWindow->contentItem();

    return item;
}

} // namespace

namespace spix {

std::unique_ptr<Item> QtScene::itemAtPath(const ItemPath& path)
{
    auto windowName = path.rootComponent();
    QQuickItem* item = getQQuickItemAtPath(path);

    if (item) {
        return std::make_unique<QtItem>(item);
    }
    return std::unique_ptr<QtItem>();
}

Events& QtScene::events()
{
    return m_events;
}

void QtScene::takeScreenshot(const ItemPath& targetItem, const std::string& filePath)
{
    auto item = getQQuickItemAtPath(targetItem);
    if (!item) {
        return;
    }

    // take screenshot of the full window
    auto windowImage = item->window()->grabWindow();

    // get the rect of the item in window space in pixels, account for the device pixel ratio
    QRectF imageCropRectItemSpace {0, 0, item->width(), item->height()};
    auto imageCropRectF = item->mapRectToScene(imageCropRectItemSpace);
    QRect imageCropRect(imageCropRectF.x() * windowImage.devicePixelRatio(),
        imageCropRectF.y() * windowImage.devicePixelRatio(), imageCropRectF.width() * windowImage.devicePixelRatio(),
        imageCropRectF.height() * windowImage.devicePixelRatio());

    // crop the window image to the item rect
    auto image = windowImage.copy(imageCropRect);
    image.save(QString::fromStdString(filePath));
}

} // namespace spix
