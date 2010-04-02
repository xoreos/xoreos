/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gffstructs.cpp
 *  %Common structures found in BioWare's GFF files.
 */

#include <cstring>

#include "aurora/gffstructs.h"

namespace Aurora {

GFFLocation::GFFLocation(uint32 area, const double *position, const double *orientation) {
	clear();

	_area = area;

	if (position)
		setPosition(position);
	if (orientation)
		setOrientation(orientation);
}

void GFFLocation::clear() {
	_area = kObjectIDInvalid;

	clearPosition();
	clearOrientation();
}

void GFFLocation::clearPosition() {
	_position[0] = 0.0;
	_position[1] = 0.0;
	_position[2] = 0.0;
}

void GFFLocation::clearOrientation() {
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

uint32 GFFLocation::getArea() const {
	return _area;
}

const double *GFFLocation::getPosition() const {
	return _position;
}

const double *GFFLocation::getOrientation() const {
	return _orientation;
}

void GFFLocation::setArea(uint32 area) {
	_area = area;
}

void GFFLocation::setPosition(const double *position) {
	std::memcpy(_position, position, 3 * sizeof(double));
}

void GFFLocation::setOrientation(const double *orientation) {
	std::memcpy(_orientation, orientation, 3 * sizeof(double));
}

bool GFFLocation::read(const GFFFile::StructRange &range) {
	for (GFFFile::StructIterator it = range.first; it != range.second; ++it) {
		if      (it->getLabel() == "Area")
			_area           = it->getUint();
		else if (it->getLabel() == "PositionX")
			_position[0]    = it->getDouble();
		else if (it->getLabel() == "PositionY")
			_position[1]    = it->getDouble();
		else if (it->getLabel() == "PositionZ")
			_position[2]    = it->getDouble();
		else if (it->getLabel() == "OrientationX")
			_orientation[0] = it->getDouble();
		else if (it->getLabel() == "OrientationY")
			_orientation[1] = it->getDouble();
		else if (it->getLabel() == "OrientationZ")
			_orientation[2] = it->getDouble();
	}

	return true;
}

} // End of namespace Aurora
