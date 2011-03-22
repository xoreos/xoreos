/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/transmatrix.cpp
 *  A transformation matrix.
 */

#include "common/transmatrix.h"
#include "common/maths.h"

namespace Common {

TransformationMatrix::TransformationMatrix() : Matrix(4, 4) {
	loadIdentity();
}

float TransformationMatrix::getX() const {
	return _elements[3];
}

float TransformationMatrix::getY() const {
	return _elements[7];
}

float TransformationMatrix::getZ() const {
	return _elements[11];
}

void TransformationMatrix::get(float &x, float &y, float &z) const {
	x = getX();
	y = getY();
	z = getZ();
}

void TransformationMatrix::loadIdentity() {
	_elements[ 0] = 1.0; _elements[ 1] = 0.0; _elements[ 2] = 0.0; _elements[ 3] = 0.0;
	_elements[ 4] = 0.0; _elements[ 5] = 1.0; _elements[ 6] = 0.0; _elements[ 7] = 0.0;
	_elements[ 8] = 0.0; _elements[ 9] = 0.0; _elements[10] = 1.0; _elements[11] = 0.0;
	_elements[12] = 0.0; _elements[13] = 0.0; _elements[14] = 0.0; _elements[15] = 1.0;
}

void TransformationMatrix::translate(float x, float y, float z) {
	Matrix tMatrix(4, 4);

	tMatrix[0][0] = 1.0; tMatrix[0][1] = 0.0; tMatrix[0][2] = 0.0; tMatrix[0][3] =   x;
	tMatrix[1][0] = 0.0; tMatrix[1][1] = 1.0; tMatrix[1][2] = 0.0; tMatrix[1][3] =   y;
	tMatrix[2][0] = 0.0; tMatrix[2][1] = 0.0; tMatrix[2][2] = 1.0; tMatrix[2][3] =   z;
	tMatrix[3][0] = 0.0; tMatrix[3][1] = 0.0; tMatrix[3][2] = 0.0; tMatrix[3][3] = 1.0;

	(*this) *= tMatrix;
}

void TransformationMatrix::scale(float x, float y, float z) {
	Matrix sMatrix(4, 4);

	sMatrix[0][0] =   x; sMatrix[0][1] = 0.0; sMatrix[0][2] = 0.0; sMatrix[0][3] = 0.0;
	sMatrix[1][0] = 0.0; sMatrix[1][1] =   y; sMatrix[1][2] = 0.0; sMatrix[1][3] = 0.0;
	sMatrix[2][0] = 0.0; sMatrix[2][1] = 0.0; sMatrix[2][2] =   z; sMatrix[2][3] = 0.0;
	sMatrix[3][0] = 0.0; sMatrix[3][1] = 0.0; sMatrix[3][2] = 0.0; sMatrix[3][3] = 1.0;

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

	float c = cosf(deg2rad(angle));
	float s = sinf(deg2rad(angle));

	// Elements for the rotation matrix
	float e11 = (x * x) * (1.0 - c) +     c;
	float e12 = (x * y) * (1.0 - c) - z * s;
	float e13 = (x * z) * (1.0 - c) + y * s;
	float e21 = (y * x) * (1.0 - c) + z * s;
	float e22 = (y * y) * (1.0 - c) +     c;
	float e23 = (y * z) * (1.0 - c) - x * s;
	float e31 = (z * x) * (1.0 - c) - y * s;
	float e32 = (z * y) * (1.0 - c) + x * s;
	float e33 = (z * z) * (1.0 - c) +     c;

	Matrix rMatrix(4, 4);

	rMatrix[0][0] = e11; rMatrix[0][1] = e12; rMatrix[0][2] = e13; rMatrix[0][3] = 0.0;
	rMatrix[1][0] = e21; rMatrix[1][1] = e22; rMatrix[1][2] = e23; rMatrix[1][3] = 0.0;
	rMatrix[2][0] = e31; rMatrix[2][1] = e32; rMatrix[2][2] = e33; rMatrix[2][3] = 0.0;
	rMatrix[3][0] = 0.0; rMatrix[3][1] = 0.0; rMatrix[3][2] = 0.0; rMatrix[3][3] = 1.0;

	(*this) *= rMatrix;
}

void TransformationMatrix::transform(const Matrix &m) {
	(*this) *= m;
}

} // End of namespace Common
