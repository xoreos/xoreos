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
 *  The new premium module menu.
 */

#include <cassert>

#include "src/common/util.h"

#include "src/aurora/talkman.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/widgets/label.h"

#include "src/engines/nwn/gui/main/newpremium.h"

namespace Engines {

namespace NWN {

WidgetListItemPremium::WidgetListItemPremium(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button.reset(loadModelGUI("ctl_btn_txt407"));
	assert(_button);

	_button->setClickable(true);

	Common::UString splitText;
	Graphics::Aurora::FontHandle f = FontMan.get(font);
	f.getFont().split(text, splitText, _button->getWidth() - 8.0f);

	_text.reset(new Graphics::Aurora::Text(f, splitText, 1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter));
}

WidgetListItemPremium::~WidgetListItemPremium() {
}

void WidgetListItemPremium::show() {
	_button->show();
	_text->show();
}

void WidgetListItemPremium::hide() {
	_text->hide();
	_button->hide();
}

void WidgetListItemPremium::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);

	x = x + (_button->getWidth () - _text->getWidth ()) / 2.0f;
	y = y + (_button->getHeight() - _text->getHeight()) / 2.0f;

	_text->setPosition(x, y, z - 1.0f);
}

float WidgetListItemPremium::getWidth() const {
	return _button->getWidth();
}

float WidgetListItemPremium::getHeight() const {
	return _button->getHeight() + _spacing;
}

void WidgetListItemPremium::setTag(const Common::UString &tag) {
	WidgetListItem::setTag(tag);

	_button->setTag(tag);
}

bool WidgetListItemPremium::activate() {
	if (!WidgetListItem::activate())
		return false;

	_button->setState("down");

	return true;
}

bool WidgetListItemPremium::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_button->setState("");

	return true;
}


NewPremiumMenu::NewPremiumMenu(Module &module, GUI &charType, ::Engines::Console *console) : GUI(console),
	_module(&module), _charType(&charType) {

	load("pre_loadmod");
}

NewPremiumMenu::~NewPremiumMenu() {
}

void NewPremiumMenu::show() {
	if (_modules.empty())
		initModuleList();

	GUI::show();
}

void NewPremiumMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	     if (tag == "ModuleListBox")
		type = kWidgetTypeListBox;
	else if (tag == "ModDescEditBox")
		type = kWidgetTypeListBox;
}

void NewPremiumMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "TitleBar") {
		dynamic_cast<WidgetLabel &>(widget).setText(TalkMan.getString(110806));
		return;
	}
}

void NewPremiumMenu::initModuleList() {
	status("Creating module list");

	Game::getPremiumModules(_modules);

	WidgetListBox &moduleList = *getListBox("ModuleListBox", true);

	moduleList.lock();

	moduleList.clear();
	moduleList.setMode(WidgetListBox::kModeSelectable);

	moduleList.reserve(_modules.size());
	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m)
		moduleList.add(new WidgetListItemPremium(*this, "fnt_galahad14", *m, 2.0f));

	moduleList.unlock();

	moduleList.select(0);
	selectedModule();
}

void NewPremiumMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "LoadButton") {
		loadModule();
		return;
	}

	if (widget.getTag() == "ModuleListBox") {
		selectedModule();

		if (dynamic_cast<WidgetListBox &>(widget).wasDblClicked())
			loadModule();

		return;
	}
}

Common::UString NewPremiumMenu::getSelectedModule() {
	size_t n = getListBox("ModuleListBox", true)->getSelected();
	if (n >= _modules.size())
		return "";

	return _modules[n];
}

void NewPremiumMenu::selectedModule() {
	Common::UString description = Module::getDescription(getSelectedModule() + ".nwm");
	if (description.empty())
		description = TalkMan.getString(67741);

	getListBox("ModDescEditBox", true)->setText("fnt_galahad14", description, 1.0f);
}

void NewPremiumMenu::loadModule() {
	Common::UString module = getSelectedModule();

	try {
		_module->load(module + ".nwm");
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}

	if (sub(*_charType, 0, false) == 2) {
		_returnCode = 2;
		return;
	}

	show();
}

} // End of namespace NWN

} // End of namespace Engines
