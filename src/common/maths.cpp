/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/maths.cpp
 *  Mathematical helpers.
 */

#include <cassert>

#include "common/maths.h"

#include "common/sinetables.h"
#include "common/cosinetables.h"
#include "common/sinewindows.h"

namespace Common {

const float *getSineTable(int bits) {
	assert((bits >= 4) && (bits <= 16));

	return sinTables[bits];
}

const float *getCosineTable(int bits) {
	assert((bits >= 4) && (bits <= 16));

	return cosTables[bits];
}

const float *getSineWindow(int bits) {
	assert((bits >= 5) && (bits <= 12));

	return sineWindows[bits];
}

} // End of namespace Common
