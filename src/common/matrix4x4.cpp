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
 *  A 4x4 matrix, usable for transformations in 3D space.
 */

#include <cstring>

#include "src/common/matrix4x4.h"
#include "src/common/maths.h"

static const float kIdentity[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

namespace Common {

Matrix4x4::Matrix4x4(bool identity) {
	if (identity)
		loadIdentity();
}

Matrix4x4::Matrix4x4(const float *m) {
	std::memcpy(_elements, m, 16 * sizeof(float));
}

Matrix4x4::~Matrix4x4() {
}

const float *Matrix4x4::get() const {
	return _elements;
}

float Matrix4x4::getX() const {
	return _elements[12];
}

float Matrix4x4::getY() const {
	return _elements[13];
}

float Matrix4x4::getZ() const {
	return _elements[14];
}

void Matrix4x4::getPosition(float &x, float &y, float &z) const {
	x = _elements[12];
	y = _elements[13];
	z = _elements[14];
}

const float *Matrix4x4::getPosition() const {
	return &(_elements[12]);
}

const float *Matrix4x4::getXAxis() const {
	return &(_elements[0]);
}

const float *Matrix4x4::getYAxis() const {
	return &(_elements[4]);
}

const float *Matrix4x4::getZAxis() const {
	return &(_elements[8]);
}

void Matrix4x4::getAxisAngle(float &angle, float &x, float &y, float &z) const {
	const float epsilon1 = 0.001f, epsilon2 = 0.01f, epsilon3 = 0.1f;

		// Look for symmetry to avoid singularities near 0°/180°
	if ((fabs(_elements[4] - _elements[1]) < epsilon2) &&
	    (fabs(_elements[8] - _elements[2]) < epsilon2) &&
	    (fabs(_elements[9] - _elements[6]) < epsilon2)) {

		if ((fabs(_elements[4] + _elements[1]                    ) < epsilon3) &&
		    (fabs(_elements[8] + _elements[2]                    ) < epsilon3) &&
		    (fabs(_elements[9] + _elements[6]                    ) < epsilon3) &&
		    (fabs(_elements[0] + _elements[5] + _elements[10] - 3) < epsilon3)) {

			// A rotation of 0°
			angle = 0.0f; x = 1.0f; y = 0.0f; z = 0.0f;
			return;
		}

		// A rotation of 180.0°

		angle = 180.0f;

		float xx = (_elements[ 0] + 1           ) / 2;
		float yy = (_elements[ 5] + 1           ) / 2;
		float zz = (_elements[10] + 1           ) / 2;
		float xy = (_elements[ 4] + _elements[1]) / 4;
		float xz = (_elements[ 8] + _elements[2]) / 4;
		float yz = (_elements[ 9] + _elements[6]) / 4;

		if ((xx > yy) && (xx > zz)) {
			if (xx < epsilon2) {
				x = 0.0f;
				y = 0.7071f;
				z = 0.7071f;
			} else {
				x = sqrtf(xx);
				y = xy / x;
				z = xz / x;
			}
		} else if (yy > zz) {
			if (yy < epsilon2) {
				x = 0.7071f;
				y = 0.0f;
				z = 0.7071f;
			} else {
				y = sqrtf(yy);
				x = xy / y;
				z = yz / y;
			}
		} else {
			if (zz < epsilon2) {
				x = 0.7071f;
				y = 0.7071f;
				z = 0.0f;
			} else {
				z = sqrtf(zz);
				x = xz / z;
				y = yz / z;
			}
		}

		return;
	}

	angle = rad2deg(acos((_elements[0] + _elements[5] + _elements[10] - 1) / 2));

	x = _elements[6] - _elements[9];
	y = _elements[8] - _elements[2];
	z = _elements[1] - _elements[4];

	float r = sqrtf((x * x) + (y * y) + (z * z));
	if (fabs(r) < epsilon1)
		r = 1.0f;

	x /= r;
	y /= r;
	z /= r;
}

void Matrix4x4::getScale(float &x, float &y, float &z) const {
	x = sqrtf(_elements[0] * _elements[0] + _elements[4] * _elements[4] + _elements[ 8] * _elements[ 8]);
	y = sqrtf(_elements[1] * _elements[1] + _elements[5] * _elements[5] + _elements[ 9] * _elements[ 9]);
	z = sqrtf(_elements[2] * _elements[2] + _elements[6] * _elements[6] + _elements[10] * _elements[10]);
}

void Matrix4x4::loadIdentity() {
	std::memcpy(_elements, kIdentity, 16 * sizeof(float));
}

void Matrix4x4::translate(float x, float y, float z) {
	float xx, yy, zz, ww;
	xx = _elements[0] * x + _elements[4] * y + _elements[8]  * z + _elements[12];
	yy = _elements[1] * x + _elements[5] * y + _elements[9]  * z + _elements[13];
	zz = _elements[2] * x + _elements[6] * y + _elements[10] * z + _elements[14];
	ww = _elements[3] * x + _elements[7] * y + _elements[11] * z + _elements[15];
	_elements[12] = xx;
	_elements[13] = yy;
	_elements[14] = zz;
	_elements[15] = ww;
}

void Matrix4x4::translate(const Vector3 &v) {
	/* As a minor optimisation, the 'w' component can be left out. This is safe
	 * if we assume that the matrix in question is not used for perspective
	 * calculations. Generally this is acceptable.
	 * If done, then _elements[15] should be set to 1.0f.
	 * It can also be safely assumed that v._w is 1.0f, for further optimisations.
	 */
	float x, y, z, w;
	x = _elements[0] * v._x + _elements[4] * v._y + _elements[8]  * v._z + _elements[12] * v._w;
	y = _elements[1] * v._x + _elements[5] * v._y + _elements[9]  * v._z + _elements[13] * v._w;
	z = _elements[2] * v._x + _elements[6] * v._y + _elements[10] * v._z + _elements[14] * v._w;
	w = _elements[3] * v._x + _elements[7] * v._y + _elements[11] * v._z + _elements[15] * v._w;
	_elements[12] = x;
	_elements[13] = y;
	_elements[14] = z;
	_elements[15] = w;
}

void Matrix4x4::scale(float x, float y, float z) {
	_elements[0]  *= x;
	_elements[1]  *= x;
	_elements[2]  *= x;
	_elements[3]  *= x;
	_elements[4]  *= y;
	_elements[5]  *= y;
	_elements[6]  *= y;
	_elements[7]  *= y;
	_elements[8]  *= z;
	_elements[9]  *= z;
	_elements[10] *= z;
	_elements[11] *= z;
}

void Matrix4x4::scale(const Vector3 &v) {
	_elements[0]  *= v._x;
	_elements[1]  *= v._x;
	_elements[2]  *= v._x;
	_elements[3]  *= v._x;
	_elements[4]  *= v._y;
	_elements[5]  *= v._y;
	_elements[6]  *= v._y;
	_elements[7]  *= v._y;
	_elements[8]  *= v._z;
	_elements[9]  *= v._z;
	_elements[10] *= v._z;
	_elements[11] *= v._z;

	// v._w should be 1.0f anyway.
	/*
	_elements[12] *= v._w;
	_elements[13] *= v._w;
	_elements[14] *= v._w;
	_elements[15] *= v._w;
	*/
}

void Matrix4x4::rotate(float angle, float x, float y, float z, bool normalise) {
	// Normalize the axis vector
	if (normalise) {
		float length = x * x + y * y + z * z;
		if ((length != 1.0f) && (length != 0.0f)) {
			// Invert length to reduce number of later divisions.
			length = 1.0f / sqrtf(length);

			x *= length;
			y *= length;
			z *= length;
		}
	}

	angle = deg2rad(angle);

	/* Slightly optimised matrix calculation for generic rotation. Note that
	 * SSE implementations might end up being faster, if implemented.
	 */
	float result[16];

	float cosa  = cos(angle);
	float sina  = sin(angle);
	float mcosa = 1.0f - cosa;
	float m0  = (x * x * mcosa) + cosa;
	float m1  = (x * y * mcosa) + (z * sina);
	float m2  = (x * z * mcosa) - (y * sina);

	float m4  = (x * y * mcosa) - (z * sina);
	float m5  = (y * y * mcosa) + cosa;
	float m6  = (y * z * mcosa) + (x * sina);

	float m8  = (x * z * mcosa) + (y * sina);
	float m9  = (y * z * mcosa) - (x * sina);
	float m10 = (z * z * mcosa) + cosa;

	for (int i = 0; i < 4; i++) {
		result[0  + i] = (_elements[i] * m0) + (_elements[i + 4] * m1) + (_elements[i + 8] * m2);
		result[4  + i] = (_elements[i] * m4) + (_elements[i + 4] * m5) + (_elements[i + 8] * m6);
		result[8  + i] = (_elements[i] * m8) + (_elements[i + 4] * m9) + (_elements[i + 8] * m10);
		result[12 + i] = (_elements[i + 12]);
	}
	std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
}

void Matrix4x4::rotateAxisLocal(const Vector3 &vin, float angle, bool normalise) {
	angle = deg2rad(angle);

	/* Slightly optimised matrix calculation for generic rotation. Note that
	 * SSE implementations might end up being faster, if implemented.
	 */
	float result[16];

	Vector3 v(vin);
	if (normalise) {
		v.norm();
	}

	float cosa  = cos(angle);
	float sina  = sin(angle);
	float mcosa = 1.0f - cosa;
	float m0  = (v._x * v._x * mcosa) + cosa;
	float m1  = (v._x * v._y * mcosa) + (v._z * sina);
	float m2  = (v._x * v._z * mcosa) - (v._y * sina);

	float m4  = (v._x * v._y * mcosa) - (v._z * sina);
	float m5  = (v._y * v._y * mcosa) + cosa;
	float m6  = (v._y * v._z * mcosa) + (v._x * sina);

	float m8  = (v._x * v._z * mcosa) + (v._y * sina);
	float m9  = (v._y * v._z * mcosa) - (v._x * sina);
	float m10 = (v._z * v._z * mcosa) + cosa;

	for (int i = 0; i < 4; i++) {
		result[0  + i] = (_elements[i] * m0) + (_elements[i + 4] * m1) + (_elements[i + 8] * m2);
		result[4  + i] = (_elements[i] * m4) + (_elements[i + 4] * m5) + (_elements[i + 8] * m6);
		result[8  + i] = (_elements[i] * m8) + (_elements[i + 4] * m9) + (_elements[i + 8] * m10);
		result[12 + i] = (_elements[i + 12]);
	}
	std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
}

void Matrix4x4::rotateXAxisLocal(float angle, bool normalise) {
	/* x-axis is [1,0,0], hence some optimisations can be made to the
	 * basic arbitrary axis rotation method.
	 */
	if (normalise) {
		// Provide a normalised vector for generic local axis rotation.
		rotateAxisLocal(Vector3(1.0f, 0.0f, 0.0f), angle, false);
	} else {
		// Assume (this) has the rotation matrix already normalised, so use optimised code path.
		angle = deg2rad(angle);
		float result[16];

		float cosa = cos(angle);
		float sina = sin(angle);
		float msina = -sina;
		for (int i = 0; i < 4; i++) {
			result[0 + i]  = (_elements[i]);
			result[4 + i]  = (_elements[i + 4] * cosa)  + (_elements[i + 8] * sina);
			result[8 + i]  = (_elements[i + 4] * msina) + (_elements[i + 8] * cosa);
			result[12 + i] = (_elements[i + 12]);
		}
		std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
	}
}

void Matrix4x4::rotateYAxisLocal(float angle, bool normalise) {
	/* y-axis is [0,1,0], hence some optimisations can be made to the
	 * basic arbitrary axis rotation method.
	 */
	if (normalise) {
		// Provide a normalised vector for generic local axis rotation.
		rotateAxisLocal(Vector3(0.0f, 1.0f, 0.0f), angle, false);
	} else {
		// Assume (this) has the rotation matrix already normalised, so use optimised code path.
		angle = deg2rad(angle);
		float result[16];

		float cosa = cos(angle);
		float sina = sin(angle);
		float msina = -sina;
		for (int i = 0; i < 4; i++) {
			result[0 + i]  = (_elements[i] * cosa) + (_elements[i + 8] * msina);
			result[4 + i]  = (_elements[i + 4]);
			result[8 + i]  = (_elements[i] * sina) + (_elements[i + 8] * cosa);
			result[12 + i] = (_elements[i + 12]);
		}
		std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
	}
}

void Matrix4x4::rotateZAxisLocal(float angle, bool normalise) {
	/* y-axis is [0,1,0], hence some optimisations can be made to the
	 * basic arbitrary axis rotation method.
	 */
	if (normalise) {
		// Provide a normalised vector for generic local axis rotation.
		rotateAxisLocal(Vector3(0.0f, 0.0f, 1.0f), angle, false);
	} else {
		// Assume (this) has the rotation matrix already normalised, so use optimised code path.
		angle = deg2rad(angle);
		float result[16];

		float cosa = cos(angle);
		float sina = sin(angle);
		float msina = -sina;
		for (int i = 0; i < 4; i++) {
			result[0 + i]  = (_elements[i] * cosa)  + (_elements[i + 4] * sina);
			result[4 + i]  = (_elements[i] * msina) + (_elements[i + 4] * cosa);
			result[8 + i]  = (_elements[i + 8]);
			result[12 + i] = (_elements[i + 12]);
		}
		std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
	}
}

void Matrix4x4::rotateAxisWorld(const Vector3 &vin, float angle, bool normalise) {
	angle = deg2rad(angle);

	float result[16];

	Vector3 v(vin._x * _elements[0] + vin._y * _elements[4] + vin._z * _elements[8],
	          vin._x * _elements[1] + vin._y * _elements[5] + vin._z * _elements[9],
	          vin._x * _elements[2] + vin._y * _elements[6] + vin._z * _elements[10]);
	if (normalise) {
		v.norm();
	}

	float cosa  = cos(angle);
	float sina  = sin(angle);
	float mcosa = 1.0f - cosa;
	float m0  = (v._x * v._x * mcosa) + cosa;
	float m1  = (v._x * v._y * mcosa) + (v._z * sina);
	float m2  = (v._x * v._z * mcosa) - (v._y * sina);

	float m4  = (v._x * v._y * mcosa) - (v._z * sina);
	float m5  = (v._y * v._y * mcosa) + cosa;
	float m6  = (v._y * v._z * mcosa) + (v._x * sina);

	float m8  = (v._x * v._z * mcosa) + (v._y * sina);
	float m9  = (v._y * v._z * mcosa) - (v._x * sina);
	float m10 = (v._z * v._z * mcosa) + cosa;

	for (int i = 0; i < 4; i++) {
		result[0 + i]  = (_elements[i] * m0) + (_elements[i + 4] * m1) + (_elements[i + 8] * m2);
		result[4 + i]  = (_elements[i] * m4) + (_elements[i + 4] * m5) + (_elements[i + 8] * m6);
		result[8 + i]  = (_elements[i] * m8) + (_elements[i + 4] * m9) + (_elements[i + 8] * m10);
		result[12 + i] = (_elements[i + 12]);
	}
	std::memcpy(_elements, result, 16 * sizeof(float));  // Copy the rotation into the matrix.
}

void Matrix4x4::rotateXAxisWorld(float angle, bool normalise) {
	this->rotateAxisWorld(Vector3(1.0f, 0.0f, 0.0f), angle, normalise);
}

void Matrix4x4::rotateYAxisWorld(float angle, bool normalise) {
	this->rotateAxisWorld(Vector3(0.0f, 1.0f, 0.0f), angle, normalise);
}

void Matrix4x4::rotateZAxisWorld(float angle, bool normalise) {
	this->rotateAxisWorld(Vector3(0.0f, 0.0f, 1.0f), angle, normalise);
}

void Matrix4x4::setRotation(const Matrix4x4 &m) {
	_elements[0]  = m[0];
	_elements[1]  = m[1];
	_elements[2]  = m[2];

	_elements[4]  = m[4];
	_elements[5]  = m[5];
	_elements[6]  = m[6];

	_elements[8]  = m[8];
	_elements[9]  = m[9];
	_elements[10] = m[10];
}

void Matrix4x4::resetRotation() {
	_elements[0]  = 1.0f;
	_elements[1]  = 0.0f;
	_elements[2]  = 0.0f;

	_elements[4]  = 0.0f;
	_elements[5]  = 1.0f;
	_elements[6]  = 0.0f;

	_elements[8]  = 0.0f;
	_elements[9]  = 0.0f;
	_elements[10] = 1.0f;
}

void Matrix4x4::transform(const Matrix4x4 &m) {
	float result[16];
	for (uint32 i = 0; i < 16; i+=4) {
		// __m128 r, __m128 a, __m128 b
		// a = _mm_load_ps(&_elements[0]);
		// b = _mm_set1_ps(m[i]);
		// r = _mm_mul_ps(a, b);
		result[i + 0] = _elements[0 + 0] * m[i];
		result[i + 1] = _elements[0 + 1] * m[i];
		result[i + 2] = _elements[0 + 2] * m[i];
		result[i + 3] = _elements[0 + 3] * m[i];
		for (uint32 j= 1; j < 4; j++) {
			// a = _mm_load_ps(&_elements[j<<2]);
			// b = _mm_set1_ps(m[i+j]);
			// r = _mm_add_ps(a, b);
			result[i + 0] += _elements[j * 4 + 0] * m[i + j];
			result[i + 1] += _elements[j * 4 + 1] * m[i + j];
			result[i + 2] += _elements[j * 4 + 2] * m[i + j];
			result[i + 3] += _elements[j * 4 + 3] * m[i + j];
		}
		// _mm_store_ps(&result[i], r);
	}
	std::memcpy(_elements, result, 16 * sizeof(float));
}

void Matrix4x4::transform(const Matrix4x4 &a, const Matrix4x4 &b) {
	for (uint32 i = 0; i < 16; i+=4) {
		_elements[i + 0] = a[0 + 0] * b[i];
		_elements[i + 1] = a[0 + 1] * b[i];
		_elements[i + 2] = a[0 + 2] * b[i];
		_elements[i + 3] = a[0 + 3] * b[i];
		for (uint32 j= 1; j < 4; j++) {
			_elements[i + 0] += a[j * 4 + 0] * b[i + j];
			_elements[i + 1] += a[j * 4 + 1] * b[i + j];
			_elements[i + 2] += a[j * 4 + 2] * b[i + j];
			_elements[i + 3] += a[j * 4 + 3] * b[i + j];
		}
	}
}

Matrix4x4 Matrix4x4::getInverse() const {
	Matrix4x4 t(false);
	float A0 = (_elements[ 0] * _elements[ 5]) - (_elements[ 1] * _elements[ 4]);
	float A1 = (_elements[ 0] * _elements[ 6]) - (_elements[ 2] * _elements[ 4]);
	float A2 = (_elements[ 0] * _elements[ 7]) - (_elements[ 3] * _elements[ 4]);
	float A3 = (_elements[ 1] * _elements[ 6]) - (_elements[ 2] * _elements[ 5]);
	float A4 = (_elements[ 1] * _elements[ 7]) - (_elements[ 3] * _elements[ 5]);
	float A5 = (_elements[ 2] * _elements[ 7]) - (_elements[ 3] * _elements[ 6]);
	float B0 = (_elements[ 8] * _elements[13]) - (_elements[ 9] * _elements[12]);
	float B1 = (_elements[ 8] * _elements[14]) - (_elements[10] * _elements[12]);
	float B2 = (_elements[ 8] * _elements[15]) - (_elements[11] * _elements[12]);
	float B3 = (_elements[ 9] * _elements[14]) - (_elements[10] * _elements[13]);
	float B4 = (_elements[ 9] * _elements[15]) - (_elements[11] * _elements[13]);
	float B5 = (_elements[10] * _elements[15]) - (_elements[11] * _elements[14]);

	float det = A0*B5 - A1*B4 + A2*B3 + A3*B2 - A4*B1 + A5*B0;
	if (fabs(det) <= 0.00001f) {
		t.loadIdentity();
		return t;
	}

	det = 1.0f / det;

	t[ 0] = (_elements[ 5] * B5 - _elements[ 6] * B4 + _elements[ 7] * B3) * det;
	t[ 4] = (_elements[ 6] * B2 - _elements[ 7] * B1 - _elements[ 4] * B5) * det;
	t[ 8] = (_elements[ 4] * B4 - _elements[ 5] * B2 + _elements[ 7] * B0) * det;
	t[12] = (_elements[ 5] * B1 - _elements[ 4] * B3 - _elements[ 6] * B0) * det;
	t[ 1] = (_elements[ 2] * B4 - _elements[ 3] * B3 - _elements[ 1] * B5) * det;
	t[ 5] = (_elements[ 0] * B5 - _elements[ 2] * B2 + _elements[ 3] * B1) * det;
	t[ 9] = (_elements[ 1] * B2 - _elements[ 3] * B0 - _elements[ 0] * B4) * det;
	t[13] = (_elements[ 0] * B3 - _elements[ 1] * B1 + _elements[ 2] * B0) * det;
	t[ 2] = (_elements[13] * A5 - _elements[14] * A4 + _elements[15] * A3) * det;
	t[ 6] = (_elements[14] * A2 - _elements[15] * A1 - _elements[12] * A5) * det;
	t[10] = (_elements[12] * A4 - _elements[13] * A2 + _elements[15] * A0) * det;
	t[14] = (_elements[13] * A1 - _elements[12] * A3 - _elements[14] * A0) * det;
	t[ 3] = (_elements[10] * A4 - _elements[11] * A3 - _elements[ 9] * A5) * det;
	t[ 7] = (_elements[ 8] * A5 - _elements[10] * A2 + _elements[11] * A1) * det;
	t[11] = (_elements[ 9] * A2 - _elements[11] * A0 - _elements[ 8] * A4) * det;
	t[15] = (_elements[ 8] * A3 - _elements[ 9] * A1 + _elements[10] * A0) * det;

	return t;
}

Matrix4x4 Matrix4x4::getTranspose() const {
	Matrix4x4 t(false);
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			t[(i << 2) + j] = _elements[(j << 2) + i];
		}
	}
	return t;
}

