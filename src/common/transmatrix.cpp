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
 */

/** @file common/transmatrix.cpp
 *  A transformation matrix.
 */

#include "common/transmatrix.h"
#include "common/maths.h"

static const float kIdentity[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

namespace Common {

TransformationMatrix::TransformationMatrix() : Matrix(4, 4) {
	loadIdentity();
}

float TransformationMatrix::getX() const {
	return _elements[12];
}

float TransformationMatrix::getY() const {
	return _elements[13];
}

float TransformationMatrix::getZ() const {
	return _elements[14];
}

void TransformationMatrix::getPosition(float &x, float &y, float &z) const {
	x = getX();
	y = getY();
	z = getZ();
}

void TransformationMatrix::loadIdentity() {
	set(kIdentity);
}

void TransformationMatrix::translate(float x, float y, float z) {
	Matrix tMatrix(4, 4);

	tMatrix.set(kIdentity);

	tMatrix(0, 3) = x;
	tMatrix(1, 3) = y;
	tMatrix(2, 3) = z;

	(*this) *= tMatrix;
}

void TransformationMatrix::scale(float x, float y, float z) {
	Matrix sMatrix(4, 4);

	sMatrix.set(kIdentity);

	sMatrix(0, 0) = x;
	sMatrix(1, 1) = y;
	sMatrix(2, 2) = z;

	(*this) *= sMatrix;
}

void TransformationMatrix::rotate(float angle, float x, float y, float z) {
	// Normalize the axis vector
	float length = x * x + y * y + z * z;
	if ((length != 1.0) && (length != 0.0)) {
		length = sqrtf(length);

		x /= length;
		y /= length;
		z /= length;
	}

	const float c = cosf(deg2rad(angle));
	const float s = sinf(deg2rad(angle));

	float e[16];

	// Elements for the rotation matrix
	e[ 0] = (x * x) * (1.0 - c) +     c;
	e[ 1] = (y * x) * (1.0 - c) + z * s;
	e[ 2] = (z * x) * (1.0 - c) - y * s;
	e[ 3] = 0.0;
	e[ 4] = (x * y) * (1.0 - c) - z * s;
	e[ 5] = (y * y) * (1.0 - c) +     c;
	e[ 6] = (z * y) * (1.0 - c) + x * s;
	e[ 7] = 0.0;
	e[ 8] = (x * z) * (1.0 - c) + y * s;
	e[ 9] = (y * z) * (1.0 - c) - x * s;
	e[10] = (z * z) * (1.0 - c) +     c;
	e[11] = 0.0;
	e[12] = 0.0;
	e[13] = 0.0;
	e[14] = 0.0;
	e[15] = 1.0;

	Matrix rMatrix(4, 4);

	rMatrix.set(e);

	(*this) *= rMatrix;
}

void TransformationMatrix::transform(const Matrix &m) {
	(*this) *= m;
}

} // End of namespace Common
