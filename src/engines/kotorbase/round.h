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
 *  Round controller for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_ROUND_H
#define ENGINES_KOTORBASE_ROUND_H

namespace Engines {

namespace KotORBase {

class Module;

class RoundController {
public:
	RoundController(Module *module);

	int getNextCombatRound() const;

	void update();

private:
	Module *_module;
	uint32_t _startTimestamp;
	int _combatRound;

	bool hasTimePassed(int ms);
	void raiseCombatRoundEnded();
	void raiseCombatRoundBegan();
	void raiseHeartbeat();
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_ROUND_H
