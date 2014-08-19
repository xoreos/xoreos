/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** @file engines/nwn/gui/ingame/ingame.cpp
 *  The NWN ingame GUI elements.
 */

#include "common/error.h"

#include "engines/nwn/creature.h"

#include "engines/nwn/gui/ingame/ingame.h"
#include "engines/nwn/gui/ingame/main.h"
#include "engines/nwn/gui/ingame/quickbar.h"
#include "engines/nwn/gui/ingame/quickchat.h"
#include "engines/nwn/gui/ingame/compass.h"
#include "engines/nwn/gui/ingame/partyleader.h"
#include "engines/nwn/gui/ingame/dialog.h"

namespace Engines {

namespace NWN {

IngameGUI::IngameGUI(Module &module) : _module(&module), _lastCompassChange(0) {
	_main = new IngameMainMenu;

	_quickbar  = new Quickbar;
	_quickchat = new Quickchat(_quickbar->getHeight() - 3.0);
	_compass   = new Compass(_quickbar->getHeight() + _quickchat->getHeight() - 6.0);

	_party.resize(1);
	_party[0] = new PartyLeader(module);

	_lastPartyMemberChange.resize(1);
	_lastPartyMemberChange[0] = 0;

	_dialog = 0;
}

IngameGUI::~IngameGUI() {
	hide();

	delete _dialog;

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		delete *p;

	delete _compass;
	delete _quickchat;
	delete _quickbar;

	delete _main;
}

int IngameGUI::showMain() {
	_main->show();
	int code = _main->run();
	_main->hide();

	return code;
}

void IngameGUI::show() {
	_quickbar->show();
	_quickchat->show();
	_compass->show();

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->show();

	if (_dialog)
		_dialog->show();
}

void IngameGUI::hide() {
	if (_dialog)
		_dialog->hide();

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
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

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
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

	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->processEventQueue();

	_compass->processEventQueue();
	_quickchat->processEventQueue();
	_quickbar->processEventQueue();
}

void IngameGUI::setPortrait(uint partyMember, const Common::UString &portrait) {
	_party[partyMember]->setPortrait(portrait);
}

void IngameGUI::setHealth(uint partyMember, uint32 current, uint32 max) {
	_party[partyMember]->setHealth(current, max);
}

void IngameGUI::setName(uint partyMember, const Common::UString &name) {
	_party[partyMember]->setName(name);
}

void IngameGUI::setArea(const Common::UString &area) {
	for (std::vector<CharacterInfo *>::iterator p = _party.begin(); p != _party.end(); ++p)
		(*p)->setArea(area);
}

void IngameGUI::setHealthy(uint partyMember) {
	_party[partyMember]->setHealthColor(1.0, 0.0, 0.0, 1.0);
}

void IngameGUI::setSick(uint partyMember) {
	_party[partyMember]->setHealthColor(189.0 / 255.0, 146.0 / 255.0,  74.0 / 255.0, 1.0);
}

void IngameGUI::setPoisoned(uint partyMember) {
	_party[partyMember]->setHealthColor(132.0 / 255.0, 182.0 / 255.0,  74.0 / 255.0, 1.0);
}

void IngameGUI::updatePartyMember(uint partyMember, const Creature &creature, bool force) {
	assert(partyMember < _party.size());

	uint32 lastPartyMemberChange = creature.lastChangedGUIDisplay();
	if (!force && (lastPartyMemberChange <= _lastPartyMemberChange[partyMember]))
		return;

	setPortrait(partyMember, creature.getPortrait());
	setName    (partyMember, creature.getName());
	setHealth  (partyMember, creature.getCurrentHP(), creature.getMaxHP());

	_lastPartyMemberChange[partyMember] = lastPartyMemberChange;
}

bool IngameGUI::hasRunningConversation() const {
	return _dialog != 0;
}

bool IngameGUI::startConversation(const Common::UString &conv,
                                  Creature &pc, Object &obj, bool playHello) {
	stopConversation();

	if (conv.empty())
		return true;

	try {
		_dialog = new Dialog(conv, pc, obj, *_module, playHello);

		_dialog->show();
	} catch (Common::Exception &e) {
		delete _dialog;
		_dialog = 0;

		e.add("Failed starting conversation \"%s\"", conv.c_str());
		Common::printException(e, "WARNING: ");
		return false;
	}

	return true;
}

void IngameGUI::stopConversation() {
	if (!_dialog)
		return;

	_dialog->abort();

	delete _dialog;

	_dialog = 0;
}

} // End of namespace NWN

} // End of namespace Engines
