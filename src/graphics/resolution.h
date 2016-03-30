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
 *  A list common video resolutions.
 */

#ifndef GRAPHICS_RESOLUTION_H
#define GRAPHICS_RESOLUTION_H

namespace Graphics {

struct Resolution {
	int width;
	int height;
};

static const Resolution kResolutions[] = {
	{ 7680, 4800 },
	{ 7680, 4320 },
	{ 6400, 4800 },
	{ 6400, 4096 },
	{ 5120, 4096 },
	{ 5120, 3200 },
	{ 4096, 3072 },
	{ 4096, 1716 },
	{ 3840, 2400 },
	{ 3200, 2400 },
	{ 3200, 2048 },
	{ 2560, 2048 },
	{ 2560, 1600 },
	{ 2560, 1440 },
	{ 2048, 1536 },
	{ 2048, 1152 },
	{ 2048, 1080 },
	{ 1920, 1200 },
	{ 1920, 1080 },
	{ 1680, 1050 },
	{ 1600, 1200 },
	{ 1600,  900 },
	{ 1440,  900 },
	{ 1400, 1050 },
	{ 1280, 1024 },
	{ 1280,  960 },
	{ 1280,  800 },
	{ 1280,  720 },
	{ 1152,  864 },
	{ 1024,  768 },
	{  800,  600 },
	{  640,  480 },
	{  320,  240 },
	{  320,  200 }
};

} // End of namespace Graphics

#endif // GRAPHICS_RESOLUTION_H
