/***
 * Copyright (C) Falko Axmann. All rights reserved.
 * Licensed under the MIT license.
 * See LICENSE.txt file in the project root for full license information.
 ****/

#include "Quit.h"

#include <Scene/Scene.h>

namespace spix {
namespace cmd {

Quit::Quit(int code)
: m_code(code)
{
}

void Quit::execute(CommandEnvironment& env)
{
    env.scene().events().quit(m_code);
}

} // namespace cmd
} // namespace spix
