/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/erffile.cpp
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/erffile.h"
#include "aurora/aurorafile.h"

static const uint32 kERFID     = MKID_BE('ERF ');
static const uint32 kMODID     = MKID_BE('MOD ');
static const uint32 kHAKID     = MKID_BE('HAK ');
static const uint32 kSAVID     = MKID_BE('SAV ');
static const uint32 kVersion1  = MKID_BE('V1.0');

namespace Aurora {

ERFFile::ERFFile() {
	_id      = 0;
	_version = 0;
}

ERFFile::~ERFFile() {
}

void ERFFile::clear() {
}

bool ERFFile::load(Common::SeekableReadStream &bif) {
	return true;
}

} // End of namespace Aurora
