/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/creature.cpp
 *  A creature.
 */

#include "engines/kotor/creature.h"

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "engines/util.h"

#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/gfffile.h"

#include "graphics/aurora/model.h"

static const uint32 kUTCID = MKID_BE('UTC ');

namespace Engines {

namespace KotOR {

Creature::Creature(const ModelLoader &modelLoader) : ModelObject(modelLoader) {
}

Creature::~Creature() {
}

void Creature::load(const Common::UString &name) {
	status("Loading creature \"%s\"", name.c_str());

	Aurora::GFFFile utc;
	loadGFF(utc, name, Aurora::kFileTypeUTC, kUTCID);

	Aurora::GFFFile::StructRange utcTop = utc.structRange();
	for (Aurora::GFFFile::StructIterator it = utcTop.first; it != utcTop.second; ++it) {
	}
}

void Creature::show() {
}

void Creature::hide() {
}

void Creature::changedPosition() {
}

void Creature::changedBearing() {
}

void Creature::changedOrientation() {
}

} // End of namespace KotOR

} // End of namespace Engines
