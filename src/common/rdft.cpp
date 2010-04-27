/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Based upon the (I)RDFT code in FFmpeg
// Copyright (c) 2009 Alex Converse <alex dot converse at gmail dot com>

/** @file common/rdft.cpp
 *  (Inverse) Real Discrete Fourier Transform.
 */

#include <cassert>

#include "common/rdft.h"

namespace Common {

RDFT::RDFT(int bits, TransformType trans) : _bits(bits), _fft(0) {
	assert ((_bits >= 4) && (_bits <= 16));

	_inverse        = trans == IDFT_C2R || trans == DFT_C2R;
	_signConvention = trans == IDFT_R2C || trans == DFT_C2R ? 1 : -1;

	_fft = new FFT(bits - 1, trans == IDFT_C2R || trans == IDFT_R2C);

	int n = 1 << bits;

	_tSin = getSineTable(bits) + (trans == DFT_R2C || trans == DFT_C2R) * (n >> 2);
	_tCos = getCosineTable(bits);
}

RDFT::~RDFT() {
	delete _fft;
}

void RDFT::calc(float *data) {
	const int n = 1 << _bits;

	const float k1 = 0.5;
	const float k2 = 0.5 - _inverse;

	if (!_inverse) {
		_fft->permute((Complex *) data);
		_fft->calc   ((Complex *) data);
	}

	Complex ev, od;

	/* i=0 is a special case because of packing, the DC term is real, so we
	   are going to throw the N/2 term (also real) in with it. */

	ev.re = data[0];

	data[0] = ev.re + data[1];
	data[1] = ev.re - data[1];

	int i;
	for (i = 1; i < (n >> 2); i++) {
		int i1 = 2 * i;
		int i2 = n - i1;

		/* Separate even and odd FFTs */
		ev.re =  k1 * (data[i1    ] + data[i2   ]);
		od.im = -k2 * (data[i1    ] - data[i2   ]);
		ev.im =  k1 * (data[i1 + 1] - data[i2 + 1]);
		od.re =  k2 * (data[i1 + 1] + data[i2 + 1]);

		/* Apply twiddle factors to the odd FFT and add to the even FFT */
		data[i1    ] =  ev.re + od.re * _tCos[i] - od.im * _tSin[i];
		data[i1 + 1] =  ev.im + od.im * _tCos[i] + od.re * _tSin[i];
		data[i2    ] =  ev.re - od.re * _tCos[i] + od.im * _tSin[i];
		data[i2 + 1] = -ev.im + od.im * _tCos[i] + od.re * _tSin[i];
	}

	data[2 * i + 1] = _signConvention * data[2 * i + 1];

	if (_inverse) {
		data[0] *= k1;
		data[1] *= k1;

		_fft->permute((Complex *) data);
		_fft->calc   ((Complex *) data);
	}

}

} // End of namespace Common
