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

#ifndef COMMON_MATRIX4X4_H
#define COMMON_MATRIX4X4_H

#include "src/common/vector3.h"

namespace Common {

/** A 4x4 matrix. */
class Matrix4x4 {
public:
	/** If set true, load identity matrix. Allows removal of redundant
	 *  assignment of transform internal array in some cases.
	 */
	Matrix4x4(bool identity = true);
	Matrix4x4(const float *m);
	~Matrix4x4();

	/** Get the matrix elements in a flat array, column-major order. */
	const float *get() const;

	float getX() const;
	float getY() const;
	float getZ() const;

	void getPosition(float &x, float &y, float &z) const;
	const float *getPosition() const;

	const float *getXAxis() const;
	const float *getYAxis() const;
	const float *getZAxis() const;

	void getAxisAngle(float &angle, float &x, float &y, float &z) const;
	void getScale(float &x, float &y, float &z) const;

	void loadIdentity();

	void translate(float x, float y, float z);
	void translate(const Vector3 &v);
	void scale    (float x, float y, float z);
	void scale    (const Vector3 &v);

	void rotate(float angle, float x, float y, float z, bool normalise = true);
	void rotateAxisLocal(const Vector3 &vin, float angle, bool normalise = true);
	void rotateXAxisLocal(float angle, bool normalise = true);
	void rotateYAxisLocal(float angle, bool normalise = true);
	void rotateZAxisLocal(float angle, bool normalise = true);
	void rotateAxisWorld(const Vector3 &vin, float angle, bool normalise = true);
	void rotateXAxisWorld(float angle, bool normalise = true);
	void rotateYAxisWorld(float angle, bool normalise = true);
	void rotateZAxisWorld(float angle, bool normalise = true);
	void setRotation(const Matrix4x4 &m);
	void resetRotation();

	/** Multiply this matrix by the provided, store result in this matrix. */
	void transform(const Matrix4x4 &m);

	/** Multiply matrices A.B, store result in this matrix. */
	void transform(const Matrix4x4 &a, const Matrix4x4 &b);

	/** Calculate the inverse matrix, return a copy. */
	Matrix4x4 getInverse() const;

	/** Returns a transposed matrix of this. */
	Matrix4x4 getTranspose() const;

	/** Rotate matrix to look at the given point. */
	void lookAt(const Vector3 &v);

	void perspective(float fovy, float aspectRatio, float znear, float zfar);
	void ortho(float l, float r, float b, float t, float n, float f);

	/** Multiply this matrix by the provided, store result in the specified matrix.
	 *  @param vin Pointer to a 16-value array containing the matrix to multiply by.
	 *  @param vout Pointer to a 16-value array to store the resulting matrix.
	 */
	void multiply(const float *vin, float *vout) const;

	const Matrix4x4 &operator=(const float *m);

	float &operator[](unsigned int index);
	float  operator[](unsigned int index) const;
	float &operator()(int row, int column);
	float  operator()(int row, int column) const;

	const Matrix4x4 &operator*=(const Matrix4x4 &m);
	Matrix4x4 operator*(const Matrix4x4 &m) const;
	Vector3 operator*(const Vector3 &v) const;

	Vector3 vectorRotate(const Vector3 &v) const;
	Vector3 vectorRotateReverse(const Vector3 &v) const;

private:
	float _elements[16];
};

} // End of namespace Common

#endif // COMMON_MATRIX4X4_H
