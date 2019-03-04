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
	struct ExecutionContext {
		Creature *creature { nullptr };
		Area *area { nullptr };
		float frameTime { 0.0f };
	};

	static void executeActions(const ExecutionContext &ctx);

private:
	/** Get if the current creature has reached a specified location. */
	static bool isLocationReached(const glm::vec2 &location, float range, const ExecutionContext &ctx);

	static void executeMoveToPoint(const Action &action, const ExecutionContext &ctx);
	static void executeFollowLeader(const Action &action, const ExecutionContext &ctx);
	static void executeOpenLock(const Action &action, const ExecutionContext &ctx);
	static void executeUseObject(const Action &action, const ExecutionContext &ctx);

	/**
	 * Move the current creature towards a specified location. Returns
	 * true if location is within a specified range.
	 */
	static bool moveTo(const glm::vec2 &location, float range, const ExecutionContext &ctx);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_ACTIONEXECUTOR_H
