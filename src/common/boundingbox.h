/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/boundingbox.h
 *  A bounding box.
 */

#ifndef COMMON_BOUNDINGBOX_H
#define COMMON_BOUNDINGBOX_H

#include "common/transmatrix.h"

namespace Common {

/** A bouding box around 3D points. */
class BoundingBox {
public:
	BoundingBox();
	~BoundingBox();

	void clear();

	bool isEmpty() const;

	const Common::TransformationMatrix &getOrigin() const;

	void getMin(float &x, float &y, float &z) const;
	void getMax(float &x, float &y, float &z) const;

	float getWidth () const; ///< Get the width of the bounding box.
	float getHeight() const; ///< Get the height of the bounding box.
	float getDepth () const; ///< Get the depth of the bounding box.

	bool isIn(float x, float y         ) const;
	bool isIn(float x, float y, float z) const;

	void add(float x, float y, float z);
	void add(const BoundingBox &box);

	void translate(float x, float y, float z);
	void scale    (float x, float y, float z);

	void rotate(float angle, float x, float y, float z);

	/** Apply the origin transformations directly to the coordinates. */
	void absolutize();

	/** Return a copy with the origin transformations directly applied to the coordinates. */
	BoundingBox getAbsolute() const;

private:
	bool _empty;

	TransformationMatrix _origin;

	float _coords[8][3];

	float _min[3];
	float _max[3];

	inline float getCoordMin(int i) const;
	inline float getCoordMax(int i) const;
};

} // End of namespace Common

#endif // COMMON_BOUNDINGBOX_H
