/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/boundingbox.cpp
 *  A bounding box.
 */

#include <cstdio>

#include "common/boundingbox.h"
#include "common/util.h"
#include "common/maths.h"

namespace Common {

BoundingBox::BoundingBox() {
	clear();
}

BoundingBox::~BoundingBox() {
}

void BoundingBox::clear() {
	_empty = true;

	// Set to boundaries
	_coords[0][0] =  FLT_MAX; _coords[0][1] =  FLT_MAX; _coords[0][2] =  FLT_MAX;
	_coords[1][0] =  FLT_MAX; _coords[1][1] =  FLT_MAX; _coords[1][2] = -FLT_MAX;
	_coords[2][0] =  FLT_MAX; _coords[2][1] = -FLT_MAX; _coords[2][2] =  FLT_MAX;
	_coords[3][0] =  FLT_MAX; _coords[3][1] = -FLT_MAX; _coords[3][2] = -FLT_MAX;
	_coords[4][0] = -FLT_MAX; _coords[4][1] =  FLT_MAX; _coords[4][2] =  FLT_MAX;
	_coords[5][0] = -FLT_MAX; _coords[5][1] =  FLT_MAX; _coords[5][2] = -FLT_MAX;
	_coords[6][0] = -FLT_MAX; _coords[6][1] = -FLT_MAX; _coords[6][2] =  FLT_MAX;
	_coords[7][0] = -FLT_MAX; _coords[7][1] = -FLT_MAX; _coords[7][2] = -FLT_MAX;

	_min[0] = 0.0; _min[1] = 0.0; _min[2] = 0.0;
	_max[0] = 0.0; _max[1] = 0.0; _max[2] = 0.0;

	_origin.loadIdentity();
}

bool BoundingBox::isEmpty() const {
	return _empty;
}

void BoundingBox::getMin(float &x, float &y, float &z) const {
	// Minimum, relative to the origin

	TransformationMatrix m = _origin;

	m.translate(_min[0], _min[1], _min[2]);

	x = m.getX();
	y = m.getY();
	z = m.getZ();
}

void BoundingBox::getMax(float &x, float &y, float &z) const {
	// Maximum, relative to the origin

	TransformationMatrix m = _origin;

	m.translate(_max[0], _max[1], _max[2]);

	x = m.getX();
	y = m.getY();
	z = m.getZ();
}

float BoundingBox::getWidth() const {
	return ABS(_max[0] - _min[0]);
}

float BoundingBox::getHeight() const {
	return ABS(_max[1] - _min[1]);
}

float BoundingBox::getDepth() const {
	return ABS(_max[2] - _min[2]);
}

bool BoundingBox::isIn(float x, float y, float z) const {
	if (_empty)
		return false;

	TransformationMatrix min = _origin;
	TransformationMatrix max = _origin;

	min.translate(_min[0], _min[1], _min[2]);
	max.translate(_max[0], _max[1], _max[2]);

	if ((x < min.getX()) || (x > max.getX()))
		return false;
	if ((y < min.getY()) || (y > max.getY()))
		return false;
	if ((z < min.getZ()) || (z > max.getZ()))
		return false;

	return true;
}

void BoundingBox::add(float x, float y, float z) {
	_coords[0][0] = MIN(_coords[0][0], x); _coords[0][1] = MIN(_coords[0][1], y); _coords[0][2] = MIN(_coords[0][2], z);
	_coords[1][0] = MIN(_coords[1][0], x); _coords[1][1] = MIN(_coords[1][1], y); _coords[1][2] = MAX(_coords[1][2], z);
	_coords[2][0] = MIN(_coords[2][0], x); _coords[2][1] = MAX(_coords[2][1], y); _coords[2][2] = MIN(_coords[2][2], z);
	_coords[3][0] = MIN(_coords[3][0], x); _coords[3][1] = MAX(_coords[3][1], y); _coords[3][2] = MAX(_coords[3][2], z);
	_coords[4][0] = MAX(_coords[4][0], x); _coords[4][1] = MIN(_coords[4][1], y); _coords[4][2] = MIN(_coords[4][2], z);
	_coords[5][0] = MAX(_coords[5][0], x); _coords[5][1] = MIN(_coords[5][1], y); _coords[5][2] = MAX(_coords[5][2], z);
	_coords[6][0] = MAX(_coords[6][0], x); _coords[6][1] = MAX(_coords[6][1], y); _coords[6][2] = MIN(_coords[6][2], z);
	_coords[7][0] = MAX(_coords[7][0], x); _coords[7][1] = MAX(_coords[7][1], y); _coords[7][2] = MAX(_coords[7][2], z);

	for (int i = 0; i < 3; i++) {
		float min = getCoordMin(i);
		float max = getCoordMax(i);

		_min[i] = _empty ? min : MIN(_min[i], min);
		_max[i] = _empty ? max : MAX(_max[i], max);
	}

	_empty = false;
}

void BoundingBox::add(const BoundingBox &box) {
	if (box._empty)
		// Don't add an empty bounding box :P
		return;

	for (int i = 0; i < 8; i++)
		add(box._coords[i][0], box._coords[i][1], box._coords[i][2]);
}

void BoundingBox::translate(float x, float y, float z) {
	_origin.translate(x, y, z);
}

void BoundingBox::scale(float x, float y, float z) {
	_origin.scale(x, y, z);
}

void BoundingBox::rotate(float angle, float x, float y, float z) {
	_origin.rotate(angle, x, y, z);
}

inline float BoundingBox::getCoordMin(int i) const {
	if (i == 0)
		return MIN(MIN(MIN(_coords[0][0], _coords[1][0]), _coords[2][0]), _coords[3][0]);
	if (i == 1)
		return MIN(MIN(MIN(_coords[0][1], _coords[1][1]), _coords[4][1]), _coords[5][1]);
	if (i == 2)
		return MIN(MIN(MIN(_coords[0][2], _coords[2][2]), _coords[4][2]), _coords[6][2]);

	return 0.0;
}

inline float BoundingBox::getCoordMax(int i) const {
	if (i == 0)
		return MAX(MAX(MAX(_coords[4][0], _coords[5][0]), _coords[6][0]), _coords[7][0]);
	if (i == 1)
		return MAX(MAX(MAX(_coords[2][1], _coords[3][1]), _coords[6][1]), _coords[7][1]);
	if (i == 2)
		return MAX(MAX(MAX(_coords[1][2], _coords[3][2]), _coords[5][2]), _coords[7][2]);

	return 0.0;
}

void BoundingBox::absolutize() {
	if (_empty)
		// Nothing to do
		return;

	for (int i = 0; i < 8; i++) {
		TransformationMatrix c = _origin;

		c.translate(_coords[i][0], _coords[i][1], _coords[i][2]);

		_coords[i][0] = c.getX(); _coords[i][1] = c.getY(), _coords[i][2] = c.getZ();
	}

	TransformationMatrix min = _origin;
	TransformationMatrix max = _origin;

	min.translate(_min[0], _min[1], _min[2]);
	max.translate(_max[0], _max[1], _max[2]);

	_min[0] = min.getX(); _min[1] = min.getY(); _min[2] = min.getZ();
	_max[0] = max.getX(); _max[1] = max.getY(); _max[2] = max.getZ();

	_origin.loadIdentity();
}

BoundingBox BoundingBox::getAbsolute() const {
	BoundingBox box = *this;

	box.absolutize();

	return box;
}

} // End of namespace Common
