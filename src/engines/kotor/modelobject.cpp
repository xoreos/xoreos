/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/modelobject.cpp
 *  An object associated with a visible model.
 */

#include "engines/kotor/modelobject.h"

namespace Engines {

namespace KotOR {

ModelObject::ModelObject(const ModelLoader &modelLoader) : _modelLoader(&modelLoader) {
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

ModelObject::~ModelObject() {
}

void ModelObject::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	changedPosition();
}

void ModelObject::setBearing(float bearing) {
	_bearing = bearing;

	changedBearing();
}

void ModelObject::moveWorld(float x, float y, float z) {
	_worldPosition[0] = x;
	_worldPosition[1] = y;
	_worldPosition[2] = z;

	changedPosition();
}

void ModelObject::turnWorld(float x, float y, float z) {
	_worldOrientation[0] = x;
	_worldOrientation[1] = y;
	_worldOrientation[2] = z;

	changedOrientation();
}

} // End of namespace KotOR

} // End of namespace Engines
