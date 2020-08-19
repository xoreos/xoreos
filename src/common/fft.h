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
 *  (Inverse) Fast Fourier Transform.
 */

/* Based on the (I)FFT code in FFmpeg (<https://ffmpeg.org/)>, which
 * is released under the terms of version 2 or later of the GNU Lesser
 * General Public License.
 *
 * The original copyright note in libavcodec/fft_template.c reads as follows:
 *
 * FFT/IFFT transforms
 * Copyright (c) 2008 Loren Merritt
 * Copyright (c) 2002 Fabrice Bellard
 * Partly based on libdjbfft by D. J. Bernstein
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef COMMON_FFT_H
#define COMMON_FFT_H

#include <memory>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"

namespace Common {

struct Complex;

/** (Inverse) Fast Fourier Transform. */
class FFT : boost::noncopyable {
public:
	FFT(int bits, bool inverse);
	~FFT();

	const uint16_t *getRevTab() const;

	/** Do the permutation needed BEFORE calling calc(). */
	void permute(Complex *z);

	/** Do a complex FFT.
	 *
	 *  The input data must be permuted before.
	 *  No 1.0/sqrt(n) normalization is done.
	 */
	void calc(Complex *z);

private:
	int  _bits;
	bool _inverse;

	std::unique_ptr<uint16_t[]> _revTab;

	std::unique_ptr<Complex[]> _expTab;
	std::unique_ptr<Complex[]> _tmpBuf;

	static int splitRadixPermutation(int i, int n, bool inverse);
};

} // End of namespace Common

#endif // COMMON_FFT_H
