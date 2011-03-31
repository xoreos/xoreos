/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/situated.cpp
 *  NWN situated.
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

#include "engines/nwn/situated.h"

namespace Engines {

namespace NWN {

Situated::Situated() : _appearanceID(Aurora::kFieldIDInvalid), _model(0), _active(false),
	_loaded(false) {
}

Situated::~Situated() {
	delete _model;
}

void Situated::show() {
	assert(_loaded);

	if (_model)
		_model->show();
}

void Situated::hide() {
	assert(_loaded);

	if (_model)
		_model->hide();
}

const Common::UString &Situated::getTag() const {
	return _tag;
}

const Common::UString &Situated::getName() const {
	return _name;
}

const Common::UString &Situated::getDescription() const {
	return _description;
}

uint32 Situated::getID() const {
	if (!_model)
		return 0;

	return _model->getID();
}

void Situated::setActive(bool active) {
	if (_active == active)
		return;

	if (_model)
		_model->drawBound(active);

	_active = active;
}

void Situated::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	assert(!_loaded);


	// General properties

	loadProperties(instance);    // Instance
	if (blueprint)
		loadProperties(*blueprint); // Blueprint


	// Specialized object properties

	loadObject(instance);    // Instance
	if (blueprint)
		loadObject(*blueprint); // Blueprint


	// Appearance

	if (_appearanceID == Aurora::kFieldIDInvalid)
		throw Common::Exception("Situated object without an appearance");

	loadAppearance();


	// Model

	if (!_modelName.empty()) {
		_model = loadModelObject(_modelName);

		if (!_model)
			throw Common::Exception("Failed to load situated object model \"%s\"",
			                        _modelName.c_str());
	} else
		warning("Situated object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());


	_model->setTag(_tag);
	_model->setClickable(true);

	// Model position

	if (_model) {
		float x = instance.getDouble("X");
		float y = instance.getDouble("Y");
		float z = instance.getDouble("Z");

		_model->setPosition(x, y, z);


		// Model orientation

		float bearing = instance.getDouble("Bearing");

		_model->setRotation(0.0, 0.0, -Common::rad2deg(bearing));
	}


	_loaded = true;
}

void Situated::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag
	if (_tag.empty())
		_tag = gff.getString("Tag");

	// Name
	if (_name.empty()) {
		Aurora::LocString name;
		gff.getLocString("LocName", name);

		_name = name.getString();
	}

	// Description
	if (_description.empty()) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Portrait
	if (_portrait.empty())
		loadPortrait(gff);

	// Appearance
	if (_appearanceID == Aurora::kFieldIDInvalid)
		_appearanceID = gff.getUint("Appearance", Aurora::kFieldIDInvalid);
}

void Situated::loadPortrait(const Aurora::GFFStruct &gff) {
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

} // End of namespace NWN

} // End of namespace Engines
