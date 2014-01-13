/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/creature.cpp
 *  KotOR creature.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/file.h"
#include "common/configman.h"

#include "aurora/types.h"
#include "aurora/talkman.h"
#include "aurora/resman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model_kotor.h"

#include "events/events.h"

#include "engines/aurora/util.h"

#include "engines/kotor/creature.h"

namespace Engines {

namespace KotOR {

Creature::Creature(const Aurora::GFFStruct &creature) : Object(kObjectTypeCreature) {
	init();

	load(creature);
}

Creature::~Creature() {
	setVisible(false);

	destroyModel(_model);
}

void Creature::init() {
	_gender = kGenderNone;
	_race   = kRaceInvalid;

	_isPC = false;

	_age = 0;

	_xp = 0;

	_baseHP    = 0;
	_bonusHP   = 0;
	_currentHP = 0;

	_hitDice = 0;

	_appearanceID = Aurora::kFieldIDInvalid;

	_model = 0;

	for (int i = 0; i < kAbilityMAX; i++)
		_abilities[i] = 0;
}

void Creature::setVisible(bool visible) {
	if (_model)
		_model->setVisible(visible);

	Object::setVisible(visible);
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, z, -y);
}

void Creature::setOrientation(float radian, float x, float y, float z) {
	Object::setOrientation(radian, x, y, z);
	Object::getOrientation(radian, x, y, z);

	if (_model)
		_model->setOrientation(radian, x, y, z);
}

void Creature::move(float x, float y, float z) {
	Object::move(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::rotate(float radian, float x, float y, float z) {
	Object::rotate(radian, x, y, z);
	Object::getOrientation(radian, x, y, z);

	if (_model)
		_model->setOrientation(radian, x, z, -y);
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
	if (_model)
		return;

	Common::UString state = "b";

	const Aurora::TwoDARow &appearance = TwoDAReg.get("appearance").getRow(_appearanceID);

	Common::UString type = appearance.getString("modeltype");

	Common::UString bodyModel = appearance.getString(Common::UString("model") + state);
	Common::UString bodyTex   = appearance.getString(Common::UString("tex")   + state);

	Common::UString headModel;

	if (!bodyTex.empty())
		bodyTex += "01";

	if (bodyModel.empty())
		bodyModel = appearance.getString("race");
	if (bodyTex.empty())
		bodyTex = appearance.getString("racetex");

	if ((type == "B") || (type == "P")) {
		const int headNormalID = appearance.getInt("normalhead");
		const int headBackupID = appearance.getInt("backuphead");

		const Aurora::TwoDAFile &heads = TwoDAReg.get("heads");

		if      (headNormalID >= 0)
			headModel = heads.getRow(headNormalID).getString("head");
		else if (headBackupID >= 0)
			headModel = heads.getRow(headBackupID).getString("head");
	}

	if (bodyModel.empty())
		return;

	try {
		_model = createWorldModel(bodyModel, bodyTex);
	} catch (Common::Exception &e) {
		e.add("Failed to load creature \"%s\" (\"%s\")", _tag.c_str(), _name.c_str());
		throw;
	}

	if (!headModel.empty()) {
		Graphics::Aurora::Model_KotOR *head = 0;

		try {
			head = createWorldModel(headModel);
		} catch (Common::Exception &e) {
			e.add("Failed to load head model for creature \"%s\" (\"%s\")", _tag.c_str(), _name.c_str());
			Common::printException(e, "WARNING: ");
		}

		if (head) {
			// Reset the head's orientation, since it'll inherit it from the headhook node on the body
			head->setBaseOrientation(0.0, 1.0, 0.0, 0.0);
			head->setOrientation(0.0, 1.0, 0.0, 0.0);

			try {
				_model->addToNode(head, "headhook");
			} catch (Common::Exception &e) {
				destroyModel(head);
				e.add("Failed to attach head \"%s\" to creature \"%s\" (\"%s\")", headModel.c_str(), _tag.c_str(), _name.c_str());
				Common::printException(e, "WARNING: ");
			}
		}
	}

	_modelIDs.push_back(_model->getID());

	if (!isStatic())
		_model->setSelectable(true);

	// Positioning
	float radian, x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(radian, x, y, z);
	setOrientation(radian, x, y, z);
}

void Creature::unloadModel() {
	setVisible(false);

	destroyModel(_model);
	_model = 0;
}

void Creature::load(const Aurora::GFFStruct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Aurora::GFFFile *utc = 0;
	if (!temp.empty()) {
		try {
			utc = new Aurora::GFFFile(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' '));
		} catch (...) {
			delete utc;
		}
	}

	load(creature, utc ? &utc->getTopLevel() : 0);

	delete utc;
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

	float oRadian, oX, oY, oZ;
	convertVector2Orientation(bearingX, bearingY, oRadian, oX, oY, oZ);

	setOrientation(oRadian, oX, oZ, -oY);
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

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Portrait

	loadPortrait(gff, _portrait);

	// Gender
	_gender = gff.getUint("Gender", _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// PC
	_isPC = gff.getBool("IsPC", _isPC);

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
	loadClasses(gff, _classes, _hitDice);

	// Skills
	if (gff.hasField("SkillList")) {
		_skills.clear();

		const Aurora::GFFList &skills = gff.getList("SkillList");
		for (Aurora::GFFList::const_iterator s = skills.begin(); s != skills.end(); ++s) {
			const Aurora::GFFStruct &skill = **s;

			_skills.push_back(skill.getSint("Rank"));
		}
	}

	// Feats
	if (gff.hasField("FeatList")) {
		_feats.clear();

		const Aurora::GFFList &feats = gff.getList("FeatList");
		for (Aurora::GFFList::const_iterator f = feats.begin(); f != feats.end(); ++f) {
			const Aurora::GFFStruct &feat = **f;

			_feats.push_back(feat.getUint("Feat"));
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

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);
}

void Creature::loadPortrait(const Aurora::GFFStruct &gff, Common::UString &portrait) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait2DA = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait2DA.empty())
			portrait = "po_" + portrait2DA;
	}

	portrait = gff.getString("Portrait", portrait);
}

void Creature::loadClasses(const Aurora::GFFStruct &gff,
                           std::vector<Class> &classes, uint8 &hitDice) {

	if (!gff.hasField("ClassList"))
		return;

	classes.clear();
	hitDice = 0;

	const Aurora::GFFList &cClasses = gff.getList("ClassList");
	for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
		classes.push_back(Class());

		const Aurora::GFFStruct &cClass = **c;

		classes.back().classID = cClass.getUint("Class");
		classes.back().level   = cClass.getUint("ClassLevel");

		hitDice += classes.back().level;
	}
}

