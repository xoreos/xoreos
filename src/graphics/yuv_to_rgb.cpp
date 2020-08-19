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
 *  Efficient YUV to RGB conversion.
 */

// The YUV to RGB conversion code is derived from ScummVM's YUV conversion code,
// which is derived from SDL's YUV overlay code, which in turn appears to be
// derived from mpeg_play. The following copyright notices have been included
// in accordance with the original license. Please note that the term "software"
// in this context only applies to the YUVToRGBLookup constructor, YUVToRGBManager
// constructor and convert420() functions below.

// Copyright (c) 1995 The Regents of the University of California.
// All rights reserved.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose, without fee, and without written agreement is
// hereby granted, provided that the above copyright notice and the following
// two paragraphs appear in all copies of this software.
//
// IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
// OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
// CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
// ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

// Copyright (c) 1995 Erik Corry
// All rights reserved.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose, without fee, and without written agreement is
// hereby granted, provided that the above copyright notice and the following
// two paragraphs appear in all copies of this software.
//
// IN NO EVENT SHALL ERIK CORRY BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
// SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF
// THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF ERIK CORRY HAS BEEN ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ERIK CORRY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND ERIK CORRY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
// UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

// Portions of this software Copyright (c) 1995 Brown University.
// All rights reserved.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose, without fee, and without written agreement
// is hereby granted, provided that the above copyright notice and the
// following two paragraphs appear in all copies of this software.
//
// IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
// OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF BROWN
// UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// BROWN UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND BROWN UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

#include "src/common/error.h"
#include "src/common/singleton.h"
#include "src/common/util.h"

#include "src/graphics/yuv_to_rgb.h"

DECLARE_SINGLETON(Graphics::YUVToRGBManager)

namespace Graphics {

class YUVToRGBLookup {
public:
	YUVToRGBLookup(YUVToRGBManager::LuminanceScale scale);

