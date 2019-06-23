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
 *  Creature action executor for KotOR games.
 */

#include "external/glm/vec3.hpp"
#include "external/glm/gtc/type_ptr.hpp"

#include "src/common/maths.h"

#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/actionexecutor.h"
#include "src/engines/kotorbase/area.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/door.h"
#include "src/engines/kotorbase/placeable.h"

static const float kWalkDistance = 2.0f;

namespace Engines {

namespace KotORBase {

void ActionExecutor::execute(const Action &action, const ExecutionContext &ctx) {
	switch (action.type) {
		case kActionMoveToPoint:
			executeMoveToPoint(action, ctx);
			break;
		case kActionFollowLeader:
			executeFollowLeader(action, ctx);
			break;
		case kActionOpenLock:
			executeOpenLock(action, ctx);
			break;
		case kActionUseObject:
			executeUseObject(action, ctx);
			break;
		default:
			warning("TODO: Handle action %u", (uint)action.type);
			break;
	}
}

void ActionExecutor::executeMoveToPoint(const Action &action, const ExecutionContext &ctx) {
	if (moveTo(action.location, action.range, ctx))
		ctx.creature->popAction();
}

void ActionExecutor::executeFollowLeader(const Action &action, const ExecutionContext &ctx) {
	Creature *partyLeader = ctx.area->_module->getPartyLeader();

	float x, y, _;
	partyLeader->getPosition(x, y, _);

	moveTo(glm::vec2(x, y), action.range, ctx);
}

void ActionExecutor::executeOpenLock(const Action &action, const ExecutionContext &ctx) {
	float x, y, _;
	action.object->getPosition(x, y, _);
	glm::vec2 location(x, y);

	bool locReached = isLocationReached(location, action.range, ctx);
	if (!locReached) {
		moveTo(location, action.range, ctx);
		return;
	}

	ctx.creature->popAction();

	if (ctx.creature != ctx.area->_module->getPartyLeader()) {
		warning("ActionExecutor::executeOpenLock(): Creature is not the party leader");
		return;
	}

	Door *door = dynamic_cast<Door *>(action.object);
	if (door) {
		ctx.creature->playAnimation("unlockdr", false);
		door->unlock(ctx.creature);
		return;
	}

	Placeable *placeable = dynamic_cast<Placeable *>(action.object);
	if (placeable) {
		ctx.creature->playAnimation("unlockcntr", false);
		placeable->unlock(ctx.creature);
		return;
	}

	warning("Cannot unlock an object that is not a door or a placeable");
}

void ActionExecutor::executeUseObject(const Action &action, const ExecutionContext &ctx) {
	float x, y, _;
	action.object->getPosition(x, y, _);
	glm::vec2 location(x, y);

	bool locReached = isLocationReached(location, action.range, ctx);
	if (!locReached) {
		moveTo(location, action.range, ctx);
		return;
	}

	ctx.creature->popAction();

	Module *module = ctx.area->_module;

	if (ctx.creature != module->getPartyLeader()) {
		warning("ActionExecutor::executeUseObject(): Creature is not the party leader");
		return;
	}

	action.object->click(module->getPartyLeader());

	Creature *creatureObject = ObjectContainer::toCreature(action.object);
	if (creatureObject) {
		const Common::UString conversation = creatureObject->getConversation();
		if (!conversation.empty())
			module->delayConversation(conversation, creatureObject);

		return;
	}

	Situated *situated = ObjectContainer::toSituated(action.object);
	if (situated) {
		const Common::UString conversation = situated->getConversation();
		if (!conversation.empty()) {
			module->delayConversation(conversation, situated);
			return;
		}

		Placeable *placeable = ObjectContainer::toPlaceable(action.object);
		if (placeable && placeable->hasInventory()) {
			module->delayContainer(placeable);
			return;
		}
	}
}

bool ActionExecutor::isLocationReached(const glm::vec2 &location, float range, const ExecutionContext &ctx) {
	float x, y, _;
	ctx.creature->getPosition(x, y, _);
	return glm::distance(glm::vec2(x, y), location) <= range;
}

bool ActionExecutor::moveTo(const glm::vec2 &location, float range, const ExecutionContext &ctx) {
	if (isLocationReached(location, range, ctx))
		return true;

	ctx.creature->makeLookAt(location.x, location.y);

	float oX, oY, oZ;
	ctx.creature->getPosition(oX, oY, oZ);
	glm::vec2 origin(oX, oY);

	glm::vec2 diff = location - origin;
	glm::vec2 dir = glm::normalize(diff);

	float dist = glm::length(diff);
	bool run = dist > kWalkDistance;
	float moveRate = run ? ctx.creature->getRunRate() : ctx.creature->getWalkRate();

	float x = origin.x + moveRate * dir.x * ctx.frameTime;
	float y = origin.y + moveRate * dir.y * ctx.frameTime;
	float z = ctx.area->evaluateElevation(x, y);

	bool haveMovement = (z != FLT_MIN) &&
	                     ctx.area->walkable(glm::vec3(oX, oY, oZ + 0.1f),
	                                        glm::vec3(x, y, z + 0.1f));

	if (haveMovement) {
		ctx.creature->playAnimation(run ? "run" : "walk", false, -1.0f);
		ctx.creature->setPosition(x, y, z);

		if (ctx.creature == ctx.area->_module->getPartyLeader())
			ctx.area->_module->movedPartyLeader();
		else
			ctx.area->notifyObjectMoved(*ctx.creature);

		diff = location - glm::vec2(x, y);
		dist = glm::length(diff);

		if (dist <= range) {
			ctx.creature->playDefaultAnimation();
			return true;
		}

	} else {
		ctx.creature->playDefaultAnimation();
	}

	return false;
}

} // End of namespace KotORBase

} // End of namespace Engines