const Common::UString &Creature::getRaceName() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("Name");

	return TalkMan.getString(strRef);
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

const Common::UString &Creature::getClassName() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Name");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getClassDescription() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Description");

	return TalkMan.getString(strRef);
}

Common::UString Creature::getClassString() const {
	Common::UString classString;

	getClassString(_classes, classString);

	return classString;
}

uint8 Creature::getHitDice() const {
	return _hitDice;
}

uint8 Creature::getAbility(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

int8 Creature::getSkillRank(uint32 skill) const {
	if (skill >= _skills.size())
		return -1;

	return _skills[skill];
}

bool Creature::hasFeat(uint32 feat) const {
	for (std::vector<uint32>::const_iterator f = _feats.begin(); f != _feats.end(); ++f)
		if (*f == feat)
			return true;

	return false;
}

void Creature::setHighlight(bool highlight) {
	if (_model)
		_model->showBoundingBox(highlight);

	Object::setHighlight(highlight);
}

void Creature::getClassString(const std::vector<Class> &classes, Common::UString &str) {
	for (std::vector<Class>::const_iterator c = classes.begin(); c != classes.end(); ++c) {
		if (!str.empty())
			str += '/';

		uint32 strRef = TwoDAReg.get("classes").getRow(c->classID).getInt("Name");

		str += TalkMan.getString(strRef);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
