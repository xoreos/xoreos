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

/** @file common/transmatrix.h
 *  A transformation matrix.
 */

#ifndef COMMON_TRANSMATRIX_H
#define COMMON_TRANSMATRIX_H

#include "common/matrix.h"

namespace Common {

/** A transformation matrix. */
class TransformationMatrix : public Matrix {
public:
	TransformationMatrix();

	float getX() const;
	float getY() const;
	float getZ() const;

	void getPosition(float &x, float &y, float &z) const;

	void loadIdentity();

	void translate(float x, float y, float z);
	void scale    (float x, float y, float z);

	void rotate(float angle, float x, float y, float z);

	void transform(const Matrix &m);
};

} // End of namespace Common

#endif // COMMON_TRANSMATRIX_H
