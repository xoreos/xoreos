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
 *  A creature in a Star Wars: Knights of the Old Republic area.
 */

#ifndef ENGINES_KOTOR_CREATURE_H
#define ENGINES_KOTOR_CREATURE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/ptrmap.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotor/object.h"
#include "src/engines/kotor/inventory.h"
#include "src/engines/kotor/action.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationInfo;
class Item;

class Creature : public Object {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	~Creature();

	/** Create a fake player character creature for testing purposes. */
	void createFakePC();
	/** Create a player character creature from a character info class. */
	void createPC(const CharacterGenerationInfo &info);

	// Basic visuals

	void show(); ///< Show the creature's model.
	void hide(); ///< Hide the creature's model.
	bool isVisible() const; ///< Is the creature's model visible?

	// Basic properties

	bool isPC() const; ///< Is the creature a player character?
	bool isPartyMember() const;

	Gender getGender() const; ///< Get the gender of the creature.
	int getLevel(const Class &c) const; ///< Get the level of the creature regarding a specific class.

	Race getRace() const; ///< Get the race of the creature.
	SubRace getSubRace() const; ///< Get the subrace of the creature.

	int getLevelByPosition(int position) const; ///< Get the level by its position in the level vector.
	Class getClassByPosition(int position) const; ///< Get the class by its position in the level vector.

	float getWalkRate() const;
	float getRunRate() const;

	// Positioning

	/** Set the creature's position. */
	void setPosition(float x, float y, float z);
	/** Set the creature's orientation. */
	void setOrientation(float x, float y, float z, float angle);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the creature.
	void leave(); ///< The cursor left the creature.

	/** (Un)Highlight the creature. */
	void highlight(bool enabled);

	/** The creature was clicked. */
	bool click(Object *triggerer = 0);

	const Common::UString &getConversation() const;

	float getCameraHeight() const;

	// Inventory and equipment

	void equipItem(Common::UString tag, EquipmentSlot slot);

	Inventory &getInventory();
	Item *getEquipedItem(EquipmentSlot slot) const;

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

	void clearActionQueue();

	/** Append action to the character's action queue. */
	void enqueueAction(const Action &action);

	const Action *peekAction() const;
	const Action *dequeueAction();

private:
	/** Parts of a creature's body. */
	struct PartModels {
		Common::UString type;

		Common::UString body;
		Common::UString head;

		Common::UString bodyTexture;
		Common::UString portrait;
	};

	/** A class level. */
	struct ClassLevel {
		Class characterClass;
		int level;
	};

	bool _isPC; ///< Is the creature a PC?

	uint32 _appearance; ///< The creature's general appearance.

	Race _race; ///< The race of the creature.
	SubRace _subRace; ///< The subrace of the creature.

	Gender _gender;
	std::vector<ClassLevel> _levels; ///< The levels of the creature.
	Skin _skin; ///< The skin type of the creature.
	uint8 _face; ///< The face of the creature.

	Common::UString _modelType;
	Common::ScopedPtr<Graphics::Aurora::Model> _model; ///< The creature's model.
	Graphics::Aurora::Model *_headModel; ///< The creature's head model.
	bool _visible;

	Common::UString _conversation;

	Inventory _inventory;
	Common::PtrMap<EquipmentSlot, Item> _equipment;

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
	void getPartModelsPC(PartModels &parts, uint32 state, uint8 textureVariation);
	void loadBody(PartModels &parts);
	void loadHead(PartModels &parts);
	void loadMovementRate(const Common::UString &name);

	void changeBody();
	void changeWeapon(EquipmentSlot slot);

	void setDefaultAnimations();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CREATURE_H
