/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/module.cpp
 *  A module.
 */

#include "engines/kotor/module.h"
#include "engines/kotor/area.h"

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "aurora/gfffile.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

static const uint32 kIFOID = MKID_BE('IFO ');

namespace Engines {

namespace KotOR {

Module::Module() : _startX(0.0), _startY(0.0), _startZ(0.0),
	_startDirX(0.0), _startDirY(0.0), _area(0) {

}

Module::~Module() {
	delete _area;

	ResMan.undo(_moduleResources);
	ResMan.undo(_scriptResources);
	ResMan.undo(_xboxLayoutResources);
	ResMan.undo(_xboxTextureResources);
}

void Module::load(const Common::UString &name) {
	status("Loading module \"%s\"", name.c_str());

	try {

		loadResources(name);

		loadIFO(name);

		loadArea();

	} catch (Common::Exception &e) {
		e.add("Failed loading module \"%s\"", name.c_str());
		throw e;
	}

	// We don't need that one anymore
	ResMan.undo(_moduleResources);
}

void Module::loadResources(const Common::UString &name) {
	_moduleResources = ResMan.addArchive(Aurora::kArchiveRIM, name + ".rim"  , 100);
	_scriptResources = ResMan.addArchive(Aurora::kArchiveRIM, name + "_s.rim", 105);

	try {
		_xboxLayoutResources  = ResMan.addArchive(Aurora::kArchiveRIM, name + "_a.rim"  , 110);
		_xboxTextureResources = ResMan.addArchive(Aurora::kArchiveRIM, name + "_adx.rim", 115);
	} catch (...) {
		// Ignore, these only exist on the Xbox
	}
}

void Module::loadIFO(const Common::UString &name) {
	Aurora::GFFFile ifo;
	loadGFF(ifo, "module", Aurora::kFileTypeIFO, kIFOID);

	const Aurora::GFFStruct &top = ifo.getTopLevel();

	_areaName = top.getString("Mod_Entry_Area");
	if (_areaName.empty())
		throw Common::Exception("No entry area in module \"%s\"", name.c_str());

	_startX = top.getDouble("Mod_Entry_X");
	_startY = top.getDouble("Mod_Entry_Y");
	_startZ = top.getDouble("Mod_Entry_Z");

	_startDirX = top.getDouble("Mod_Entry_Dir_X");
	_startDirY = top.getDouble("Mod_Entry_Dir_Y");

	// TODO: Mod_On*

	reset();
}

void Module::loadArea() {
	_area = new Area;

	_area->load(_areaName);
}

void Module::enter() {
	assert(_area);

	reset();

	GfxMan.lockFrame();
	_area->show();
	GfxMan.unlockFrame();
}

void Module::leave() {
	assert(_area);

	GfxMan.lockFrame();
	_area->hide();
	GfxMan.unlockFrame();
}

void Module::reset() {
	CameraMan.reset();

	// Roughly head position
	CameraMan.setPosition(_startX, _startZ + 1.8, _startY);

	CameraMan.setOrientation(_startDirX, _startDirY);
}

} // End of namespace KotOR

} // End of namespace Engines
