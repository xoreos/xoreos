/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/placeable.cpp
 *  NWN placeable.
 */

#include "common/endianness.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/util.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/placeable.h"

namespace Engines {

namespace NWN {

Placeable::Placeable() {
}

Placeable::~Placeable() {
}

void Placeable::load(const Aurora::GFFStruct &placeable) {
	Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFFFile *utp = 0;
	if (!temp.empty()) {
		try {
			utp = loadGFF(temp, Aurora::kFileTypeUTP, MKID_BE('UTP '));
		} catch (...) {
		}
	}

	Situated::load(placeable, utp ? &utp->getTopLevel() : 0);

	if (!utp)
		warning("Placeable \"%s\" has no blueprint", _tag.c_str());

	delete utp;
}

void Placeable::loadObject(const Aurora::GFFStruct &gff) {
}

void Placeable::loadAppearance() {
	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeables");

	_modelName = twoda.getCellString(_appearanceID, "ModelName");
}

} // End of namespace NWN

} // End of namespace Engines
