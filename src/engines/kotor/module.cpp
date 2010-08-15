/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "aurora/gfffile.h"

namespace Engines {

namespace KotOR {

Module::Module() : _startX(0.0), _startY(0.0), _startZ(0.0), _startDirX(0.0), _startDirY(0.0), _area(0) {
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

Module::~Module() {
	delete _area;

	ResMan.undo(_moduleResources);
	ResMan.undo(_scriptResources);
}

void Module::load(const Common::UString &name) {
	loadResources(name);

	loadIFO(name);

	loadArea();

	// We don't need that one anymore
	ResMan.undo(_moduleResources);
}

Area *Module::createArea() const {
	return new Area;
}

void Module::loadResources(const Common::UString &name) {
	_moduleResources = ResMan.addArchive(Aurora::kArchiveRIM, name + ".rim"  , 100);
	_scriptResources = ResMan.addArchive(Aurora::kArchiveRIM, name + "_s.rim", 105);
}

void Module::loadIFO(const Common::UString &name) {
	Common::SeekableReadStream *ifoFile = 0;
	Aurora::GFFFile ifo;
	try {
		if (!(ifoFile = ResMan.getResource("module", Aurora::kFileTypeIFO)))
			throw Common::Exception("No module.ifo in module \"%s\"", name.c_str());

		ifo.load(*ifoFile);

		delete ifoFile;
	} catch(...) {
		delete ifoFile;
		throw;
	}


	Aurora::GFFFile::StructRange ifoTop = ifo.structRange();
	for (Aurora::GFFFile::StructIterator it = ifoTop.first; it != ifoTop.second; ++it) {
		if      (it->getLabel() == "Mod_Entry_Area")
			_areaName  = it->getString();
		else if (it->getLabel() == "Mod_Entry_X")
			_startX    = it->getDouble();
		else if (it->getLabel() == "Mod_Entry_Y")
			_startY    = it->getDouble();
		else if (it->getLabel() == "Mod_Entry_Z")
			_startZ    = it->getDouble();
		else if (it->getLabel() == "Mod_Entry_Dir_X")
			_startDirX = it->getDouble();
		else if (it->getLabel() == "Mod_Entry_Dir_Y")
			_startDirY = it->getDouble();

		// TODO: Mod_On*
	}

	if (_areaName.empty())
		throw Common::Exception("No entry area in module \"%s\"", name.c_str());

	Common::vector2orientation(_startDirX, _startDirY, _orientation[0], _orientation[1], _orientation[2]);
}

void Module::loadArea() {
	_area = createArea();

	_area->load(_areaName);
}

void Module::enter() {
	assert(_area);

	_area->setPosition(-_startX, -_startY, -_startZ);
	_area->setOrientation(_orientation[0], _orientation[1], _orientation[2]);
	_area->show();
}

void Module::leave() {
	assert(_area);

	_area->hide();
}

} // End of namespace KotOR

} // End of namespace Engines
