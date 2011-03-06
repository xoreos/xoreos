/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/tileset.cpp
 *  NWN tileset.
 */

#include "common/error.h"
#include "common/configfile.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "engines/nwn/tileset.h"

namespace Engines {

namespace NWN {

Tileset::Tileset(const Common::UString &resRef) {
	Common::SeekableReadStream *setFile = ResMan.getResource(resRef, Aurora::kFileTypeSET);
	if (!setFile)
		throw Common::Exception("No such tileset \"%s\"", resRef.c_str());

	Common::ConfigFile set;

	try {
		set.load(*setFile);
	} catch (...) {
		delete setFile;
		throw;
	}

	delete setFile;

	load(set);
}

Tileset::~Tileset() {
}

const Common::UString &Tileset::getName() const {
	return _name;
}

void Tileset::load(const Common::ConfigFile &set) {
	const Common::ConfigDomain *general = set.getDomain("GENERAL");
	if (!general)
		throw Common::Exception("Tileset has no \"GENERAL\" domain");

	loadGeneral(*general);
}

void Tileset::loadGeneral(const Common::ConfigDomain &general) {
	_name = TalkMan.getString(general.getUint("DisplayName", Aurora::kStrRefInvalid));
}

} // End of namespace NWN

} // End of namespace Engines