void Matrix4x4::lookAt(const Vector3 &v) {
	/* [x,y,z] is the z-axis vector. Cross this with [0,1,0] to create the x-axis, and
	 * calculate the y-axis with (z-axis x x-axis).
	 * Most of this was taken from stock standard gluLookAt components, however there's
	 * one crucial difference. gluLookAt assumes that the world is rotated (standard
	 * OpenGL stuff); this assumes rotating some object.
	 */
	Vector3 z(v);
	Vector3 y(0.0f, 1.0f, 0.0f);
	Vector3 x;

	z.norm();

	if (((z._x * z._x) < 0.00001f) && ((z._z * z._z) < 0.00001f)) {
		y._y = 0.0f;
		y._x = -1.0f;
	}

	x = y * z;//z * y;
	x.norm();

	y = z*x;//x * z;
	//y.norm();  // No need to normalise - x and z both are, and are both perpendicular to y.

	_elements[0] = x._x;  _elements[1] = x._y;  _elements[2] = x._z;
	_elements[4] = y._x;  _elements[5] = y._y;  _elements[6] = y._z;
	_elements[8] = z._x;  _elements[9] = z._y;  _elements[10] = z._z;
}

void Matrix4x4::perspective(float fovy, float aspectRatio, float znear, float zfar) {
	float xmin, xmax, ymin, ymax;
	Matrix4x4 pMatrix(false);
	std::memset(&pMatrix, 0, 16 * sizeof(float));

	ymax = znear * tan(deg2rad(0.5f * fovy));
	ymin = -ymax;
	xmax = ymax * aspectRatio;
	xmin = ymin * aspectRatio;

	pMatrix[0]  = (2.0f * znear) / (xmax - xmin);
	pMatrix[5]  = (2.0f * znear) / (ymax - ymin);
	pMatrix[8]  = (xmax + xmin)  / (xmax - xmin);
	pMatrix[9]  = (ymax + ymin)  / (ymax - ymin);
	pMatrix[10] = -((zfar + znear) / (zfar - znear));
	pMatrix[11] = -1.0f;
	pMatrix[14] = (-2.0f * zfar * znear) / (zfar - znear);

	this->transform(pMatrix);
}

