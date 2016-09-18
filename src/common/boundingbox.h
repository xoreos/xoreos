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

#ifndef COMMON_BOUNDINGBOX_H
#define COMMON_BOUNDINGBOX_H

#include "src/common/matrix4x4.h"

namespace Common {

/** A bounding box around 3D points. */
class BoundingBox {
public:
	BoundingBox();
	~BoundingBox();

	void clear();

	bool empty() const;

	const Matrix4x4 &getOrigin() const;

	void getMin(float &x, float &y, float &z) const;
	void getMax(float &x, float &y, float &z) const;

	float getWidth () const; ///< Get the width of the bounding box.
	float getHeight() const; ///< Get the height of the bounding box.
	float getDepth () const; ///< Get the depth of the bounding box.

	bool isIn(float x, float y         ) const;
	bool isIn(float x, float y, float z) const;

	bool isIn(float x1, float y1, float z1, float x2, float y2, float z2) const;

	void add(float x, float y, float z);
	void add(const BoundingBox &box);

	void translate(float x, float y, float z);
	void scale    (float x, float y, float z);

	void rotate(float angle, float x, float y, float z);

	void transform(const Matrix4x4 &m);

	/** Apply the origin transformations directly to the coordinates. */
	void absolutize();

	/** Return a copy with the origin transformations directly applied to the coordinates. */
	BoundingBox getAbsolute() const;

private:
	bool _empty;
	bool _absolute;

	Matrix4x4 _origin;

	float _coords[8][3];

	float _min[3];
	float _max[3];

	inline float getCoordMin(int i) const;
	inline float getCoordMax(int i) const;

	bool getIntersection(float fDst1, float fDst2,
	                     float x1, float y1, float z1,
	                     float x2, float y2, float z2,
	                     float &x, float &y, float &z) const;
	bool inBox(float x, float y, float z, float minX, float minY, float minZ,
	           float maxX, float maxY, float maxZ, int axis) const;
};

} // End of namespace Common

#endif // COMMON_BOUNDINGBOX_H
