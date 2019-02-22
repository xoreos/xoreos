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

#ifndef ENGINES_KOTORBASE_ACTIONEXECUTOR_H
#define ENGINES_KOTORBASE_ACTIONEXECUTOR_H

namespace Engines {

namespace KotORBase {

struct Action;
class Area;
class Creature;

class ActionExecutor {
public:
	static void executeActions(Creature &creature, Area &area, float dt);
	static void executeMoveToPoint(Creature &creature, Area &area, const Action &action, float dt);
	static void executeFollowLeader(Creature &creature, Area &area, const Action &action, float dt);

private:
	/**
	 * Move the creature towards a point. Returns true if the point is
	 * within the specified range, false otherwise.
	 */
	static bool moveTo(Creature &creature, Area &area, float x, float y, float range, float dt);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_ACTIONEXECUTOR_H