void Matrix4x4::ortho(float l, float r, float b, float t, float n, float f)
{
	Matrix4x4 mMatrix(false);
	std::memset(&mMatrix, 0, 16*sizeof(float));

	mMatrix[0]  =  2.0f / (r - l);
	mMatrix[5]  =  2.0f / (t - b);
	mMatrix[10] = -2.0f / (f - n);
	mMatrix[12] = -((r+l) / (r-l));
	mMatrix[13] = -((t+b) / (t-b));
	mMatrix[14] = -((f+n) / (f-n));
	mMatrix[15] =  1.0f;

	this->transform(mMatrix);
}

void Matrix4x4::multiply(const float *vin, float *vout) const {
	vout[0] = vin[0] * _elements[ 0] + vin[1] * _elements[ 4] + vin[2] * _elements[ 8] + _elements[12];
	vout[1] = vin[0] * _elements[ 1] + vin[1] * _elements[ 5] + vin[2] * _elements[ 9] + _elements[13];
	vout[2] = vin[0] * _elements[ 2] + vin[1] * _elements[ 6] + vin[2] * _elements[10] + _elements[14];
	float w = vin[0] * _elements[ 3] + vin[1] * _elements[ 7] + vin[2] * _elements[11] + _elements[15];
	vout[0] /= w;
	vout[1] /= w;
	vout[2] /= w;
}

