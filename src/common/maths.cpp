/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/maths.cpp
 *  Mathematical helpers.
 */

#include <cassert>

#include "common/maths.h"

#include "common/sinetables.h"
#include "common/cosinetables.h"

namespace Common {

const float *getSineTable(int bits) {
	assert((bits >= 4) && (bits <= 16));

	return sinTables[bits];
}

const float *getCosineTable(int bits) {
	assert((bits >= 4) && (bits <= 16));

	return cosTables[bits];
}

} // End of namespace Common
