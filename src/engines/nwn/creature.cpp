/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/creature.cpp
 *  NWN creature.
 */

#include "common/util.h"
#include "common/maths.h"

#include "aurora/types.h"
#include "aurora/talkman.h"
#include "aurora/resman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/pltfile.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/creature.h"

#include "engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

Creature::BodyPart::BodyPart() : id(Aurora::kFieldIDInvalid) {
}


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

	_gender = kGenderNone;
	_race   = kRaceInvalid;

	_subRace.clear();

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_baseHP    = 0;
	_bonusHP   = 0;
	_currentHP = 0;

	for (int i = 0; i < kAbilityMAX; i++)
		_abilities[i] = 0;

	_classes.clear();

	_hitDice = 0;

	_deity.clear();

	_goodEvil = 0;
	_lawChaos = 0;

	_lastChangedGUIDisplay = 0;

	_appearanceID = Aurora::kFieldIDInvalid;
	_phenotype    = Aurora::kFieldIDInvalid;

	_bodyParts.clear();
	_bodyParts.resize(kBodyPartMAX);

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

uint32 Creature::getGender() const {
	return _gender;
}

bool Creature::isFemale() const {
	// Male and female are hardcoded.  Other genders (none, both, other)
	// count as male when it comes to tokens in text strings.

	return _gender == Aurora::kGenderFemale;
}

uint32 Creature::getRace() const {
	return _race;
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

uint8 Creature::getAbility(Ability ability) const {
	assert((ability > 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

int32 Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32 Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
}

void Creature::constructModelName(const Common::UString &type, uint32 id,
                                  const Common::UString &gender,
                                  const Common::UString &race,
                                  const Common::UString &phenoType,
                                  const Common::UString &phenoTypeAlt,
                                  Common::UString &model) {

	model = Common::UString::sprintf("p%s%s%s_%s%03d",
	        gender.c_str(), race.c_str(), phenoType.c_str(), type.c_str(), id);

	if (ResMan.hasResource(model, Aurora::kFileTypeMDL))
		return;

	model = Common::UString::sprintf("p%s%s%s_%s%03d",
	        gender.c_str(), race.c_str(), phenoTypeAlt.c_str(), type.c_str(), id);

	if (!ResMan.hasResource(model, Aurora::kFileTypeMDL))
		model.clear();
}

static const char *kBodyPartModels[] = {
	"head"  ,
	"neck"  ,
	"chest" ,
	"pelvis",
	"belt"  ,
	"rfoot" , "lfoot" ,
	"rshin" , "lshin" ,
	"lthigh", "rthigh",
	"rfarm" , "lfarm" ,
	"rbicep", "lbicep",
	"rshoul", "lshoul",
	"rhand" , "lhand"
};

void Creature::getPartModels() {
	const Aurora::TwoDAFile &appearance = TwoDAReg.get("appearance");

	const Aurora::TwoDARow &gender = TwoDAReg.get("gender").getRow(_gender);
	const Aurora::TwoDARow &race   = TwoDAReg.get("racialtypes").getRow(_race);
	const Aurora::TwoDARow &raceAp = appearance.getRow(race.getInt("Appearance"));
	const Aurora::TwoDARow &pheno  = TwoDAReg.get("phenotype").getRow(_phenotype);

	Common::UString genderChar   = gender.getString("GENDER");
	Common::UString raceChar     = raceAp.getString("RACE");
	Common::UString phenoChar    = Common::UString("%d", _phenotype);
	Common::UString phenoAltChar = pheno.getString("DefaultPhenoType");

	for (uint i = 0; i < kBodyPartMAX; i++)
		constructModelName(kBodyPartModels[i], _bodyParts[i].id,
		                   genderChar, raceChar, phenoChar, phenoAltChar,
		                   _bodyParts[i].modelName);
}

void Creature::finishPLTs(std::list<Graphics::Aurora::PLTHandle> &plts) {
	for (std::list<Graphics::Aurora::PLTHandle>::iterator p = plts.begin();
	     p != plts.end(); ++p) {

		Graphics::Aurora::PLTFile &plt = p->getPLT();

		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerSkin   , _colorSkin);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerHair   , _colorHair);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo1, _colorTattoo1);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo2, _colorTattoo2);

		plt.rebuild();
	}
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
		getPartModels();

		for (uint i = 0; i < kBodyPartMAX; i++) {
			if (i != kBodyPartHead)
				continue;

			if (_bodyParts[i].modelName.empty())
				continue;

			TextureMan.clearNewPLTs();

			_model = loadModelObject(_bodyParts[i].modelName, _bodyParts[i].modelName);
			if (!_model)
				continue;

			TextureMan.getNewPLTs(_bodyParts[i].plts);

			finishPLTs(_bodyParts[i].plts);
		}

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

	// Set the PC tag to something recognizable for now.
	// Let's hope no script depends on it being "".

	_tag = Common::UString::sprintf("[PC: %s]", _name.c_str());

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