const Matrix4x4 &Matrix4x4::operator=(const float *m) {
	std::memcpy(_elements, m, 16 * sizeof(float));
	return *this;
}

float &Matrix4x4::operator[](unsigned int index) {
	return _elements[index];
}

float  Matrix4x4::operator[](unsigned int index) const {
	return _elements[index];
}

float &Matrix4x4::operator()(int row, int column) {
	return _elements[(column << 2) + row];
}

float  Matrix4x4::operator()(int row, int column) const {
	return _elements[(column << 2) + row];
}

const Matrix4x4 &Matrix4x4::operator*=(const Matrix4x4 &m) {
	this->transform(m);
	return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &m) const {
	return Matrix4x4(*this) *= m;
}

Vector3 Matrix4x4::operator*(const Vector3 &v) const {
	return Vector3(v._x * _elements[ 0] + v._y * _elements[ 4] + v._z * _elements[ 8] + v._w * _elements[12],
	               v._x * _elements[ 1] + v._y * _elements[ 5] + v._z * _elements[ 9] + v._w * _elements[13],
	               v._x * _elements[ 2] + v._y * _elements[ 6] + v._z * _elements[10] + v._w * _elements[14],
	               v._x * _elements[ 3] + v._y * _elements[ 7] + v._z * _elements[11] + v._w * _elements[15]);
}

Vector3 Matrix4x4::vectorRotate(const Vector3 &v) const {
	return Vector3(v._x * _elements[0] + v._y * _elements[4] + v._z * _elements[8],
	               v._x * _elements[1] + v._y * _elements[5] + v._z * _elements[9],
	               v._x * _elements[2] + v._y * _elements[6] + v._z * _elements[10]);
}

Vector3 Matrix4x4::vectorRotateReverse(const Vector3 &v) const {
	return Vector3(v._x * _elements[0] + v._y * _elements[1] + v._z * _elements[2],
	               v._x * _elements[4] + v._y * _elements[5] + v._z * _elements[6],
	               v._x * _elements[8] + v._y * _elements[9] + v._z * _elements[10]);
}

} // End of namespace Common
