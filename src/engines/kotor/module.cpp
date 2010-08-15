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

	reset();
}

void Module::loadArea() {
	_area = createArea();

	_area->load(_areaName);
}

void Module::enter() {
	assert(_area);

	reset();

	_area->setPosition(_position[0], _position[1], _position[2]);
	_area->setOrientation(_orientation[0], _orientation[1], _orientation[2]);

	_area->show();
}

void Module::leave() {
	assert(_area);

	_area->hide();
}

void Module::reset() {
	_position[0] = -_startX;
	_position[1] = -_startY;
	_position[2] = -_startZ;

	Common::vector2orientation(_startDirX, _startDirY, _orientation[0], _orientation[1], _orientation[2]);
}

const float *Module::getPosition() const {
	return _position;
}

const float *Module::getOrientation() const {
	return _orientation;
}

void Module::setPosition(float x, float y, float z) {
	assert(_area);

	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	_area->setPosition(_position[0], _position[1], _position[2]);
}

void Module::setOrientation(float x, float y, float z) {
	assert(_area);

	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;

	// Clamp
	for (int i = 0; i < 3; i++) {
		while (_orientation[i] >  360)
			_orientation[i] -= 360;
		while (_orientation[i] < -360)
			_orientation[i] += 360;
	}

	_area->setOrientation(_orientation[0], _orientation[1], _orientation[2]);
}

void Module::turn(float x, float y, float z) {
	setOrientation(_orientation[0] + x, _orientation[1] + y, _orientation[2] + z);
}

void Module::move(float x, float y, float z) {
	setPosition(_position[0] - x, _position[1] - y, _position[2] - z);
}

void Module::move(float n) {
	float x = n * sin(Common::deg2rad(_orientation[1]));
	float y = n * cos(Common::deg2rad(_orientation[1]));

	move(x, y, 0.0);
}

void Module::strafe(float n) {
	float x = n * sin(Common::deg2rad(_orientation[1] + 90.0));
	float y = n * cos(Common::deg2rad(_orientation[1] + 90.0));

	move(x, y, 0.0);
}

} // End of namespace KotOR

} // End of namespace Engines
