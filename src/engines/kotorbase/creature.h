/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Creature within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_CREATURE_H
#define ENGINES_KOTORBASE_CREATURE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/ptrmap.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/inventory.h"
#include "src/engines/kotorbase/action.h"

namespace Engines {

namespace KotORBase {

class CharacterGenerationInfo;
class Item;

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	/** Load from a creature template. */
	Creature(const Common::UString &resRef);
	~Creature();

	// Character generation

	/** Create a fake player character creature for testing purposes. */
	void createFakePC();
	/** Create a player character creature from a character info class. */
	void createPC(const CharacterGenerationInfo &info);

	// Basic visuals

	/** Is the creature's model visible? */
	bool isVisible() const;

	/** Show the creature's model. */
	void show();
	/** Hide the creature's model. */
	void hide();

	// Basic properties

	/** Get the gender of the creature. */
	Gender getGender() const;
	/** Get the level of the creature regarding a specific class. */
	int getLevel(const Class &c) const;
	/** Get the race of the creature. */
	Race getRace() const;
	/** Get the subrace of the creature. */
	SubRace getSubRace() const;
	/** Get the level by its position in the level vector. */
	int getLevelByPosition(int position) const;
	/** Get the class by its position in the level vector. */
	Class getClassByPosition(int position) const;
	/** Get the movement rate of this creature when walking. */
	float getWalkRate() const;
	/** Get the movement rate of this creature when running. */
	float getRunRate() const;
	/** Get id of the conversation with this creature. */
	const Common::UString &getConversation() const;

	/** Is the creature a player character? */
	bool isPC() const;
	/** Is the creature a party member? */
	bool isPartyMember() const;

	// Attributes

	/** Get the current rank of the specified skill. */
	int getSkillRank(Skill skill);
	/** Get the current score of the specified ability. */
	int getAbilityScore(Ability ability);

	// Positioning

	/** Get the camera height for this creature. */
	float getCameraHeight() const;

	/** Set the creature's position. */
	void setPosition(float x, float y, float z);
	/** Set the creature's orientation. */
	void setOrientation(float x, float y, float z, float angle);

	// Object/Cursor interactions

	/** The cursor entered the creature. */
	void enter();
	/** The cursor left the creature. */
	void leave();
	/** (Un)Highlight the creature. */
	void highlight(bool enabled);
	/** The creature was clicked. */
	bool click(Object *triggerer = 0);

	// Inventory and equipment

	Inventory &getInventory();
	Item *getEquipedItem(InventorySlot slot) const;

	void equipItem(Common::UString tag, InventorySlot slot);

	// Animation

	void playDefaultAnimation();
	void playDefaultHeadAnimation();

	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

	void playHeadAnimation(const Common::UString &anim,
	                       bool restart = true,
	                       float length = 0.0f,
	                       float speed = 1.0f);

	// Action queue

	/** Clear the action queue of this creature. */
	void clearActionQueue();
	/** Append action to the action queue of this creature. */
	void enqueueAction(const Action &action);
	/** Get the next action of this creature if exists.  */
	const Action *peekAction() const;
	/** Get the next action of this creature and remove it from the action queue. */
	const Action *dequeueAction();

protected:
	// Parts of a creature's body.
	struct PartModels {
		Common::UString type;

		Common::UString body;
		Common::UString head;

		Common::UString bodyTexture;
		Common::UString portrait;
	};

	Gender _gender;
	Skin _skin; ///< The skin type of the creature.
	uint8 _face; ///< The face of the creature.

	virtual void getPartModelsPC(PartModels &parts, uint32 state, uint8 textureVariation) = 0;
	void loadMovementRate(const Common::UString &name);

private:
	struct ClassLevel {
		Class characterClass;
		int level;
	};

	bool _isPC; ///< Is the creature a PC?

	uint32 _appearance; ///< The creature's general appearance.

	Race _race; ///< The race of the creature.
	SubRace _subRace; ///< The subrace of the creature.

	std::vector<ClassLevel> _levels; ///< The levels of the creature.

	Common::UString _modelType;
	Common::ScopedPtr<Graphics::Aurora::Model> _model; ///< The creature's model.
	Graphics::Aurora::Model *_headModel; ///< The creature's head model.
	bool _visible;

	// Abilities
	unsigned int _strength;
	unsigned int _dexterity;
	unsigned int _constitution;
	unsigned int _intelligence;
	unsigned int _wisdom;
	unsigned int _charisma;

	std::vector<uint32> _skills; ///< The skill levels of the creature

	Common::UString _conversation;

	Inventory _inventory;
	Common::PtrMap<InventorySlot, Item> _equipment;

	std::vector<Action> _actionQueue;

	float _walkRate;
	float _runRate;


	void init();

	void load(const Aurora::GFF3Struct &creature);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadPortrait(const Aurora::GFF3Struct &gff);
	void loadAppearance();

	void getPartModels(PartModels &parts, uint32 state = 'a');
	void loadBody(PartModels &parts);
	void loadHead(PartModels &parts);

	void changeBody();
	void changeWeapon(InventorySlot slot);

	void setDefaultAnimations();
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_CREATURE_H
