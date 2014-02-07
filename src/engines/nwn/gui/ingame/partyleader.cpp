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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/ingame/partyleader.cpp
 *  The NWN ingame party leader panel.
 */

#include "common/util.h"

#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "engines/nwn/gui/widgets/quadwidget.h"
#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/button.h"
#include "engines/nwn/gui/widgets/portrait.h"

#include "engines/nwn/gui/ingame/partyleader.h"

#include "engines/nwn/module.h"

namespace Engines {

namespace NWN {

static const char *kButtonTags[] = {
	"ButtonMap"      , "ButtonJournal"  , "ButtonRest"  , "ButtonOptions",
	"ButtonInventory", "ButtonCharacter", "ButtonSpells", "ButtonPlayers"
};

static const char *kButtonModels[] = {
	"pb_but_map", "pb_but_jour", "pb_but_rest" , "pb_but_opts",
	"pb_but_inv", "pb_but_char", "pb_but_spell", "pb_but_pvp"
};

static const uint32 kButtonTooltips[] = {
	7036, 7037, 8105, 7040, 7035, 7039, 7038, 8106
};

PartyLeader::PartyLeader(Module &module) : _module(&module),
	_currentHP(1), _maxHP(1) {

	// The panel

	WidgetPanel *playerPanel = new WidgetPanel(*this, "LeaderPanel", "pnl_party_bar");

	playerPanel->setPosition(floor(- playerPanel->getWidth()) + 2.0, 0.0, 0.0);

	addWidget(playerPanel);


	// Buttons

	float buttonsX = - floor(playerPanel->getWidth ()) +  5.0;
	float buttonsY = - floor(playerPanel->getHeight()) + 58.0;

	for (int i = 0; i < 8; i++) {
		WidgetButton *button = new WidgetButton(*this, kButtonTags[i], kButtonModels[i]);

		//button->setTooltip(TalkMan.getString(kButtonTooltips[i]));
		//button->setTooltipPosition(0.0, -10.0, -1.0);

		const float x = floor(buttonsX + ((i / 4) * 36.0));
		const float y = floor(buttonsY - ((i % 4) * 18.0));
		const float z = 1.0;

		button->setPosition(x, y, z);

		addWidget(button);
	}

	getWidget("ButtonPlayers", true)->setDisabled(true);


	// Portrait

	_portrait =
		new PortraitWidget(*this, "LeaderPortrait", "gui_po_nwnlogo_", Portrait::kSizeMedium);

	_portrait->setPosition(-67.0, -3.0, 1.0);
	//_portrait->setTooltipPosition(-50.0, 50.0, -1.0);

	addWidget(_portrait);


	// Health bar

	_health = new QuadWidget(*this, "LeaderHealthbar", "", 0.0, 0.0, 6.0, 100.0);

	_health->setColor(1.0, 0.0, 0.0, 1.0);
	_health->setPosition(-76.0, -3.0, 1.0);

	addWidget(_health);


	updatePortraitTooltip();
	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

PartyLeader::~PartyLeader() {
}

void PartyLeader::setPortrait(const Common::UString &portrait) {
	_currentPortrait = portrait;
	_portrait->setPortrait(portrait);
}

void PartyLeader::setName(const Common::UString &name) {
	_name = name;

	updatePortraitTooltip();
}

void PartyLeader::setArea(const Common::UString &area) {
	_area = area;

	updatePortraitTooltip();
}

void PartyLeader::setHealthColor(float r, float g, float b, float a) {
	_health->setColor(r, g, b, a);
}

void PartyLeader::setHealth(int32 current, int32 max) {
	_currentHP = current;
	_maxHP     = max;

	float barLength = 0.0;
	if (_maxHP > 0)
		barLength = CLIP(((float) current) / ((float) max), 0.0f, 1.0f) * 100.0;

	float oldHeight = _health->getHeight();
	float oldWidth  = _health->getWidth();

	float healthX, healthY, healthZ;
	_health->getPosition(healthX, healthY, healthZ);

	_health->setSize(floor(oldWidth), floor(barLength));
	_health->setPosition(floor(healthX), floor(healthY - oldHeight + barLength), healthZ);

	updatePortraitTooltip();
}

void PartyLeader::callbackActive(Widget &widget) {
	if (widget.getTag() == "ButtonOptions") {
		removeFocus();
		_module->showMenu();
		return;
	}

}

void PartyLeader::updatePortraitTooltip() {
	/*
	Common::UString tooltip =
		Common::UString::sprintf("%s %d/%d\n%s",
				_name.c_str(), _currentHP, _maxHP, _area.c_str());

	_portrait->setTooltip(tooltip);
	*/
}

void PartyLeader::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	setPosition(newWidth / 2.0, newHeight / 2.0, -400.0);
}

} // End of namespace NWN

} // End of namespace Engines
