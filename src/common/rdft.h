/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Based upon the (I)RDFT code in FFmpeg
// Copyright (c) 2009 Alex Converse <alex dot converse at gmail dot com>

/** @file common/rdft.h
 *  (Inverse) Real Discrete Fourier Transform.
 */

#ifndef COMMON_RDFT_H
#define COMMON_RDFT_H

#include "common/types.h"
#include "common/maths.h"
#include "common/fft.h"

namespace Common {

/** (Inverse) Real Discrete Fourier Transform. */
class RDFT {
public:
	enum TransformType {
		DFT_R2C,
		IDFT_C2R,
		IDFT_R2C,
		DFT_C2R
	};

	RDFT(int bits, TransformType trans);
	~RDFT();

	void calc(float *data);

private:
	int _bits;
	int _inverse;
	int _signConvention;

	const float *_tSin;
	const float *_tCos;

	FFT *_fft;
};

} // End of namespace Common

#endif // COMMON_RDFT_H
