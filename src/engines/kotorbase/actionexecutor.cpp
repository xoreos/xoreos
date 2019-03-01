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

#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

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

void ActionExecutor::executeActions(Creature &creature, Area &area, float dt) {
	const Action *action = creature.getCurrentAction();
	if (!action)
		return;

	switch (action->type) {
		case kActionMoveToPoint:
			executeMoveToPoint(creature, area, *action, dt);
			break;
		case kActionFollowLeader:
			executeFollowLeader(creature, area, *action, dt);
			break;
		case kActionOpenLock:
			executeOpenLock(creature, area, *action, dt);
			break;
		default:
			warning("TODO: Handle action %u", (uint)action->type);
			break;
	}
}

void ActionExecutor::executeMoveToPoint(Creature &creature, Area &area, const Action &action, float dt) {
	float x = action.location.x;
	float y = action.location.y;

	if (moveTo(creature, area, x, y, action.range, dt))
		creature.dequeueAction();
}

void ActionExecutor::executeFollowLeader(Creature &creature, Area &area, const Action &UNUSED(action), float dt) {
	float x, y, z;
	area._module->getPartyLeader()->getPosition(x, y, z);
	moveTo(creature, area, x, y, 1.0f, dt);
}

void ActionExecutor::executeOpenLock(Creature &creature, Area &area, const Action &action, float dt) {
	float x, y, _;
	action.object->getPosition(x, y, _);

	bool locReached = isLocationReached(creature, x, y, 1.0f);
	if (!locReached) {
		moveTo(creature, area, x, y, 1.0f, dt);
		return;
	}

	creature.dequeueAction();

	Door *door = dynamic_cast<Door *>(action.object);
	if (door) {
		creature.playAnimation("unlockdr", false);
		if (door) {
			door->unlock(&creature);
		}
	} else {
		Placeable *placeable = dynamic_cast<Placeable *>(action.object);
		if (placeable) {
			creature.playAnimation("unlockcntr", false);
			placeable->unlock(&creature);
		} else {
			warning("Cannot unlock an object that is not a door or a placeable");
			return;
		}
	}
}

bool ActionExecutor::isLocationReached(Creature &creature, float x, float y, float range) {
	float oX, oY, _;
	creature.getPosition(oX, oY, _);

	float dx = x - oX;
	float dy = y - oY;

	return dx * dx + dy * dy <= range * range;
}

bool ActionExecutor::moveTo(Creature &creature, Area &area, float x, float y, float range, float dt) {
	float oX, oY, oZ;
	creature.getPosition(oX, oY, oZ);

	glm::vec2 origin(oX, oY);
	glm::vec2 diff = glm::vec2(x, y) - origin;
	float dist = glm::length(diff);

	if (dist <= range) {
		creature.playDefaultAnimation();
		return true;
	}

	creature.makeLookAt(x, y);

	bool run = dist > kWalkDistance;
	float moveRate = run ? creature.getRunRate() : creature.getWalkRate();
	glm::vec2 dir = glm::normalize(diff);
	float newX = origin.x + moveRate * dir.x * dt;
	float newY = origin.y + moveRate * dir.y * dt;
	float z = area.evaluateElevation(newX, newY);

	bool haveMovement = (z != FLT_MIN) &&
	                     area.walkable(glm::vec3(oX, oY, oZ + 0.1f),
	                                   glm::vec3(newX, newY, z + 0.1f));

	if (haveMovement) {
		creature.playAnimation(run ? "run" : "walk", false, -1.0f);
		creature.setPosition(newX, newY, z);

		if (&creature == area._module->getPartyLeader())
			area._module->movedPartyLeader();
		else
			area.notifyObjectMoved(creature);

	} else {
		creature.playDefaultAnimation();
	}

	return false;
}

} // End of namespace KotORBase

} // End of namespace Engines