	YUVToRGBManager::LuminanceScale getScale() const { return _scale; }
	const byte *getRGBToPix() const { return _rgbToPix; }

private:
	YUVToRGBManager::LuminanceScale _scale;
	byte _rgbToPix[3 * 768];
};

YUVToRGBLookup::YUVToRGBLookup(YUVToRGBManager::LuminanceScale scale) {
	_scale = scale;

	byte *r_2_pix_alloc = &_rgbToPix[0 * 768];
	byte *g_2_pix_alloc = &_rgbToPix[1 * 768];
	byte *b_2_pix_alloc = &_rgbToPix[2 * 768];

	if (scale == YUVToRGBManager::kScaleFull) {
		// Set up entries 0-255 in rgb-to-pixel value tables.
		for (int i = 0; i < 256; i++) {
			r_2_pix_alloc[i + 256] = i;
			g_2_pix_alloc[i + 256] = i;
			b_2_pix_alloc[i + 256] = i;
		}

		// Spread out the values we have to the rest of the array so that we do
		// not need to check for overflow.
		for (int i = 0; i < 256; i++) {
			r_2_pix_alloc[i] = r_2_pix_alloc[256];
			r_2_pix_alloc[i + 512] = r_2_pix_alloc[511];
			g_2_pix_alloc[i] = g_2_pix_alloc[256];
			g_2_pix_alloc[i + 512] = g_2_pix_alloc[511];
			b_2_pix_alloc[i] = b_2_pix_alloc[256];
			b_2_pix_alloc[i + 512] = b_2_pix_alloc[511];
		}
	} else {
		// Set up entries 0-255 in rgb-to-pixel value tables.
		for (int i = 16; i < 236; i++) {
			int scaledValue = (i - 16) * 255 / 219;
			r_2_pix_alloc[i + 256] = scaledValue;
			g_2_pix_alloc[i + 256] = scaledValue;
			b_2_pix_alloc[i + 256] = scaledValue;
		}

		// Spread out the values we have to the rest of the array so that we do
		// not need to check for overflow. We have to do it here in two steps.
		for (int i = 0; i < 256 + 16; i++) {
			r_2_pix_alloc[i] = r_2_pix_alloc[256 + 16];
			g_2_pix_alloc[i] = g_2_pix_alloc[256 + 16];
			b_2_pix_alloc[i] = b_2_pix_alloc[256 + 16];
		}

		for (int i = 256 + 236; i < 768; i++) {
			r_2_pix_alloc[i] = r_2_pix_alloc[256 + 236 - 1];
			g_2_pix_alloc[i] = g_2_pix_alloc[256 + 236 - 1];
			b_2_pix_alloc[i] = b_2_pix_alloc[256 + 236 - 1];
		}
	}
}

YUVToRGBManager::YUVToRGBManager() {
	int16_t *Cr_r_tab = &_colorTab[0 * 256];
	int16_t *Cr_g_tab = &_colorTab[1 * 256];
	int16_t *Cb_g_tab = &_colorTab[2 * 256];
	int16_t *Cb_b_tab = &_colorTab[3 * 256];

	// Generate the tables for the display surface

	for (int i = 0; i < 256; i++) {
		// Gamma correction (luminescence table) and chroma correction
		// would be done here. See the Berkeley mpeg_play sources.

		int16_t CR = (i - 128), CB = CR;
		Cr_r_tab[i] = (int16_t) ( (0.419 / 0.299) * CR) + 0 * 768 + 256;
		Cr_g_tab[i] = (int16_t) (-(0.299 / 0.419) * CR) + 1 * 768 + 256;
		Cb_g_tab[i] = (int16_t) (-(0.114 / 0.331) * CB);
		Cb_b_tab[i] = (int16_t) ( (0.587 / 0.331) * CB) + 2 * 768 + 256;
	}
}

YUVToRGBManager::~YUVToRGBManager() {
}

const YUVToRGBLookup *YUVToRGBManager::getLookup(LuminanceScale scale) {
	if (_lookup && _lookup->getScale() == scale)
		return _lookup.get();

	_lookup = std::make_unique<YUVToRGBLookup>(scale);
	return _lookup.get();
}

#define PUT_PIXEL(s, a, d) \
	L = &rgbToPix[(s)]; \
	*((d)) = L[cb_b]; \
	*((d) + 1) = L[crb_g]; \
	*((d) + 2) = L[cr_r]; \
	*((d) + 3) = (a)

void YUVToRGBManager::convert420(LuminanceScale scale, byte *dst, int dstPitch, const byte *ySrc, const byte *uSrc, const byte *vSrc, const byte *aSrc, int yWidth, int yHeight, int yPitch, int uvPitch) {
	const YUVToRGBLookup *lookup = YUVToRGBMan.getLookup(scale);
	const byte *rgbToPix = lookup->getRGBToPix();

	int halfHeight = yHeight >> 1;
	int halfWidth = yWidth >> 1;

	dst += dstPitch * (yHeight - 2);

	for (int h = 0; h < halfHeight; h++) {
		for (int w = 0; w < halfWidth; w++) {
			const byte *L;

			int16_t cr_r  = _colorTab[*vSrc + 0 * 256];
			int16_t crb_g = _colorTab[*vSrc + 1 * 256] + _colorTab[*uSrc + 2 * 256];
			int16_t cb_b  = _colorTab[*uSrc + 3 * 256];
			uSrc++;
			vSrc++;

			PUT_PIXEL(*ySrc, *aSrc, dst + dstPitch);
			PUT_PIXEL(*(ySrc + yPitch), *(aSrc + yPitch), dst);
			ySrc++;
			aSrc++;
			dst += 4;
			PUT_PIXEL(*ySrc, *aSrc, dst + dstPitch);
			PUT_PIXEL(*(ySrc + yPitch), *(aSrc + yPitch), dst);
			ySrc++;
			aSrc++;
			dst += 4;
		}

		dst -= yWidth * 4 + dstPitch * 2;
		ySrc += (yPitch << 1) - yWidth;
		aSrc += (yPitch << 1) - yWidth;
		uSrc += uvPitch - halfWidth;
		vSrc += uvPitch - halfWidth;
	}
}

void YUVToRGBManager::convert420(LuminanceScale scale, byte *dst, int dstPitch, const byte *ySrc, const byte *uSrc, const byte *vSrc, int yWidth, int yHeight, int yPitch, int uvPitch) {
	const YUVToRGBLookup *lookup = YUVToRGBMan.getLookup(scale);
	const byte *rgbToPix = lookup->getRGBToPix();

	int halfHeight = yHeight >> 1;
	int halfWidth = yWidth >> 1;

	dst += dstPitch * (yHeight - 2);

	for (int h = 0; h < halfHeight; h++) {
		for (int w = 0; w < halfWidth; w++) {
			const byte *L;

			int16_t cr_r  = _colorTab[*vSrc + 0 * 256];
			int16_t crb_g = _colorTab[*vSrc + 1 * 256] + _colorTab[*uSrc + 2 * 256];
			int16_t cb_b  = _colorTab[*uSrc + 3 * 256];
			uSrc++;
			vSrc++;

			PUT_PIXEL(*ySrc, 0xFF, dst + dstPitch);
			PUT_PIXEL(*(ySrc + yPitch), 0xFF, dst);
			ySrc++;
			dst += 4;
			PUT_PIXEL(*ySrc, 0xFF, dst + dstPitch);
			PUT_PIXEL(*(ySrc + yPitch), 0xFF, dst);
			ySrc++;
			dst += 4;
		}

		dst -= yWidth * 4 + dstPitch * 2;
		ySrc += (yPitch << 1) - yWidth;
		uSrc += uvPitch - halfWidth;
		vSrc += uvPitch - halfWidth;
	}
}

} // End of namespace Graphics
