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

#include <set>

#include "src/common/types.h"
#include <memory>
#include "src/common/ustring.h"
#include "src/common/ptrmap.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/inventory.h"
#include "src/engines/kotorbase/action.h"
#include "src/engines/kotorbase/creatureinfo.h"
#include "src/engines/kotorbase/actionqueue.h"

namespace Engines {

namespace KotORBase {

class CharacterGenerationInfo;
class Item;
struct CreatureSearchCriteria;

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

	/** Initialize this creature as a fake player character for testing purposes. */
	void initAsFakePC();
	/** Initialize this creature as a player character from a specified info class. */
	void initAsPC(const CharacterGenerationInfo &chargenInfo, const CreatureInfo &info);

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
	/** Get abstract information of this creature. */
	CreatureInfo &getCreatureInfo();

	/** Is the creature a player character? */
	bool isPC() const;
	/** Is the creature a party member? */
	bool isPartyMember() const;

	/** Does this creature match a specified search criteria? */
	bool matchSearchCriteria(const Object *target, const CreatureSearchCriteria &criteria) const;

	// Interactive properties

	bool isCommandable() const;

	/** Toggle usability of this object. */
	void setUsable(bool usable);
	/** Set whether this creature's action queue can be modified. */
	void setCommandable(bool commandable);

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

	const Common::UString &getCursor() const;

	/** (Un)Highlight the creature. */
	void highlight(bool enabled);
	/** The creature was clicked. */
	bool click(Object *triggerer = 0);

	// Inventory and equipment

	Inventory &getInventory();
	Item *getEquipedItem(InventorySlot slot) const;
	float getMaxAttackRange() const;

	void equipItem(Common::UString tag, InventorySlot slot, bool updateModel = true);
	void equipItem(Common::UString tag, InventorySlot slot, CreatureInfo &invOwner, bool updateModel = true);

	// Animation

	void playDefaultAnimation();
	void playDefaultHeadAnimation();
	void playDrawWeaponAnimation();
	void playAttackAnimation();
	void playDodgeAnimation();

	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

	void playHeadAnimation(const Common::UString &anim,
	                       bool restart = true,
	                       float length = 0.0f,
	                       float speed = 1.0f);

	// Actions

	/** Get the current action that this creature is executing. */
	const Action *getCurrentAction() const;

	/** Clear all actions of this creature. */
	void clearActions();
	/** Add action to the action queue of this creature. */
	void addAction(const Action &action);
	/** Remove the current action from the action queue of this creature. */
	void popAction();

	// Tooltip

	void getTooltipAnchor(float &x, float &y, float &z) const;

	// Perception

	void updatePerception(Creature &object);

	// Combat

	bool isInCombat() const;
	Object *getAttackTarget() const;
	int getAttackRound() const;
	Object *getAttemptedAttackTarget() const;

	void setAttemptedAttackTarget(Object *target);

	void startCombat(Object *target, int round);
	void cancelCombat();
	void executeAttack(Object *target);

	// Death

	bool isDead() const override;

	/** Handle creature's death. Return true if changed from alive to dead. */
	bool handleDeath();

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
	uint8_t _face; ///< The face of the creature.

	virtual void getPartModelsPC(PartModels &parts, uint32_t state, uint8_t textureVariation) = 0;
	void loadMovementRate(const Common::UString &name);

private:
	// Perception

	std::set<Object *> _seenObjects;
	std::set<Object *> _heardObjects;

	// Combat

	bool _inCombat { false };
	Object *_attackTarget { nullptr };
	int _attackRound { 0 };
	Object *_attemptedAttackTarget { nullptr };


	bool _isPC; ///< Is the creature a PC?

	uint32_t _appearance; ///< The creature's general appearance.

	Race _race; ///< The race of the creature.
	SubRace _subRace; ///< The subrace of the creature.

	Common::UString _modelType;
	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The creature's model.
	Graphics::Aurora::Model *_headModel; ///< The creature's head model.
	bool _visible;
	bool _commandable;

	CreatureInfo _info;

	Common::UString _conversation;

	Common::PtrMap<InventorySlot, Item> _equipment;

	ActionQueue _actions;

	float _walkRate;
	float _runRate;

	bool _dead { false };


	// Perception

	void handleObjectSeen(Object &object);
	void handleObjectVanished(Object &object);
	void handleObjectHeard(Object &object);
	void handleObjectInaudible(Object &object);


	void init();

	void load(const Aurora::GFF3Struct &creature);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff, bool clearScripts = true);
	void loadPortrait(const Aurora::GFF3Struct &gff);
	void loadEquipment(const Aurora::GFF3Struct &gff);
	void loadAbilities(const Aurora::GFF3Struct &gff);

	void getModelState(uint32_t &state, uint8_t &textureVariation);
	void getPartModels(PartModels &parts, uint32_t state, uint8_t textureVariation);
	void loadBody(PartModels &parts);
	void loadHead(PartModels &parts);

	void loadEquippedModel();
	void attachWeaponModel(InventorySlot slot);

	void setDefaultAnimations();

	void reloadEquipment();
	bool addItemToEquipment(const Common::UString &tag, InventorySlot slot);

	int getWeaponAnimationNumber() const;
	int computeWeaponDamage(const Item *weapon) const;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_CREATURE_H
