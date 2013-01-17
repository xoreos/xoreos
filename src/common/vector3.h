/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/vector3.h
 *  A 3d vector.
 */

#ifndef COMMON_VECTOR3_H
#define COMMON_VECTOR3_H

#include <cmath>

namespace Common {

class Vector3 {
	float _x, _y, _z;

public:
	Vector3() {
	}

	Vector3(float x, float y, float z) : _x(x), _y(y), _z(z) {
	}

	Vector3(const Vector3 &b) : _x(b._x), _y(b._y), _z(b._z) {
	}

	const float &operator[] (int i) const {
		return (&_x)[i];
	}

	float &operator[] (int i) {
		return (&_x)[i];
	}

	Vector3 operator - (const Vector3 &v) const {
		return Vector3(_x - v._x, _y - v._y, _z - v._z);
	}

	Vector3 operator + (const Vector3 &v) const {
		return Vector3(_x + v._x, _y + v._y, _z + v._z);
	}

	Vector3 operator * (const float f) const {
		return Vector3(_x * f, _y * f, _z * f);
	}

	Vector3 cross(const Vector3 & v) const {
		return Vector3(_y * v._z - _z * v._y,
					_z * v._x - _x * v._z,
					_x * v._y - _y * v._x);
	}

	float dot(const Vector3 & v) const {
		return _x * v._x + _y * v._y + _z * v._z;
	}

	float length() const {
		return sqrtf((*this).dot(*this));
	}

	Vector3 & norm() {
		(*this) = (*this) * (1 / length());
		return *this;
	}
};

}

#endif // COMMON_VECTOR3_H
