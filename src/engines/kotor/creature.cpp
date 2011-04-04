/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "common/maths.h"
#include "common/error.h"
#include "common/ustring.h"

#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/gfffile.h"
#include "aurora/locstring.h"

#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

namespace Engines {

namespace KotOR {

Creature::Part::Part() : model(0) {
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
}

Creature::Part::~Part() {
	delete model;
}


Creature::Creature() : _appearance(Aurora::kFieldIDInvalid) {
}

Creature::~Creature() {
}

void Creature::show() {
	for (std::list<Part>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		part->model->show();
}

void Creature::hide() {
	for (std::list<Part>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		part->model->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	for (std::list<Part>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		part->model->setPosition(x + part->position[0],
		                         y + part->position[1],
		                         z + part->position[2]);
}

void Creature::setOrientation(float x, float y, float z) {
	Object::setOrientation(x, y, z);
	Object::getOrientation(x, y, z);

	for (std::list<Part>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		part->model->setRotation(x, z, -y);
}

void Creature::load(const Aurora::GFFStruct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Aurora::GFFFile *utc = 0;
	if (!temp.empty()) {
		try {
			utc = loadGFF(temp, Aurora::kFileTypeUTC, MKID_BE('UTC '));
		} catch (...) {
		}
	}

	load(creature, utc ? &utc->getTopLevel() : 0);

	if (!utc)
		warning("Creature \"%s\" has no blueprint", _tag.c_str());

	delete utc;
}

void Creature::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	assert(!_loaded);

	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance


	// Appearance

	if (_appearance == Aurora::kFieldIDInvalid)
		throw Common::Exception("Creature without an appearance");

	loadAppearance();

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	float o[3];
	Common::vector2orientation(bearingX, bearingY, o[0], o[1], o[2]);

	setOrientation(o[0], o[1], o[2]);

	_loaded = true;
}

void Creature::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	if (gff.hasField("LocName")) {
		Aurora::LocString name;
		gff.getLocString("LocName", name);

		_name = name.getString();
	}

	// Description
	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Portrait
	loadPortrait(gff);

	// Appearance
	_appearance = gff.getUint("Appearance_Type", _appearance);

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);
}

void Creature::loadPortrait(const Aurora::GFFStruct &gff) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait = twoda.getCellString(portraitID, "BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Creature::loadAppearance() {
	const Aurora::TwoDAFile &appearance = TwoDAReg.get("appearance");
	const Aurora::TwoDAFile &heads      = TwoDAReg.get("heads");

	const Common::UString &modelType = appearance.getCellString(_appearance, "modeltype");

	Common::UString bodyModel = appearance.getCellString(_appearance, "modelb");
	if (bodyModel.empty())
		bodyModel = appearance.getCellString(_appearance, "race");

	Common::UString bodyTexture = appearance.getCellString(_appearance, "texb");
	if (!bodyTexture.empty())
		bodyTexture += "01";

	if (bodyTexture.empty())
		bodyTexture = appearance.getCellString(_appearance, "racetex");

	const int headNormalID = appearance.getCellInt(_appearance, "normalhead", -1);
	const int headBackupID = appearance.getCellInt(_appearance, "backuphead", -1);

	Common::UString headModel;
	if (modelType == "B") {
		if      (headNormalID >= 0)
			headModel = heads.getCellString(headNormalID, "head");
		else if (headBackupID >= 0)
			headModel = heads.getCellString(headBackupID, "head");
	}

	//  Totally segmented  ||    Body + Head     ||    ???
	if ((modelType == "P") || (modelType == "B") || bodyModel.empty())
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")",
		        _tag.c_str(), modelType.c_str(), bodyModel.c_str());

	if (modelType != "P") {
		float headX = 0.0, headY = 0.0, headZ = 0.0;

		loadBody(bodyModel, bodyTexture, headX, headY, headZ);

		if (modelType == "B")
			loadHead(headModel, headX, headY, headZ);
	}

}

void Creature::loadBody(const Common::UString &model, const Common::UString &texture,
	                      float &headX, float &headY, float &headZ) {

	if (model.empty())
		return;

	Graphics::Aurora::Model *m = loadModelObject(model, texture);
	if (!m)
		return;

	_ids.push_back(m->getID());

	m->setTag(_tag + "#Body");
	m->setClickable(isClickable());

	Graphics::Aurora::ModelNode *head = m->getNode("headhook");
	if (head)
		head->getAbsolutePosition(headX, headY, headZ);

	_parts.push_back(Part());

	_parts.back().model = m;
}

void Creature::loadHead(const Common::UString &model,
                        float headX, float headY, float headZ) {

	if (model.empty())
		return;

	Graphics::Aurora::Model *m = loadModelObject(model);
	if (!m)
		return;

	_ids.push_back(m->getID());

	m->setTag(_tag + "#Head");
	m->setClickable(isClickable());

	_parts.push_back(Part());

	_parts.back().model       = m;
	_parts.back().position[0] = headX;
	_parts.back().position[1] = headY;
	_parts.back().position[2] = headZ;
}

void Creature::enter() {
	highlight(true);
}

void Creature::leave() {
	highlight(false);
}

void Creature::highlight(bool enabled) {
	for (std::list<Part>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		part->model->drawBound(enabled);
}

} // End of namespace KotOR

} // End of namespace Engines
