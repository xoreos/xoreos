/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

	void getGLMatrix(float *m) const;
	void setGLMatrix(const float *m);

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
