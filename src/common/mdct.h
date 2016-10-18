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
 *  (Inverse) Modified Discrete Cosine Transforms.
 */

/* Based on the (I)MDCT code in FFmpeg (<https://ffmpeg.org/)>, which
 * is released under the terms of version 2 or later of the GNU Lesser
 * General Public License.
 *
 * The original copyright note in libavcodec/mdct_template.c reads as follows:
 *
 * MDCT/IMDCT transforms
 * Copyright (c) 2002 Fabrice Bellard
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

#ifndef COMMON_MDCT_H
#define COMMON_MDCT_H

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/scopedptr.h"

namespace Common {

class FFT;

/** (Inverse) Modified Discrete Cosine Transforms. */
class MDCT : boost::noncopyable {
public:
	MDCT(int bits, bool inverse, double scale);
	~MDCT();

	/** Compute MDCT of size N = 2^nbits. */
	void calcMDCT(float *output, const float *input);

	/** Compute inverse MDCT of size N = 2^nbits. */
	void calcIMDCT(float *output, const float *input);

private:
	int _bits;
	int _size;

	ScopedArray<float> _tCos;
	float *_tSin;

	ScopedPtr<FFT> _fft;

	/** Compute the middle half of the inverse MDCT of size N = 2^nbits,
	 *  thus excluding the parts that can be derived by symmetry.
	 */
	void calcHalfIMDCT(float *output, const float *input);
};

} // End of namespace Common

#endif // COMMON_MDCT_H
