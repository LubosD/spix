/***
 * Copyright (C) Falko Axmann. All rights reserved.
 * Licensed under the MIT license.
 * See LICENSE.txt file in the project root for full license information.
 ****/

#include "QtItemTools.h"

#include <QQuickItem>

namespace spix {
namespace qt {

const QLatin1String repeater_class_name = QLatin1String("QQuickRepeater");
const char* item_at_method_name = "itemAt";

QQuickItem* RepeaterChildAtIndex(QQuickItem* repeater, int index)
{
    QQuickItem* retVal = nullptr;
    bool success = QMetaObject::invokeMethod(
        repeater, item_at_method_name, Qt::DirectConnection, Q_RETURN_ARG(QQuickItem*, retVal), Q_ARG(int, index));
    if (success)
        return retVal;

    return nullptr;
}

QQuickItem* RepeaterChildWithName(QQuickItem* repeater, const QString& name)
{
    int itemIndex = 0;
    QQuickItem* item = nullptr;
    do {
        item = RepeaterChildAtIndex(repeater, itemIndex);
        if (item) {
            if (item->objectName() == name)
                break;

            QQuickItem* childItem = item->findChild<QQuickItem*>(name);
            if (childItem)
                return childItem;
        }

        ++itemIndex;
    } while (item);

    return item;
}

const QLatin1String listview_class_name = QLatin1String("QQuickListView");
const QLatin1String gridview_class_name = QLatin1String("QQuickGridView");
const char* item_at_index_method_name = "itemAtIndex";

QQuickItem* ListViewChildAtIndex(QQuickItem* repeater, int index)
{
    QQuickItem* retVal = nullptr;
    bool success = QMetaObject::invokeMethod(
        repeater, item_at_index_method_name, Qt::DirectConnection, Q_RETURN_ARG(QQuickItem*, retVal), Q_ARG(int, index));
    if (success)
        return retVal;

    return nullptr;
}

QQuickItem* ListViewChildWithName(QQuickItem* repeater, const QString& name)
{
    int itemIndex = 0;
    QQuickItem* item = nullptr;
    do {
        item = ListViewChildAtIndex(repeater, itemIndex);
        if (item) {
            if (item->objectName() == name)
                break;

            QQuickItem* childItem = item->findChild<QQuickItem*>(name);
            if (childItem)
                return childItem;
        }

        ++itemIndex;
    } while (item);

    return item;
}

} // namespace qt
} // namespace spix
