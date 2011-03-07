/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/door.cpp
 *  NWN door.
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

#include "engines/nwn/door.h"

namespace Engines {

namespace NWN {

Door::Door() : _genericType(Aurora::kFieldIDInvalid) {
}

Door::~Door() {
}

void Door::load(const Aurora::GFFStruct &door) {
	Common::UString temp = door.getString("TemplateResRef");
	if (temp.empty())
		throw Common::Exception("Door without a template resref");

	Aurora::GFFFile utd;
	loadGFF(utd, temp, Aurora::kFileTypeUTD, MKID_BE('UTD '));

	Situated::load(utd.getTopLevel(), door);
}

void Door::loadObject(const Aurora::GFFStruct &gff) {
	if (_genericType == Aurora::kFieldIDInvalid)
		_genericType = gff.getUint("GenericType", Aurora::kFieldIDInvalid);
}

void Door::loadAppearance() {
	if (_appearanceID == 0) {
		if (_genericType == Aurora::kFieldIDInvalid)
			throw Common::Exception("Door has no appearance ID and no generic type");

		loadAppearance(TwoDAReg.get("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get("doortypes"), _appearanceID);
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id) {
	_modelName = twoda.getCellString(id, "ModelName");
	if (_modelName.empty())
		_modelName = twoda.getCellString(id, "Model");
}

} // End of namespace NWN

} // End of namespace Engines
