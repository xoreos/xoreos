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
 *  A transformation matrix.
 */

#ifndef COMMON_TRANSMATRIX_H
#define COMMON_TRANSMATRIX_H

#include "src/common/matrix.h"
#include "src/common/vector3.h"

namespace Common {

/** A transformation matrix. */
class TransformationMatrix {
public:
	/** If set true, load identity matrix. Allows removal of redundant
	 *  assignment of transform internal array in some cases.
	 */
	TransformationMatrix(bool identity = true);
	TransformationMatrix(const TransformationMatrix &m);
	TransformationMatrix(const float *m);
	~TransformationMatrix();

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
	void setRotation(const TransformationMatrix &m);
	void resetRotation();

	/** Multiply this matrix by the provided, store result in this matrix. */
	void transform(const TransformationMatrix &m);

	/** Multiply matrices A.B, store result in this matrix. */
	void transform(const TransformationMatrix &a, const TransformationMatrix &b);

	/** Calculate the inverse matrix, return a copy. */
	TransformationMatrix getInverse();

	/** Returns a transposed matrix of this. */
	TransformationMatrix getTranspose();

	/** Rotate matrix to look at the given point. */
	void lookAt(const Vector3 &v);

	void perspective(float fovy, float aspectRatio, float znear, float zfar);
	void ortho(float l, float r, float b, float t, float n, float f);

	const TransformationMatrix &operator=(const TransformationMatrix &m);
	const TransformationMatrix &operator=(const float *m);

	float &operator[](unsigned int index);
	float  operator[](unsigned int index) const;
	float &operator()(int row, int column);
	float  operator()(int row, int column) const;

	const TransformationMatrix &operator*=(const TransformationMatrix &m);
	TransformationMatrix operator*(const TransformationMatrix &m) const;
	Vector3 operator*(const Vector3 &v) const;

	Vector3 vectorRotate(Vector3 &v) const;
	Vector3 vectorRotateReverse(Vector3 &v) const;

private:
	float _elements[16];
};

} // End of namespace Common

#endif // COMMON_TRANSMATRIX_H
