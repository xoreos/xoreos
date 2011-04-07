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

#include "common/endianness.h"

#include "aurora/types.h"
#include "aurora/talkman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "events/events.h"

#include "engines/aurora/util.h"

#include "engines/nwn/creature.h"

namespace Engines {

namespace NWN {

Creature::Creature() : _lastChangedGUIDisplay(0) {
}

Creature::~Creature() {
}

void Creature::clear() {
	_firstName.clear();
	_lastName.clear();
	_description.clear();

	_fullName.clear();

	_portrait.clear();
	_portraitID = Aurora::kStrRefInvalid;

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_classes.clear();

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

uint32 Creature::lastChangedGUIDisplay() const {
	return _lastChangedGUIDisplay;
}

void Creature::loadCharacter(Common::SeekableReadStream &stream) {
	Aurora::GFFFile gff;
	loadGFF(gff, stream, MKID_BE('BIC '));

	load(gff.getTopLevel());

	// All BICs should be PCs.
	_isPC = true;

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

void Creature::load(const Aurora::GFFStruct &gffTop) {
	clear();

	// Name and description
	gffTop.getLocString("FirstName"  , _firstName);
	gffTop.getLocString("LastName"   , _lastName);
	gffTop.getLocString("Description", _description);

	_fullName = createFullName();

	// Portrait
	_portrait   = gffTop.getString("Portrait");
	_portraitID = gffTop.getUint("PortraitId", Aurora::kStrRefInvalid);

	// PC and DM
	_isPC = gffTop.getBool("IsPC", false);
	_isDM = gffTop.getBool("IsDM", false);

	// Age
	_age = gffTop.getUint("Age", 0);

	// Experience
	_xp = gffTop.getUint("Experience", 0);

	// Classes
	if (gffTop.hasField("ClassList")) {
		const Aurora::GFFList &cClasses = gffTop.getList("ClassList");

		for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
			_classes.push_back(Class());

			const Aurora::GFFStruct &cClass = **c;

			_classes.back().classID = cClass.getUint("Class");
			_classes.back().level   = cClass.getUint("ClassLevel");
		}
	}

	// Health
	_baseHP    = gffTop.getSint("HitPoints", 0);
	_bonusHP   = gffTop.getSint("MaxHitPoints", _baseHP) - _baseHP;
	_currentHP = gffTop.getSint("CurrentHitPoints", _baseHP);
}

Common::UString Creature::createFullName() {
	Aurora::Language language = TalkMan.getMainLanguage();

	Common::UString first;
	first = _firstName.getString(language);
	if (first.empty())
		first = _firstName.getString();

	Common::UString last;
	last = _lastName.getString(language);
	if (last.empty())
		last = _lastName.getString();

	if (first.empty() && last.empty())
		return "";

	if (first.empty())
		return last;
	if (last.empty())
		return first;

	return first + " " + last;
}

const Aurora::LocString &Creature::getFirstName() const {
	return _firstName;
}

const Aurora::LocString &Creature::getLastName() const {
	return _lastName;
}

const Aurora::LocString &Creature::getDescription() const {
	return _description;
}

const Common::UString &Creature::getFullName() const {
	return _fullName;
}

Common::UString Creature::getPortrait() const {
	if (_portrait.empty()) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait = twoda.getRow(_portraitID).getString("BaseResRef");
		if (!portrait.empty())
			return "po_" + portrait;

		return "";
	}

	return _portrait;
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

} // End of namespace NWN

} // End of namespace Engines
