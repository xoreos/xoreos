/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A bounding box.
 */

#include "src/common/boundingbox.h"
#include "src/common/util.h"
#include "src/common/maths.h"

namespace Common {

BoundingBox::BoundingBox() {
	clear();
}

BoundingBox::~BoundingBox() {
}

void BoundingBox::clear() {
	_empty    = true;
	_absolute = true;

	// Set to boundaries
	_coords[0][0] =  FLT_MAX; _coords[0][1] =  FLT_MAX; _coords[0][2] =  FLT_MAX;
	_coords[1][0] =  FLT_MAX; _coords[1][1] =  FLT_MAX; _coords[1][2] = -FLT_MAX;
	_coords[2][0] =  FLT_MAX; _coords[2][1] = -FLT_MAX; _coords[2][2] =  FLT_MAX;
	_coords[3][0] =  FLT_MAX; _coords[3][1] = -FLT_MAX; _coords[3][2] = -FLT_MAX;
	_coords[4][0] = -FLT_MAX; _coords[4][1] =  FLT_MAX; _coords[4][2] =  FLT_MAX;
	_coords[5][0] = -FLT_MAX; _coords[5][1] =  FLT_MAX; _coords[5][2] = -FLT_MAX;
	_coords[6][0] = -FLT_MAX; _coords[6][1] = -FLT_MAX; _coords[6][2] =  FLT_MAX;
	_coords[7][0] = -FLT_MAX; _coords[7][1] = -FLT_MAX; _coords[7][2] = -FLT_MAX;

	_min[0] = 0.0f; _min[1] = 0.0f; _min[2] = 0.0f;
	_max[0] = 0.0f; _max[1] = 0.0f; _max[2] = 0.0f;

	_origin.loadIdentity();
}

bool BoundingBox::empty() const {
	return _empty;
}

const Matrix4x4 &BoundingBox::getOrigin() const {
	return _origin;
}

void BoundingBox::getMin(float &x, float &y, float &z) const {
	// Minimum, relative to the origin

	if (_absolute) {
		x = _min[0]; y = _min[1]; z = _min[2];
		return;
	}

	Matrix4x4 min = _origin;
	min.translate(_min[0], _min[1], _min[2]);

	Matrix4x4 max = _origin;
	max.translate(_max[0], _max[1], _max[2]);

	x = MIN(min.getX(), max.getX());
	y = MIN(min.getY(), max.getY());
	z = MIN(min.getZ(), max.getZ());
}

void BoundingBox::getMax(float &x, float &y, float &z) const {
	// Maximum, relative to the origin

	if (_absolute) {
		x = _max[0]; y = _max[1]; z = _max[2];
		return;
	}

	Matrix4x4 min = _origin;
	min.translate(_min[0], _min[1], _min[2]);

	Matrix4x4 max = _origin;
	max.translate(_max[0], _max[1], _max[2]);

	x = MAX(min.getX(), max.getX());
	y = MAX(min.getY(), max.getY());
	z = MAX(min.getZ(), max.getZ());
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

bool BoundingBox::isIn(float x, float y) const {
	if (_empty)
		return false;

	float minX, minY, minZ;
	getMin(minX, minY, minZ);

	float maxX, maxY, maxZ;
	getMax(maxX, maxY, maxZ);

	if ((x < minX) || (x > maxX))
		return false;
	if ((y < minY) || (y > maxY))
		return false;

	return true;
}

bool BoundingBox::isIn(float x, float y, float z) const {
	if (_empty)
		return false;

	float minX, minY, minZ;
	getMin(minX, minY, minZ);

	float maxX, maxY, maxZ;
	getMax(maxX, maxY, maxZ);

	if ((x < minX) || (x > maxX))
		return false;
	if ((y < minY) || (y > maxY))
		return false;
	if ((z < minZ) || (z > maxZ))
		return false;

	return true;
}

bool BoundingBox::getIntersection(float fDst1, float fDst2,
                                  float x1, float y1, float z1,
                                  float x2, float y2, float z2,
                                  float &x, float &y, float &z) const {

	if ((fDst1 * fDst2) >= 0.0f)
		return false;
	if (fDst1 == fDst2)
		return false;

	x = x1 + ((x2 - x1) * (-fDst1 / (fDst2 - fDst1)));
	y = y1 + ((y2 - y1) * (-fDst1 / (fDst2 - fDst1)));
	z = z1 + ((z2 - z1) * (-fDst1 / (fDst2 - fDst1)));

	return true;
}

bool BoundingBox::inBox(float x, float y, float z, float minX, float minY, float minZ,
                        float maxX, float maxY, float maxZ, int axis) const {

	if (((axis == 1) && (z > minZ) && (z < maxZ) && (y > minY) && (y < maxY)) ||
      ((axis == 2) && (z > minZ) && (z < maxZ) && (x > minX) && (x < maxX)) ||
	    ((axis == 3) && (x > minX) && (x < maxX) && (y > minY) && (y < maxY)))
		return true;

	return false;
}

bool BoundingBox::isIn(float x1, float y1, float z1, float x2, float y2, float z2) const {
	if (_empty)
		return false;

	float minX, minY, minZ;
	getMin(minX, minY, minZ);

	float maxX, maxY, maxZ;
	getMax(maxX, maxY, maxZ);

	if ((x2 < minX) && (x1 < minX)) return false;
	if ((x2 > maxX) && (x1 > maxX)) return false;
	if ((y2 < minY) && (y1 < minY)) return false;
	if ((y2 > maxY) && (y1 > maxY)) return false;
	if ((z2 < minZ) && (z1 < minZ)) return false;
	if ((z2 > maxZ) && (z1 > maxZ)) return false;

	if ((x1 > minX) && (x1 < maxX) &&
	    (y1 > minY) && (y1 < maxY) &&
	    (z1 > minZ) && (z1 < maxZ))
		return true;

	float x, y, z;

	if (getIntersection(x1 - minX, x2 - minX, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 1))
		return true;
	if (getIntersection(y1 - minY, y2 - minY, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 2))
		return true;
	if (getIntersection(z1 - minZ, z2 - minZ, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 3))
		return true;
	if (getIntersection(x1 - maxX, x2 - maxX, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 1))
		return true;
	if (getIntersection(y1 - maxY, y2 - maxY, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 2))
		return true;
	if (getIntersection(z1 - maxZ, z2 - maxZ, x1, y1, z1, x2, y2, z2, x, y, z) &&
	    inBox(x, y, z, minX, minY, minZ, maxX, maxY, maxZ, 3))
		return true;

	return false;
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
	_absolute = false;
}

void BoundingBox::scale(float x, float y, float z) {
	_origin.scale(x, y, z);
	_absolute = false;
}

void BoundingBox::rotate(float angle, float x, float y, float z) {
	_origin.rotate(angle, x, y, z);
	_absolute = false;
}

void BoundingBox::transform(const Matrix4x4 &m) {
	_origin *= m;
	_absolute = false;
}

inline float BoundingBox::getCoordMin(int i) const {
	if (i == 0)
		return MIN(MIN(MIN(_coords[0][0], _coords[1][0]), _coords[2][0]), _coords[3][0]);
	if (i == 1)
		return MIN(MIN(MIN(_coords[0][1], _coords[1][1]), _coords[4][1]), _coords[5][1]);
	if (i == 2)
		return MIN(MIN(MIN(_coords[0][2], _coords[2][2]), _coords[4][2]), _coords[6][2]);

	return 0.0f;
}

inline float BoundingBox::getCoordMax(int i) const {
	if (i == 0)
		return MAX(MAX(MAX(_coords[4][0], _coords[5][0]), _coords[6][0]), _coords[7][0]);
	if (i == 1)
		return MAX(MAX(MAX(_coords[2][1], _coords[3][1]), _coords[6][1]), _coords[7][1]);
	if (i == 2)
		return MAX(MAX(MAX(_coords[1][2], _coords[3][2]), _coords[5][2]), _coords[7][2]);

	return 0.0f;
}

void BoundingBox::absolutize() {
	if (_empty)
		// Nothing to do
		return;

	float coords[8][3];
	for (int i = 0; i < 8; i++) {
		Matrix4x4 c = _origin;

		c.translate(_coords[i][0], _coords[i][1], _coords[i][2]);

		c.getPosition(coords[i][0], coords[i][1], coords[i][2]);
	}

	clear();

	for (int i = 0; i < 8; i++)
		add(coords[i][0], coords[i][1], coords[i][2]);

	_absolute = true;
}

BoundingBox BoundingBox::getAbsolute() const {
	BoundingBox box = *this;

	box.absolutize();

	return box;
}

} // End of namespace Common
