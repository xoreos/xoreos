/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/console.cpp
 *  KotOR (debug) console.
 */

#include "common/ustring.h"
#include "common/util.h"

#include "engines/kotor/console.h"
#include "engines/kotor/module.h"

namespace Engines {

namespace KotOR {

Console::Console(Module &module) : ::Engines::Console("fnt_console"),
	_module(&module) {

	registerCommand("gotomodule", "Usage: gotomodule <module>\nEnter the specified module");
}

Console::~Console() {
}

bool Console::cmdCallback(Common::UString cmd, Common::UString args) {
	cmd.tolower();

	if (cmd == "gotomodule")
		return gotoModule(args);

	return false;
}

bool Console::gotoModule(Common::UString args) {
	_module->leave();
	_module->clear();

	try {
		_module->load(args);
		_module->enter();
	} catch (Common::Exception &e) {
		printException(e);
	}

	return true;
}

} // End of namespace KOTOR

} // End of namespace Engines
