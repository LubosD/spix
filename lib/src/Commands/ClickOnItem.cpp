/***
 * Copyright (C) Falko Axmann. All rights reserved.
 * Licensed under the MIT license.
 * See LICENSE.txt file in the project root for full license information.
 ****/

#include "ClickOnItem.h"

#include <Scene/Scene.h>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace spix {
namespace cmd {

ClickOnItem::ClickOnItem(ItemPosition path, bool longClick, std::promise<void> promise)
: m_position(std::move(path)), m_longClick(longClick), m_promise(std::move(promise))
{
}

void ClickOnItem::execute(CommandEnvironment& env)
{
    try {
        auto path = m_position.itemPath();
        auto item = env.scene().itemAtPath(path);

        if (!item)
            throw std::runtime_error("ClickOnItem: Item not found: " + path.string());

        auto size = item->size();
        auto mousePoint = m_position.positionForItemSize(size);
        env.scene().events().mouseDown(item.get(), mousePoint, Events::MouseButtons::left);
        
        if (!m_longClick)
        // Note: doing sleeping here doesn't work, because QML really needs the event loop running for the long click
        // Flushing the event loop does NOT help.
        //    std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //else
            env.scene().events().mouseUp(item.get(), mousePoint, Events::MouseButtons::left);

        m_promise.set_value();
    } catch (const std::runtime_error& e) {
        env.state().reportError(e.what());
        m_promise.set_exception(std::make_exception_ptr(e));
    }
}

} // namespace cmd
} // namespace spix
