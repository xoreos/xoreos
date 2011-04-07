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


Creature::PartModels::PartModels() {
	headPosition[0] = 0.0;
	headPosition[1] = 0.0;
	headPosition[2] = 0.0;
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

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Creature::loadAppearance() {
	PartModels parts;

	getPartModels(parts);

	if ((parts.type == "P") || parts.body.empty()) {
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")",
		        _tag.c_str(), parts.type.c_str(), parts.body.c_str());
		return;
	}

	loadBody(parts);
	loadHead(parts);
}

void Creature::getPartModels(PartModels &parts, uint32 state) {
	const Aurora::TwoDARow &appearance = TwoDAReg.get("appearance").getRow(_appearance);

	parts.type = appearance.getString("modeltype");

	parts.body = appearance.getString(Common::UString("model") + state);
	if (parts.body.empty())
		parts.body = appearance.getString("race");

	parts.bodyTexture = appearance.getString(Common::UString("tex") + state);
	if (!parts.bodyTexture.empty())
		parts.bodyTexture += "01";

	if (parts.bodyTexture.empty())
		parts.bodyTexture = appearance.getString("racetex");

	if ((parts.type == "B") || (parts.type == "P")) {
		const int headNormalID = appearance.getInt("normalhead");
		const int headBackupID = appearance.getInt("backuphead");

		const Aurora::TwoDAFile &heads = TwoDAReg.get("heads");

		if      (headNormalID >= 0)
			parts.head = heads.getRow(headNormalID).getString("head");
		else if (headBackupID >= 0)
			parts.head = heads.getRow(headBackupID).getString("head");
	}
}

void Creature::loadBody(PartModels &parts) {
	Graphics::Aurora::Model *m = loadModelObject(parts.body, parts.bodyTexture);
	if (!m)
		return;

	_ids.push_back(m->getID());

	m->setTag(_tag + "#Body");
	m->setClickable(isClickable());

	Graphics::Aurora::ModelNode *head = m->getNode("headhook");
	if (head)
		head->getAbsolutePosition(parts.headPosition[0],
		                          parts.headPosition[1],
		                          parts.headPosition[2]);

	_parts.push_back(Part());

	_parts.back().model = m;
}

void Creature::loadHead(PartModels &parts) {
	if (parts.head.empty())
		return;

	Graphics::Aurora::Model *m = loadModelObject(parts.head);
	if (!m)
		return;

	_ids.push_back(m->getID());

	m->setTag(_tag + "#Head");
	m->setClickable(isClickable());

	_parts.push_back(Part());

	_parts.back().model       = m;
	_parts.back().position[0] = parts.headPosition[0];
	_parts.back().position[1] = parts.headPosition[1];
	_parts.back().position[2] = parts.headPosition[2];
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
