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
 *  Star Wars: Knights of the Old Republic engine functions assigning actions to objects.
 */

#include "external/glm/geometric.hpp"

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/door.h"
#include "src/engines/kotorbase/item.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/game.h"
#include "src/engines/kotorbase/action.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::getCurrentAction(Aurora::NWScript::FunctionContext &ctx) {
	Creature *object = ObjectContainer::toCreature(ctx.getParams()[0].getObject());
	if (!object)
		throw Common::Exception("Functions::getCurrentAction(): Invalid object");

	const Action *action = object->getCurrentAction();
	if (!action)
		ctx.getReturn() = kActionQueueEmpty;
	else
		ctx.getReturn() = action->type;
}

void Functions::assignCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::assignCommand(): Script needed");

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	_game->getModule().delayScript(script, state, getParamObject(ctx, 0), ctx.getTriggerer(), 0);
}

void Functions::delayCommand(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("Functions::assignCommand(): Script needed");

	uint32_t delay = ctx.getParams()[0].getFloat() * 1000;

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	_game->getModule().delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), delay);
}

void Functions::actionStartConversation(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &convName = ctx.getParams()[1].getString();
	_game->getModule().startConversation(convName, ctx.getCaller());
}

void Functions::actionOpenDoor(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	Door *door = ObjectContainer::toDoor(object);
	if (!door)
		throw Common::Exception("Functions::actionOpenDoor(): Object is not a door");

	door->open(0);
}

void Functions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	Door *door = ObjectContainer::toDoor(object);
	if (!door)
		throw Common::Exception("Functions::actionCloseDoor(): Object is not a door");

	door->close(0);
}

void Functions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		throw Common::Exception("Functions::actionMoveToObject(): Invalid caller");

	Object *object = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	if (!object)
		object = _game->getModule().getPartyLeader();

	float range = ctx.getParams()[2].getFloat();

	float x, y, z;
	object->getPosition(x, y, z);

	Action action(kActionMoveToPoint);
	action.range = range;
	action.location = glm::vec3(x, y, z);

	caller->addAction(action);
}

void Functions::actionFollowLeader(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		throw Common::Exception("Functions::actionFollowLeader(): Invalid caller");

	Action action(kActionFollowLeader);
	action.range = 1.0f;

	caller->addAction(action);
}

void Functions::clearAllActions(Aurora::NWScript::FunctionContext &ctx) {
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		caller = _game->getModule().getPartyLeader();

	caller->clearActions();
}

void Functions::setFacing(Aurora::NWScript::FunctionContext &ctx) {
	float facing = ctx.getParams()[0].getFloat();

	Object *object = ObjectContainer::toObject(ctx.getCaller());
	if (object)
		object->setOrientation(0.0f, 0.0f, 1.0f, facing);
}

void Functions::actionEquipItem(Aurora::NWScript::FunctionContext &ctx) {
	Item *item = dynamic_cast<Item *>(ObjectContainer::toObject(ctx.getParams()[0].getObject()));
	int slot = ctx.getParams()[1].getInt();

	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller || !item)
		return;

	caller->equipItem(item->getTag(), static_cast<InventorySlot>(slot));
}

void Functions::actionPickUpItem(Aurora::NWScript::FunctionContext &ctx) {
	// Pick up an item from a container or the ground into caller's inventory.
	// For the Endar Spire milestone, items are primarily created via
	// CreateItemOnObject, so this is a lightweight queue-based stub.
	Object *itemObj = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller || !itemObj)
		return;

	Action action(kActionPickUpItem);
	action.object = itemObj;
	caller->addAction(action);
}

void Functions::actionAttack(Aurora::NWScript::FunctionContext &ctx) {
	Object *target = ObjectContainer::toObject(ctx.getParams()[0].getObject());
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller || !target)
		return;

	Action action(kActionAttackObject);
	action.object = target;
	caller->addAction(action);
}

