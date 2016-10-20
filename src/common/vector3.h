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
 *  A 3d vector.
 */

#ifndef COMMON_VECTOR3_H
#define COMMON_VECTOR3_H

#include <cmath>

namespace Common {

/**
 * After some considerable thought on the matter, the decision in the end
 * was to allow Vector3 to operate interchangeably with 4x4 matrix
 * transformations.
 * Pros:
 *    - Transforms (4x4 matrix) can use a Vector3 directly.
 *    - Vector3 will be far more widely used than Vector4, so no conversion
 *      requirements. It will just work.
 *    - 128bit (16 byte) alignment is typically good for caching.
 * Cons:
 *    - Can't abuse vertex array data directly as Vector3 (alignment issues
 *      might prevent that anyway).
 *    - Vector3 actually has 4 components; everyone should be aware of the
 *      actual size of the class.
 *    - More space used. Note that moving to SSE intrinsics would require
 *      the extra component padding anyway.
 * Please note that _w is initialised to 1.0f. This is not a mistake.
 */
class Vector3 {
public:
	float _x, _y, _z, _w;

	Vector3() {
	}

	Vector3(float x, float y, float z, float w = 1.0f) : _x(x), _y(y), _z(z), _w(w) {
	}

	Vector3(const float *b) : _x(b[0]), _y(b[1]), _z(b[2]), _w(1.0f) {
	}

	inline const float &operator[](int i) const {
		return (&_x)[i];
	}

	inline float &operator[](int i) {
		return (&_x)[i];
	}

	inline const Vector3 &operator=(const float *v) {
		_x = v[0]; _y = v[1]; _z = v[2]; // Note: _w not included here.
		return *this;
	}

	inline const Vector3 &operator+=(const Vector3 &v) {
		_x += v._x; _y += v._y; _z += v._z;
		return *this;
	}

	inline const Vector3 &operator-=(const Vector3 &v) {
		_x -= v._x; _y -= v._y; _z -= v._z;
		return *this;
	}

	inline const Vector3 &operator*=(float value) {
		_x *= value; _y *= value; _z *= value;
		return *this;
	}

	inline const Vector3 &operator/=(float value) {
		float inverse = 1.0f / value;
		_x *= inverse; _y *= inverse; _z *= inverse;
		return *this;
	}

	/** Component-wise multiply. */
	inline const Vector3 &multiply(const Vector3 &v) {
		_x *= v._x; _y *= v._y; _z *= v._z;
		return *this;
	}

	/** Component-wise divide. */
	inline const Vector3 &divide(const Vector3 &v) {
		_x /= v._x; _y /= v._y; _z /= v._z;
		return *this;
	}

	Vector3 operator-(const Vector3 &v) const {
		return Vector3(_x - v._x, _y - v._y, _z - v._z);
	}

	Vector3 operator+(const Vector3 &v) const {
		return Vector3(_x + v._x, _y + v._y, _z + v._z);
	}

	Vector3 operator*(const float f) const {
		return Vector3(_x * f, _y * f, _z * f);
	}

	Vector3 operator*(const Vector3 &v) const {
		return Vector3(_y * v._z - _z * v._y,
		               _z * v._x - _x * v._z,
		               _x * v._y - _y * v._x);
	}

	Vector3 cross(const Vector3 &v) const {
		return Vector3(_y * v._z - _z * v._y,
		               _z * v._x - _x * v._z,
		               _x * v._y - _y * v._x);
	}

	float dot(const Vector3 &v) const {
		return _x * v._x + _y * v._y + _z * v._z;
	}

	float length() const {
		return sqrtf((*this).dot(*this));
	}

	Vector3 &norm() {
		(*this) = (*this) * (1.0f / length());
		return *this;
	}

	float angle(const Vector3 &v) const {
		return acos((dot(v) / (sqrt(((_x * _x) + (_y * _y) + (_z * _z)) * ((v._x * v._x) + (v._y * v._y) + (v._z * v._z))))));
	}
};

} // End of namespace Common

#endif // COMMON_VECTOR3_H
