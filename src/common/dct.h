/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Based upon the (I)DCT code in FFmpeg
// Copyright (c) 2009 Peter Ross <pross@xvid.org>
// Copyright (c) 2010 Alex Converse <alex.converse@gmail.com>
// Copyright (c) 2010 Vitor Sessak

/** @file common/dct.h
 *  (Inverse) Discrete Cosine Transforms.
 */

#ifndef COMMON_DCT_H
#define COMMON_DCT_H

#include "common/types.h"
#include "common/maths.h"
#include "common/rdft.h"

namespace Common {

/** (Inverse) Discrete Cosine Transforms. */
class DCT {
public:
	enum TransformType {
		DCT_II,
		DCT_III,
		DCT_I,
		DST_I
	};

	DCT(int bits, TransformType trans);
	~DCT();

	void calc(float *data);

private:
	int _bits;
	TransformType _trans;

	const float *_tCos;

	float *_csc2;

	RDFT *_rdft;

	void calcDCTI  (float *data);
	void calcDCTII (float *data);
	void calcDCTIII(float *data);
	void calcDSTI  (float *data);
};

} // End of namespace Common

#endif // COMMON_DCT_H
