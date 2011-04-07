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

#include "graphics/aurora/types.h"

#include "engines/nwn/object.h"

namespace Aurora {
	class GFFStruct;
}

namespace Engines {

namespace NWN {

class Tooltip;

class Creature : public Object {
public:
	Creature();
	~Creature();

	/** Clear the creature information. */
	void clear();

	/** Last time info was changed that's displayed in the GUI. */
	uint32 lastChangedGUIDisplay() const;

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

	void enter();
	void leave();

	void highlight(bool enabled);

	/** Load from a character file. */
	void loadCharacter(Common::SeekableReadStream &stream);
	/** Load from a creature instance. */
	void load(const Aurora::GFFStruct &creature);

	/** Load the creature's model. */
	void loadModel();
	/** Unload the creature's model. */
	void unloadModel();

	/** Return the creature's first name. */
	const Common::UString &getFirstName() const;
	/** Return the creature's last name. */
	const Common::UString &getLastName() const;

	/** Return the creature's class description. */
	Common::UString getClassString() const;

	bool isPC() const; ///< Is the creature a player character?
	bool isDM() const; ///< Is the creature a dungeon master?

	/** Return the creature's age. */
	uint32 getAge() const;

	/** Return the creature's XP. */
	uint32 getXP() const;

	/** Return the current HP this creature has. */
	int32 getCurrentHP() const;
	/** Return the max HP this creature can have. */
	int32 getMaxHP() const;

private:
	struct Class {
		uint32 classID;
		uint16 level;
	};

	uint32 _lastChangedGUIDisplay;

	Common::UString _firstName;
	Common::UString _lastName;

	uint32 _gender;
	uint32 _race;

	bool _isPC;
	bool _isDM;

	uint32 _age;

	uint32 _xp;

	int32 _baseHP;
	int32 _bonusHP;
	int32 _currentHP;

	std::vector<Class> _classes;

	uint32 _appearanceID;
	uint32 _phenotype;

	uint32 _head;
	uint32 _neck;
	uint32 _torso;
	uint32 _pelvis;
	uint32 _belt;
	uint32 _rFoot;
	uint32 _lFoot;
	uint32 _rShin;
	uint32 _lShin;
	uint32 _lThigh;
	uint32 _rThigh;
	uint32 _rFArm;
	uint32 _lFArm;
	uint32 _rBicep;
	uint32 _lBicep;
	uint32 _rShoul;
	uint32 _lShoul;
	uint32 _rHand;
	uint32 _lHand;

	uint32 _colorSkin;
	uint32 _colorHair;
	uint32 _colorTattoo1;
	uint32 _colorTattoo2;

	Graphics::Aurora::Model *_model;

	Tooltip *_tooltip;


	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);

	void createTooltip();
	void showTooltip();
	void hideTooltip();

};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CREATURE_H
