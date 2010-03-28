/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/keyfile.h"

static const uint32 kKeyID    = MKID_BE('KEY ');
static const uint32 kVersion1 = MKID_BE('V1  ');

namespace Aurora {

KeyFile::KeyFile() {
}

KeyFile::~KeyFile() {
}

void KeyFile::clear() {
	_bifs.clear();
	_resources.clear();
}

bool KeyFile::load(Common::SeekableReadStream &key) {
	if (key.readUint32BE() != kKeyID) {
		warning("KeyFile::load(): Not a KEY file");
		return false;
	}
	if (key.readUint32BE() != kVersion1) {
		warning("KeyFile::load(): Unsupported file version");
		return false;
	}

	return true;
}

const KeyFile::BifList &KeyFile::getBifs() const {
	return _bifs;
}

const KeyFile::ResourceList &KeyFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora
