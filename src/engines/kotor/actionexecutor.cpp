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
 *  Creature action executor for Star Wars: Knights of the Old Republic.
 */

#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "src/common/maths.h"

#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotor/actionexecutor.h"
#include "src/engines/kotor/area.h"
#include "src/engines/kotor/module.h"

static const float kWalkDistance = 2.0f;

namespace Engines {

namespace KotOR {

void ActionExecutor::executeActions(Creature &creature, Area &area, float dt) {
	const Action *action = creature.peekAction();
	if (!action)
		return;

	switch (action->type) {
		case kActionMoveToPoint:
			executeMoveToPoint(creature, area, *action, dt);
			break;
		case kActionFollowLeader:
			executeFollowLeader(creature, area, *action, dt);
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
	area._module->getPC()->getPosition(x, y, z);
	moveTo(creature, area, x, y, 1.0f, dt);
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
	}
	else
		creature.playDefaultAnimation();

	return false;
}

} // End of namespace KotOR

} // End of namespace Engines
