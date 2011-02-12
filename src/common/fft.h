/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Based upon the (I)FFT code in FFmpeg
// Copyright (c) 2008 Loren Merritt
// Copyright (c) 2002 Fabrice Bellard
// Partly based on libdjbfft by D. J. Bernstein

/** @file common/fft.h
 *  (Inverse) Fast Fourier Transform.
 */

#ifndef COMMON_FFT_H
#define COMMON_FFT_H

#include "common/types.h"
#include "common/maths.h"

namespace Common {

/** (Inverse) Fast Fourier Transform. */
class FFT {
public:
	FFT(int bits, int inverse);
	~FFT();

	/** Do the permutation needed BEFORE calling calc(). */
	void permute(Complex *z);

	/** Do a complex FFT.
	 *
	 *  The input data must be permuted before.
	 *  No 1.0/sqrt(n) normalization is done.
	 */
	void calc(Complex *z);

private:
	int _bits;
	int _inverse;

	uint16 *_revTab;

	Complex *_expTab;
	Complex *_tmpBuf;

	const float *_tSin;
	const float *_tCos;

	int _splitRadix;
	int _permutation;

	static int splitRadixPermutation(int i, int n, int inverse);
};

} // End of namespace Common

#endif // COMMON_FFT_H
