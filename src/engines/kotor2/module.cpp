/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/module.cpp
 *  A module.
 */

#include "engines/kotor2/module.h"
#include "engines/kotor2/area.h"

namespace Engines {

namespace KotOR2 {

Module::Module(Console &console) : Engines::KotOR::Module(console) {
}

Module::~Module() {
}

::Engines::KotOR::Area *Module::createArea() const {
	return new ::Engines::KotOR2::Area;
}

} // End of namespace KotOR2

} // End of namespace Engines
