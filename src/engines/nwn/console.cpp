/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/console.cpp
 *  NWN (debug) console.
 */

#include "boost/bind.hpp"

#include "common/ustring.h"
#include "common/util.h"

#include "engines/nwn/console.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

namespace Engines {

namespace NWN {

Console::Console() : ::Engines::Console("fnt_console"), _module(0) {
	registerCommand("listareas", boost::bind(&Console::cmdListAreas, this, _1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea" , boost::bind(&Console::cmdGotoArea , this, _1),
			"Usage: gotoarea <area>\nMove to a specific area");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::cmdListAreas(const CommandLine &cl) {
	if (!_module)
		return;

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		print(Common::UString::sprintf("%s (\"%s\")", a->c_str(), Area::getName(*a).c_str()));
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (!_module)
		return;

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			_module->_newArea = *a;
			return;
		}

	print(Common::UString::sprintf("Area \"%s\" does not exist", cl.args.c_str()));
}

} // End of namespace NWN

} // End of namespace Engines
