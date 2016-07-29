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
 *  The new module menu.
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

#include "src/engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

WidgetListItemModule::WidgetListItemModule(::Engines::GUI &gui,
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

WidgetListItemModule::~WidgetListItemModule() {
}

void WidgetListItemModule::show() {
	_button->show();
	_text->show();
}

void WidgetListItemModule::hide() {
	_text->hide();
	_button->hide();
}

void WidgetListItemModule::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);

	x = x + (_button->getWidth () - _text->getWidth ()) / 2.0f;
	y = y + (_button->getHeight() - _text->getHeight()) / 2.0f;

	_text->setPosition(x, y, z - 1.0f);
}

float WidgetListItemModule::getWidth() const {
	return _button->getWidth();
}

float WidgetListItemModule::getHeight() const {
	return _button->getHeight() + _spacing;
}

void WidgetListItemModule::setTag(const Common::UString &tag) {
	WidgetListItem::setTag(tag);

	_button->setTag(tag);
}

bool WidgetListItemModule::activate() {
	if (!WidgetListItem::activate())
		return false;

	_button->setState("down");

	return true;
}

bool WidgetListItemModule::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_button->setState("");

	return true;
}


NewModuleMenu::NewModuleMenu(Module &module, GUI &charType, ::Engines::Console *console) : GUI(console),
	_module(&module), _charType(&charType) {

	load("pre_loadmod");
}

NewModuleMenu::~NewModuleMenu() {
}

void NewModuleMenu::show() {
	if (_modules.empty())
		initModuleList();

	GUI::show();
}

void NewModuleMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "ModuleListBox")
		type = kWidgetTypeListBox;
	else if (tag == "ModDescEditBox")
		type = kWidgetTypeListBox;
}

void NewModuleMenu::initModuleList() {
	status("Creating module list");

	Game::getModules(_modules);

	WidgetListBox &moduleList = *getListBox("ModuleListBox", true);

	moduleList.lock();

	moduleList.clear();
	moduleList.setMode(WidgetListBox::kModeSelectable);

	moduleList.reserve(_modules.size());
	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m)
		moduleList.add(new WidgetListItemModule(*this, "fnt_galahad14", *m, 2.0f));

	moduleList.unlock();

	moduleList.select(0);
	selectedModule();
}

void NewModuleMenu::callbackActive(Widget &widget) {
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

Common::UString NewModuleMenu::getSelectedModule() {
	size_t n = getListBox("ModuleListBox", true)->getSelected();
	if (n >= _modules.size())
		return "";

	return _modules[n];
}

void NewModuleMenu::selectedModule() {
	Common::UString description = Module::getDescription(getSelectedModule() + ".mod");
	if (description.empty())
		description = TalkMan.getString(67741);

	getListBox("ModDescEditBox", true)->setText("fnt_galahad14", description, 1.0f);
}

void NewModuleMenu::loadModule() {
	Common::UString module = getSelectedModule();

	try {
		_module->load(module + ".mod");
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
