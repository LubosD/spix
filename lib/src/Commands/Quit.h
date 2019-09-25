/***
 * Copyright (C) Falko Axmann. All rights reserved.
 * Licensed under the MIT license.
 * See LICENSE.txt file in the project root for full license information.
 ****/

#pragma once

#include "Command.h"

namespace spix {
namespace cmd {

class Quit : public Command {
public:
	Quit(int code);
    void execute(CommandEnvironment& env) override;
private:
    int m_code;
};

} // namespace cmd
} // namespace spix
