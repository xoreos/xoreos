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

#include "aurora/talkman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "engines/aurora/util.h"

#include "engines/nwn/creature.h"

namespace Engines {

namespace NWN {

Creature::Creature() {
}

Creature::~Creature() {
}

void Creature::clear() {
	_firstName.clear();
	_lastName.clear();
	_description.clear();

	_fullName.clear();

	_portrait.clear();
	_portraitID = 0xFFFFFFFF;

	_age = 0;

	_xp = 0;

	_classes.clear();
}

void Creature::loadCharacter(Common::SeekableReadStream &stream) {
	Aurora::GFFFile gff;
	loadGFF(gff, stream, MKID_BE('BIC '));

	load(gff.getTopLevel());
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
	_portraitID = gffTop.getUint("PortraitId", 0xFFFFFFFF);

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
}

Common::UString Creature::createFullName() {
	Aurora::Language language = TalkMan.getMainLanguage();

	Common::UString first;
	first = _firstName.getString(language);
	if (first.empty())
		first = _firstName.getFirstString();

	Common::UString last;
	last = _lastName.getString(language);
	if (last.empty())
		last = _lastName.getFirstString();

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

		Common::UString portrait = twoda.getCellString(_portraitID, "BaseResRef");
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

		uint32 strRef = TwoDAReg.get("classes").getCellInt(c->classID, "Name");

		classString += TalkMan.getString(strRef);
	}

	return classString;
}

uint32 Creature::getAge() const {
	return _age;
}

uint32 Creature::getXP() const {
	return _xp;
}

} // End of namespace NWN

} // End of namespace Engines
