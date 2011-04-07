/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/creature.cpp
 *  NWN creature.
 */

#include "common/util.h"
#include "common/maths.h"

#include "aurora/types.h"
#include "aurora/talkman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/creature.h"

#include "engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

Creature::Creature() : _model(0), _tooltip(0) {
}

Creature::~Creature() {
	hide();

	delete _model;
	delete _tooltip;
}

void Creature::show() {
	assert(_loaded);

	loadModel();

	if (_model)
		_model->show();
}

void Creature::hide() {
	leave();

	if (_model)
		_model->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z) {
	Object::setOrientation(x, y, z);
	Object::getOrientation(x, y, z);

	if (_model)
		_model->setRotation(x, z, -y);
}

void Creature::clear() {
	Object::clear();

	_firstName.clear();
	_lastName.clear();
	_description.clear();

	_gender = Aurora::kFieldIDInvalid;
	_race   = Aurora::kFieldIDInvalid;

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_classes.clear();

	_lastChangedGUIDisplay = 0;

	_appearanceID = Aurora::kFieldIDInvalid;
	_phenotype    = Aurora::kFieldIDInvalid;

	_head   = Aurora::kFieldIDInvalid;
	_neck   = Aurora::kFieldIDInvalid;
	_torso  = Aurora::kFieldIDInvalid;
	_pelvis = Aurora::kFieldIDInvalid;
	_belt   = Aurora::kFieldIDInvalid;
	_rFoot  = Aurora::kFieldIDInvalid;
	_lFoot  = Aurora::kFieldIDInvalid;
	_rShin  = Aurora::kFieldIDInvalid;
	_lShin  = Aurora::kFieldIDInvalid;
	_lThigh = Aurora::kFieldIDInvalid;
	_rThigh = Aurora::kFieldIDInvalid;
	_rFArm  = Aurora::kFieldIDInvalid;
	_lFArm  = Aurora::kFieldIDInvalid;
	_rBicep = Aurora::kFieldIDInvalid;
	_lBicep = Aurora::kFieldIDInvalid;
	_rShoul = Aurora::kFieldIDInvalid;
	_lShoul = Aurora::kFieldIDInvalid;
	_rHand  = Aurora::kFieldIDInvalid;
	_lHand  = Aurora::kFieldIDInvalid;

	_colorSkin    = Aurora::kFieldIDInvalid;
	_colorHair    = Aurora::kFieldIDInvalid;
	_colorTattoo1 = Aurora::kFieldIDInvalid;
	_colorTattoo2 = Aurora::kFieldIDInvalid;

	delete _model;
	_model = 0;

	delete _tooltip;
	_tooltip = 0;
}

uint32 Creature::lastChangedGUIDisplay() const {
	return _lastChangedGUIDisplay;
}

const Common::UString &Creature::getFirstName() const {
	return _firstName;
}

const Common::UString &Creature::getLastName() const {
	return _lastName;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isDM() const {
	return _isDM;
}

uint32 Creature::getAge() const {
	return _age;
}

uint32 Creature::getXP() const {
	return _xp;
}

int32 Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32 Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
}

void Creature::loadModel() {
	assert(_loaded);

	if (_model)
		return;

	if (_appearanceID == Aurora::kFieldIDInvalid) {
		warning("Creature \"%s\" has no appearance", _tag.c_str());
		return;
	}

	const Aurora::TwoDARow &appearance = TwoDAReg.get("appearance").getRow(_appearanceID);

	if (_portrait.empty())
		_portrait = appearance.getString("PORTRAIT");

	if (appearance.getString("MODELTYPE") == "P") {

		warning("ModelType P: \"%s\"", _tag.c_str());

	} else
		_model = loadModelObject(appearance.getString("RACE"));

	// Positioning

	float x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z);
	setOrientation(x, y, z);

	// Clickable

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());
	}
}

void Creature::unloadModel() {
	hide();

	delete _model;
	_model = 0;
}

void Creature::loadCharacter(Common::SeekableReadStream &stream) {
	clear();

	Aurora::GFFFile gff;
	loadGFF(gff, stream, MKID_BE('BIC '));

	load(gff.getTopLevel(), 0);

	// All BICs should be PCs.
	_isPC = true;

	_lastChangedGUIDisplay = EventMan.getTimestamp();
	_loaded = true;
}

void Creature::load(const Aurora::GFFStruct &creature) {
	clear();

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

	_lastChangedGUIDisplay = EventMan.getTimestamp();
	_loaded = true;
}

void Creature::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

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
}

