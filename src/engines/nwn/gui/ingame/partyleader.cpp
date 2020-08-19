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
 *  The NWN ingame party leader panel.
 */

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/graphics/graphics.h"

#include "src/engines/nwn/gui/widgets/quadwidget.h"
#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/portrait.h"

#include "src/engines/nwn/gui/ingame/partyleader.h"

#include "src/engines/nwn/module.h"

namespace Engines {

namespace NWN {

static const char * const kButtonTags[] = {
	"ButtonMap"      , "ButtonJournal"  , "ButtonRest"  , "ButtonOptions",
	"ButtonInventory", "ButtonCharacter", "ButtonSpells", "ButtonPlayers"
};

static const char * const kButtonModels[] = {
	"pb_but_map", "pb_but_jour", "pb_but_rest" , "pb_but_opts",
	"pb_but_inv", "pb_but_char", "pb_but_spell", "pb_but_pvp"
};

static const uint32_t kButtonTooltips[] = {
	7036, 7037, 8105, 7040, 7035, 7039, 7038, 8106
};

PartyLeader::PartyLeader(Module &module) : _module(&module),
	_currentHP(1), _maxHP(1) {

	// The panel

	WidgetPanel *playerPanel = new WidgetPanel(*this, "LeaderPanel", "pnl_party_bar");

	playerPanel->setPosition(- playerPanel->getWidth(), 0.0f, 0.0f);

	addWidget(playerPanel);


	// Buttons

	float buttonsX = - playerPanel->getWidth () +  4.0f;
	float buttonsY = - playerPanel->getHeight() + 57.0f;

	for (int i = 0; i < 8; i++) {
		WidgetButton *button = new WidgetButton(*this, kButtonTags[i], kButtonModels[i]);

		button->setTooltip(TalkMan.getString(kButtonTooltips[i]));
		button->setTooltipPosition(0.0f, -10.0f, -1.0f);

		const float x = buttonsX + ((i / 4) * 36.0f);
		const float y = buttonsY - ((i % 4) * 18.0f);
		const float z = -100.0f;

		button->setPosition(x, y, z);

		addWidget(button);
	}

	getWidget("ButtonPlayers", true)->setDisabled(true);


	// Portrait

	_portrait =
		new PortraitWidget(*this, "LeaderPortrait", "gui_po_nwnlogo_", Portrait::kSizeMedium);

	_portrait->setPosition(-67.0f, -103.0f, -100.0f);
	_portrait->setTooltipPosition(-50.0f, 50.0f, -1.0f);

	addWidget(_portrait);


	// Health bar

	_health = new QuadWidget(*this, "LeaderHealthbar", "", 0.0f, 0.0f, 6.0f, 100.0f);

	_health->setColor(1.0f, 0.0f, 0.0f, 1.0f);
	_health->setPosition(-76.0f, -103.0f, -100.0f);

	addWidget(_health);


	updatePortraitTooltip();
	notifyResized(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
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

void PartyLeader::setHealth(int32_t current, int32_t max) {
	_currentHP = current;
	_maxHP     = max;

	float barLength = 0.0f;
	if (_maxHP > 0)
		barLength = CLIP(((float) current) / ((float) max), 0.0f, 1.0f) * 100.0f;

	_health->setHeight(barLength);

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
	Common::UString tooltip =
		Common::UString::format("%s %d/%d\n%s",
				_name.c_str(), _currentHP, _maxHP, _area.c_str());

	_portrait->setTooltip(tooltip);
}

void PartyLeader::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                                int newWidth, int newHeight) {

	setPosition(newWidth / 2.0f, newHeight / 2.0f, -10.0f);
}

} // End of namespace NWN

} // End of namespace Engines