static const char *kBodyPartFields[] = {
	"Appearance_Head",
	"BodyPart_Neck"  ,
	"BodyPart_Torso" ,
	"BodyPart_Pelvis",
	"BodyPart_Belt"  ,
	"ArmorPart_RFoot", "BodyPart_LFoot" ,
	"BodyPart_RShin" , "BodyPart_LShin" ,
	"BodyPart_LThigh", "BodyPart_RThigh",
	"BodyPart_RFArm" , "BodyPart_LFArm" ,
	"BodyPart_RBicep", "BodyPart_LBicep",
	"BodyPart_RShoul", "BodyPart_LShoul",
	"BodyPart_RHand" , "BodyPart_LHand"
};

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

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Portrait

	loadPortrait(gff);

	// Gender
	_gender = gff.getUint("Gender", _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// Subrace
	_subRace = gff.getString("Subrace", _subRace);

	// PC and DM
	_isPC = gff.getBool("IsPC", _isPC);
	_isDM = gff.getBool("IsDM", _isDM);

	// Age
	_age = gff.getUint("Age", _age);

	// Experience
	_xp = gff.getUint("Experience", _xp);

	// Abilities
	_abilities[kAbilityStrength]     = gff.getUint("Str", _abilities[kAbilityStrength]);
	_abilities[kAbilityDexterity]    = gff.getUint("Dex", _abilities[kAbilityDexterity]);
	_abilities[kAbilityConstitution] = gff.getUint("Con", _abilities[kAbilityConstitution]);
	_abilities[kAbilityIntelligence] = gff.getUint("Int", _abilities[kAbilityIntelligence]);
	_abilities[kAbilityWisdom]       = gff.getUint("Wis", _abilities[kAbilityWisdom]);
	_abilities[kAbilityCharisma]     = gff.getUint("Cha", _abilities[kAbilityCharisma]);

	// Classes
	if (gff.hasField("ClassList")) {
		_classes.clear();
		_hitDice = 0;

		const Aurora::GFFList &cClasses = gff.getList("ClassList");

		for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
			_classes.push_back(Class());

			const Aurora::GFFStruct &cClass = **c;

			_classes.back().classID = cClass.getUint("Class");
			_classes.back().level   = cClass.getUint("ClassLevel");

			_hitDice += _classes.back().level;
		}
	}

	// Deity
	_deity = gff.getString("Deity", _deity);

	// Health
	if (gff.hasField("HitPoints")) {
		_baseHP    = gff.getSint("HitPoints");
		_bonusHP   = gff.getSint("MaxHitPoints", _baseHP) - _baseHP;
		_currentHP = gff.getSint("CurrentHitPoints", _baseHP);
	}

	// Alignment

	_goodEvil = gff.getUint("GoodEvil", _goodEvil);
	_lawChaos = gff.getUint("LawfulChaotic", _lawChaos);

	// Appearance

	_appearanceID = gff.getUint("Appearance_Type", _appearanceID);
	_phenotype    = gff.getUint("Phenotype"      , _phenotype);

	// Body parts
	for (uint i = 0; i < kBodyPartMAX; i++)
		_bodyParts[i].id = gff.getUint(kBodyPartFields[i], _bodyParts[i].id);

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

const Common::UString &Creature::getConvRace() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("ConverName");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvrace() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("ConverNameLower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvRaces() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("NamePlural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getSubRace() const {
	return _subRace;
}

void Creature::getClass(uint32 position, uint32 &classID, uint16 &level) const {
	if (position >= _classes.size()) {
		classID = kClassInvalid;
		level   = 0;
		return;
	}

	classID = _classes[position].classID;
	level   = _classes[position].level;
}

uint16 Creature::getClassLevel(uint32 classID) const {
	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c)
		if (c->classID == classID)
			return c->level;

	return 0;
}

const Common::UString &Creature::getConvClass() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Name");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvclass() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Lower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvClasses() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Plural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getDeity() const {
	return _deity;
}

uint8 Creature::getGoodEvil() const {
	return _goodEvil;
}

uint8 Creature::getLawChaos() const {
	return _lawChaos;
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

uint8 Creature::getHitDice() const {
	return _hitDice;
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