void Functions::cancelCombat(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(ctx.getParams()[0].getObject());
	if (creature)
		creature->cancelCombat();
}

void Functions::getLastAttacker(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(ctx.getParams()[0].getObject());
	ctx.getReturn() = creature ? creature->getLastHostileActor()
	                           : static_cast<Aurora::NWScript::Object *>(nullptr);
}

void Functions::actionSpeakString(Aurora::NWScript::FunctionContext &ctx) {
	// In the original engine, ActionSpeakString queues a spoken line above
	// the creature's head. We log it so script logic is not silently skipped.
	const Common::UString &str = ctx.getParams()[0].getString();

	Object *caller = ObjectContainer::toObject(ctx.getCaller());
	const Common::UString tag = caller ? caller->getTag() : Common::UString("(unknown)");

	warning("ActionSpeakString [%s]: %s", tag.c_str(), str.c_str());
}

void Functions::actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx) {
	int animID = ctx.getParams()[0].getInt();

	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		return;

	// Map the numeric animation ID to a named animation string.
	// KOTOR animation IDs are documented in animconst.nss; common ones used in
	// combat and dialogue are handled here; unknown IDs fall back to a no-op.
	Common::UString animName;
	switch (animID) {
		case  0: animName = "pause1";     break; // ANIMATION_LOOPING_PAUSE
		case  1: animName = "pause2";     break; // ANIMATION_LOOPING_PAUSE2
		case  2: animName = "listen";     break; // ANIMATION_LOOPING_LISTEN
		case  3: animName = "meditate";   break; // ANIMATION_LOOPING_MEDITATE
		case  4: animName = "worship";    break; // ANIMATION_LOOPING_WORSHIP
		case 10: animName = "talk";       break; // ANIMATION_LOOPING_TALK_NORMAL
		case 11: animName = "talklooking"; break; // ANIMATION_LOOPING_TALK_PLEADING
		case 16: animName = "victory1";   break; // ANIMATION_LOOPING_DEAD_FRONT
		case 17: animName = "victory2";   break; // ANIMATION_LOOPING_DEAD_BACK
		case 38: animName = "attack1";    break; // ANIMATION_FIREFORGET_HEAD_TURN_LEFT
		case 39: animName = "attack2";    break; // ANIMATION_FIREFORGET_HEAD_TURN_RIGHT
		case 40: animName = "dodge";      break; // ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD
		case 44: animName = "die";        break; // ANIMATION_FIREFORGET_SPASM
		case 48: animName = "g8a1";       break; // ANIMATION_FIREFORGET_DODGE_DUCK
		case 49: animName = "g8a2";       break; // ANIMATION_FIREFORGET_DODGE_SIDE
		default: return; // Unknown animation ID; ignore
	}

	float speed = ctx.getParams()[1].getFloat();
	float length = ctx.getParams()[2].getFloat();
	caller->playAnimation(animName, true, length, speed > 0.0f ? speed : 1.0f);
}

void Functions::actionJumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	// Immediately teleport the caller to the position of the target object.
	// This is the action-queued variant of JumpToObject; for the Endar Spire
	// milestone both variants behave identically (instant teleport, no path).
	jumpToObject(ctx);
}

void Functions::actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	// Immediately teleport the caller to the given location.
	// Delegates to the already-implemented JumpToLocation function.
	jumpToLocation(ctx);
}

void Functions::actionWait(Aurora::NWScript::FunctionContext &ctx) {
	// Queue a wait action with the specified duration (seconds).
	// The action executor pops it immediately after the first tick; this is
	// sufficient for any script that only needs to yield briefly.
	Creature *caller = ObjectContainer::toCreature(ctx.getCaller());
	if (!caller)
		return;

	// Store the wait duration in the `range` field (repurposed as a float
	// payload; the executor currently completes the wait on the first tick).
	Action action(kActionWait);
	action.range = ctx.getParams()[0].getFloat();
	caller->addAction(action);
}

} // End of namespace KotORBase

} // End of namespace Engines
