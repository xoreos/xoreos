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

#include "boost/bind.hpp"

#include "common/ustring.h"
#include "common/util.h"

#include "engines/kotor/console.h"
#include "engines/kotor/module.h"

namespace Engines {

namespace KotOR {

Console::Console(Module &module) : ::Engines::Console("fnt_console"),
	_module(&module) {

	registerCommand("gotomodule", boost::bind(&Console::cmdGotoModule, this, _1),
			"Usage: gotomodule <module>\nEnter the specified module");
}

Console::~Console() {
}

void Console::cmdGotoModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	_module->leave();
	_module->clear();

	try {
		_module->load(cl.args);
		_module->enter();
	} catch (Common::Exception &e) {
		printException(e);
	}
}

} // End of namespace KOTOR

} // End of namespace Engines