void Creature::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Name

	if (gff.hasField("FirstName")) {
		Aurora::LocString firstName;
		gff.getLocString("FirstName", firstName);

		_firstName = firstName.getString();
	}

	if (gff.hasField("LastName")) {
		Aurora::LocString lastName;
		gff.getLocString("LastName", lastName);

		_lastName = lastName.getString();
	}

	_name = _firstName + " " + _lastName;
	_name.trim();

	// Description

	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Portrait

	loadPortrait(gff);

	// Gender
	_gender = gff.getUint("Gender", _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// PC and DM
	_isPC = gff.getBool("IsPC", _isPC);
	_isDM = gff.getBool("IsDM", _isDM);

	// Age
	_age = gff.getUint("Age", _age);

	// Experience
	_xp = gff.getUint("Experience", _xp);

	// Classes
	if (gff.hasField("ClassList")) {
		_classes.clear();

		const Aurora::GFFList &cClasses = gff.getList("ClassList");

		for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
			_classes.push_back(Class());

			const Aurora::GFFStruct &cClass = **c;

			_classes.back().classID = cClass.getUint("Class");
			_classes.back().level   = cClass.getUint("ClassLevel");
		}
	}

	// Health
	if (gff.hasField("HitPoints")) {
		_baseHP    = gff.getSint("HitPoints");
		_bonusHP   = gff.getSint("MaxHitPoints", _baseHP) - _baseHP;
		_currentHP = gff.getSint("CurrentHitPoints", _baseHP);
	}

	// Appearance

	_appearanceID = gff.getUint("Appearance_Type", _appearanceID);
	_phenotype    = gff.getUint("Phenotype"      , _phenotype);

	// Body parts
	_head   = gff.getUint("Appearance_Head", _head);
	_neck   = gff.getUint("BodyPart_Neck"  , _neck);
	_torso  = gff.getUint("BodyPart_Torso" , _torso);
	_pelvis = gff.getUint("BodyPart_Pelvis", _pelvis);
	_belt   = gff.getUint("BodyPart_Belt"  , _belt);
	_rFoot  = gff.getUint("ArmorPart_RFoot", _rFoot);
	_lFoot  = gff.getUint("BodyPart_LFoot" , _lFoot);
	_rShin  = gff.getUint("BodyPart_RShin" , _rShin);
	_lShin  = gff.getUint("BodyPart_LShin" , _lShin);
	_lThigh = gff.getUint("BodyPart_LThigh", _lThigh);
	_rThigh = gff.getUint("BodyPart_RThigh", _rThigh);
	_rFArm  = gff.getUint("BodyPart_RFArm" , _rFArm);
	_lFArm  = gff.getUint("BodyPart_LFArm" , _lFArm);
	_rBicep = gff.getUint("BodyPart_RBicep", _rBicep);
	_lBicep = gff.getUint("BodyPart_LBicep", _lBicep);
	_rShoul = gff.getUint("BodyPart_RShoul", _rShoul);
	_lShoul = gff.getUint("BodyPart_LShoul", _lShoul);
	_rHand  = gff.getUint("BodyPart_RHand" , _rHand);
	_lHand  = gff.getUint("BodyPart_LHand" , _lHand);

	// Colors
	_colorSkin    = gff.getUint("Color_Skin", _colorSkin);
	_colorHair    = gff.getUint("Color_Hair", _colorHair);
	_colorTattoo1 = gff.getUint("Color_Tattoo1", _colorTattoo1);
	_colorTattoo2 = gff.getUint("Color_Tattoo2", _colorTattoo2);
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

Common::UString Creature::getClassString() const {
	Common::UString classString;

	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c) {
		if (!classString.empty())
			classString += '/';

		uint32 strRef = TwoDAReg.get("classes").getRow(c->classID).getInt("Name");

		classString += TalkMan.getString(strRef);
	}

	return classString;
}

void Creature::enter() {
	highlight(true);
}

void Creature::leave() {
	highlight(false);
}

void Creature::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);

	if (enabled)
		showTooltip();
	else
		hideTooltip();
}

void Creature::createTooltip() {
	if (_tooltip)
		return;

	_tooltip = new Tooltip(Tooltip::kTypeFeedback, *_model);

	_tooltip->setAlign(0.5);
	_tooltip->addLine(_name, 0.5, 0.5, 1.0, 1.0);
	_tooltip->setPortrait(_portrait);
}

void Creature::showTooltip() {
	createTooltip();
	_tooltip->show();
}

void Creature::hideTooltip() {
	if (!_tooltip)
		return;

	_tooltip->hide();
}

} // End of namespace NWN

} // End of namespace Engines
