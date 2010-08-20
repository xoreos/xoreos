/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/placeable.cpp
 *  A placeable.
 */

#include "engines/kotor/placeable.h"

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "engines/util.h"

#include "aurora/2dafile.h"
#include "aurora/gfffile.h"

#include "graphics/aurora/model.h"

static const uint32 kUTPID = MKID_BE('UTP ');

namespace Engines {

namespace KotOR {

Placeable::Placeable(const ModelLoader &modelLoader) : _modelLoader(&modelLoader),
	_appearance(0xFFFFFFFF), _model(0) {

	_position[0] = 0.0;
	_position[1] = 0.0;
	_position[2] = 0.0;
	_bearing     = 0.0;

	_worldPosition   [0] = 0.0;
	_worldPosition   [1] = 0.0;
	_worldPosition   [2] = 0.0;
	_worldOrientation[0] = 0.0;
	_worldOrientation[1] = 0.0;
	_worldOrientation[2] = 0.0;
}

Placeable::~Placeable() {
	delete _model;
}

void Placeable::load(const Common::UString &name) {
	Aurora::GFFFile utp;
	loadGFF(utp, name, Aurora::kFileTypeUTP, kUTPID);

	Aurora::GFFFile::StructRange utpTop = utp.structRange();
	for (Aurora::GFFFile::StructIterator it = utpTop.first; it != utpTop.second; ++it) {
		if (it->getLabel() == "Appearance")
			_appearance = it->getUint();
	}

	if (_appearance == 0xFFFFFFFF)
		throw Common::Exception("Placeable without an appearance");

	warning("TODO: Placeable \"%s\": %d", name.c_str(), _appearance);
}

void Placeable::loadAppearance(const Aurora::TwoDAFile &placeables) {
}

void Placeable::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Placeable::setBearing(float bearing) {
	_bearing = bearing;
}

void Placeable::moveWorld(float x, float y, float z) {
	_worldPosition[0] = x;
	_worldPosition[1] = y;
	_worldPosition[2] = z;
}

void Placeable::turnWorld(float x, float y, float z) {
	_worldOrientation[0] = x;
	_worldOrientation[1] = y;
	_worldOrientation[2] = z;
}

} // End of namespace KotOR

} // End of namespace Engines
