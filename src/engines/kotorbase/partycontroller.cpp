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
 *  Handles the party management in KotOR games.
 */

#include "src/common/error.h"

#include "src/engines/kotorbase/partycontroller.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"

namespace Engines {

namespace KotORBase {

PartyController::PartyController(Module *module) :
		_module(module) {
}

size_t PartyController::getPartyMemberCount() const {
	return _party.size();
}

Creature *PartyController::getPartyLeader() const {
	return _party[0].second;
}

const std::pair<int, Creature *> &PartyController::getPartyMemberByIndex(int index) const {
	if (index >= static_cast<int>(_party.size()))
		throw Common::Exception("PartyController::getPartyMember(): Invalid index");

	return _party[index];
}

std::vector<int> PartyController::getPartyMembers() const {
	std::vector<int> partyMembers;
	for (auto partyMember : _party) {
		partyMembers.push_back(partyMember.first);
	}
	return partyMembers;
}

bool PartyController::isObjectPartyMember(Creature *object) const {
	for (auto partyMember : _party) {
		if (partyMember.second == object)
			return true;
	}
	return false;
}

void PartyController::clearCurrentParty() {
	_party.clear();
}

void PartyController::addPartyMember(int npc, Creature *creature) {
	if (!_party.empty()) {
		Creature *partyLeader = getPartyLeader();
		float x, y, z;
		partyLeader->getPosition(x, y, z);
		creature->setPosition(x, y, z);
	}

	_party.push_back(std::make_pair(npc, creature));

	if (npc != -1) {
		_module->getCurrentArea()->addCreature(creature);
		creature->show();
	}
}

void PartyController::setPartyLeader(int npc) {
	int index = -1;

	int partySize = static_cast<int>(_party.size());
	for (int i = 1; i < partySize; ++i) {
		if (_party[i].first == npc) {
			index = i;
			break;
		}
	}

	if (index == -1)
		return;

	setPartyLeaderByIndex(index);
}

void PartyController::setPartyLeaderByIndex(int index) {
	std::pair<int, Creature *> tmp = _party[0];
	_party[0] = _party[index];
	_party[index] = tmp;

	_party[0].second->setUsable(false);
	_party[0].second->clearAllActions();

	_party[index].second->setUsable(true);
}

const Common::UString &PartyController::getAvailableNPCTemplate(int npc) const {
	auto it = _availableParty.find(npc);
	if (it == _availableParty.end())
		throw Common::Exception("PartyController::getAvailableNPCTemplate: Invalid NPC");

	return it->second;
}

bool PartyController::isAvailableCreature(int npc) const {
	return _availableParty.find(npc) != _availableParty.end();
}

void PartyController::clearAvailableParty() {
	_availableParty.clear();
}

void PartyController::addAvailableNPCByTemplate(int npc, const Common::UString &templ) {
	auto i = _availableParty.find(npc);
	if (i != _availableParty.end())
		_availableParty.erase(i);

	_availableParty.insert(std::make_pair(npc, templ));
}

void PartyController::raiseHeartbeatEvent() {
	for (auto partyMember : _party) {
		partyMember.second->runScript("k_ai_master", partyMember.second, _module->getCurrentArea());
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
