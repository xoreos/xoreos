/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/creature.h
 *  NWN creature.
 */

#ifndef ENGINES_NWN_CREATURE_H
#define ENGINES_NWN_CREATURE_H

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/locstring.h"

namespace Aurora {
	class GFFStruct;
}

namespace Engines {

namespace NWN {

class Creature {
public:
	Creature();
	~Creature();

	/** Clear the creature information. */
	void clear();

	/** Load from a character file. */
	void loadCharacter(const Common::UString &name);

	/** Return the creature's first name. */
	const Aurora::LocString &getFirstName() const;
	/** Return the creature's last name. */
	const Aurora::LocString &getLastName() const;
	/** Return the creature's description. */
	const Aurora::LocString &getDescription() const;

	/** Return the creature's full name. */
	const Common::UString &getFullName() const;

	/** Return the creature's age. */
	uint32 getAge() const;

	/** Return the creature's XP. */
	uint32 getXP() const;

private:
	Aurora::LocString _firstName;
	Aurora::LocString _lastName;
	Aurora::LocString _description;

	Common::UString _fullName;

	uint32 _age;

	uint32 _xp;

	void load(const Aurora::GFFStruct &gffTop);

	Common::UString createFullName();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CREATURE_H
