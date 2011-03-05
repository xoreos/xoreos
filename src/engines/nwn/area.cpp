/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/area.cpp
 *  NWN area.
 */

#include "engines/nwn/area.h"
#include "engines/nwn/module.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &name) : _module(&module), _name(name) {
}

Area::~Area() {
}

const Common::UString &Area::getName() {
	return _name;
}

void Area::show() {
}

void Area::hide() {
}

} // End of namespace NWN

} // End of namespace Engines
