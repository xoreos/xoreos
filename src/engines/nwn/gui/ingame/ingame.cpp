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
 *  The NWN ingame GUI elements.
 */

#include <cassert>

#include "src/common/error.h"

#include "src/engines/nwn/module.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/ingame/ingame.h"
#include "src/engines/nwn/gui/ingame/main.h"
#include "src/engines/nwn/gui/ingame/quickbar.h"
#include "src/engines/nwn/gui/ingame/quickchat.h"
#include "src/engines/nwn/gui/ingame/compass.h"
#include "src/engines/nwn/gui/ingame/partyleader.h"
#include "src/engines/nwn/gui/ingame/dialog.h"

namespace Engines {

namespace NWN {

IngameGUI::IngameGUI(Module &module, ::Engines::Console *console) :
	_module(&module), _lastCompassChange(0) {

	_main = std::make_unique<IngameMainMenu>(_module->getGameVersion(), console);

	_quickbar  = std::make_unique<Quickbar>();
	_quickchat = std::make_unique<Quickchat>(_quickbar->getHeight() - 3.0f);
	_compass   = std::make_unique<Compass>(_quickbar->getHeight() + _quickchat->getHeight() - 6.0f);

	_party.push_back(new PartyLeader(module));

	_lastPartyMemberChange.resize(1);
	_lastPartyMemberChange[0] = 0;
}

IngameGUI::~IngameGUI() {
	hide();
}

uint32_t IngameGUI::showMain() {
	_main->show();
	uint32_t code = _main->run();
	_main->hide();

	return code;
}

void IngameGUI::abortMain() {
	_main->abort();
}

void IngameGUI::show() {
	_quickbar->show();
	_quickchat->show();
	_compass->show();

	for (Common::PtrVector<CharacterInfo>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->show();

	if (_dialog)
		_dialog->show();
}

void IngameGUI::hide() {
	if (_dialog)
		_dialog->hide();

	for (Common::PtrVector<CharacterInfo>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->hide();

	_compass->hide();
	_quickchat->hide();
	_quickbar->hide();
}

void IngameGUI::addEvent(const Events::Event &event) {
	// The dialog takes preference
	if (_dialog) {
		_dialog->addEvent(event);
		return;
	}

	for (Common::PtrVector<CharacterInfo>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->addEvent(event);

	_compass->addEvent(event);
	_quickchat->addEvent(event);
	_quickbar->addEvent(event);
}

void IngameGUI::processEventQueue() {
	// The dialog takes preference
	if (_dialog) {
		if (_dialog->processEventQueue() != 0)
			stopConversation();

		return;
	}

	for (Common::PtrVector<CharacterInfo>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->processEventQueue();

	_compass->processEventQueue();
	_quickchat->processEventQueue();
	_quickbar->processEventQueue();
}

void IngameGUI::setPortrait(size_t partyMember, const Common::UString &portrait) {
	_party[partyMember]->setPortrait(portrait);
}

void IngameGUI::setHealth(size_t partyMember, uint32_t current, uint32_t max) {
	_party[partyMember]->setHealth(current, max);
}

void IngameGUI::setName(size_t partyMember, const Common::UString &name) {
	_party[partyMember]->setName(name);
}

void IngameGUI::setArea(const Common::UString &area) {
	for (Common::PtrVector<CharacterInfo>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->setArea(area);
}

void IngameGUI::setHealthy(size_t partyMember) {
	_party[partyMember]->setHealthColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void IngameGUI::setSick(size_t partyMember) {
	_party[partyMember]->setHealthColor(189.0f / 255.0f, 146.0f / 255.0f,  74.0f / 255.0f, 1.0f);
}

void IngameGUI::setPoisoned(size_t partyMember) {
	_party[partyMember]->setHealthColor(132.0f / 255.0f, 182.0f / 255.0f,  74.0f / 255.0f, 1.0f);
}

void IngameGUI::updatePartyMember(size_t partyMember, const Creature &creature, bool force) {
	assert(partyMember < _party.size());

	uint32_t lastPartyMemberChange = creature.lastChangedGUIDisplay();
	if (!force && (lastPartyMemberChange <= _lastPartyMemberChange[partyMember]))
		return;

	setPortrait(partyMember, creature.getPortrait());
	setName    (partyMember, creature.getName());
	setHealth  (partyMember, creature.getCurrentHP(), creature.getMaxHP());

	_lastPartyMemberChange[partyMember] = lastPartyMemberChange;
}

bool IngameGUI::hasRunningConversation() const {
	return _dialog.get() != 0;
}

bool IngameGUI::startConversation(const Common::UString &conv,
                                  Creature &pc, Object &obj, bool playHello) {
	stopConversation();

	if (conv.empty())
		return true;

	try {
		_dialog = std::make_unique<Dialog>(conv, pc, obj, *_module, playHello);

		_dialog->show();
	} catch (...) {
		_dialog.reset();

		Common::exceptionDispatcherWarning("Failed starting conversation \"%s\"", conv.c_str());
		return false;
	}

	return true;
}

void IngameGUI::stopConversation() {
	if (!_dialog)
		return;

	_dialog->abort();
	_dialog.reset();
}

} // End of namespace NWN

} // End of namespace Engines
