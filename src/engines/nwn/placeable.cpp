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

Placeable::Placeable(const Aurora::GFFStruct &placeable) :
	_appearanceID(Aurora::kFieldIDInvalid), _model(0) {

	load(placeable);
}

Placeable::~Placeable() {
	delete _model;
}

void Placeable::show() {
	_model->show();
}

void Placeable::hide() {
	_model->hide();
}

const Common::UString &Placeable::getTag() const {
	return _tag;
}

const Common::UString &Placeable::getName() const {
	return _name;
}

const Common::UString &Placeable::getDescription() const {
	return _description;
}

void Placeable::load(const Aurora::GFFStruct &placeable) {
	loadProperties(placeable);

	// Template

	Common::UString temp = placeable.getString("TemplateResRef");
	if (temp.empty())
		throw Common::Exception("Placeable without a template resref");

	Aurora::GFFFile utp;
	loadGFF(utp, temp, Aurora::kFileTypeUTP, MKID_BE('UTP '));

	loadAppearance();

	// Model position

	float x = placeable.getDouble("X");
	float y = placeable.getDouble("Y");
	float z = placeable.getDouble("Z");

	_model->setPosition(x, y, z);

	// Model orientation

	float bearing = placeable.getDouble("Bearing");

	_model->setOrientation(0.0, 0.0, -Common::rad2deg(bearing));
}

void Placeable::loadTemplate(const Aurora::GFFStruct &utp) {
	loadProperties(utp);
}

void Placeable::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag
	if (_tag.empty())
		_tag = gff.getString("Tag");

	// Name
	if (_name.empty()) {
		Aurora::LocString name;
		gff.getLocString("LocName", name);

		_name = name.getFirstString();
	}

	// Description
	if (_description.empty()) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getFirstString();
	}

	// Portrait
	if (_portrait.empty())
		loadPortrait(gff);

	// Appearance
	if (_appearanceID == Aurora::kFieldIDInvalid)
		_appearanceID = gff.getUint("Appearance", Aurora::kFieldIDInvalid);
}

void Placeable::loadPortrait(const Aurora::GFFStruct &gff) {
	_portrait = gff.getString("Portrait");
	if (!_portrait.empty())
		return;

	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID == 0)
		return;

	const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

	Common::UString portrait = twoda.getCellString(portraitID, "BaseResRef");
	if (portrait.empty())
		return;

	_portrait = "po_" + portrait;
}

void Placeable::loadAppearance() {
	if (_appearanceID == Aurora::kFieldIDInvalid)
		throw Common::Exception("Placeable without an appearance");

	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeables");

	Common::UString modelName = twoda.getCellString(_appearanceID, "ModelName");
	if (modelName.empty())
		throw Common::Exception("Placeable without a model");

	_model = loadModelObject(modelName);
	if (!_model)
		throw Common::Exception("Failed to load placeable model \"%s\"", modelName.c_str());
}

} // End of namespace NWN

} // End of namespace Engines
