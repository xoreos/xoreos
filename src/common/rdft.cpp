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

// Based upon the (I)RDFT code in FFmpeg
// Copyright (c) 2009 Alex Converse <alex dot converse at gmail dot com>

/** @file
 *  (Inverse) Real Discrete Fourier Transform.
 */

#include <cassert>

#include "src/common/maths.h"
#include "src/common/sinetables.h"
#include "src/common/cosinetables.h"
#include "src/common/fft.h"
#include "src/common/rdft.h"

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

	const float k1 = 0.5f;
	const float k2 = 0.5f - (_inverse ? 1.0f : 0.0f);

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
