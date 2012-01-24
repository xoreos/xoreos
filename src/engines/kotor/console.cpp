/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/console.cpp
 *  KotOR (debug) console.
 */

#include "boost/bind.hpp"

#include "common/ustring.h"
#include "common/util.h"

#include "graphics/aurora/fontman.h"

#include "engines/kotor/console.h"
#include "engines/kotor/module.h"

namespace Engines {

namespace KotOR {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_module(0) {

	registerCommand("loadmodule", boost::bind(&Console::cmdLoadModule, this, _1),
			"Usage: loadmodule <module>\nLoad and enter the specified module");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	_module->replaceModule(cl.args);
}

} // End of namespace KOTOR

} // End of namespace Engines
