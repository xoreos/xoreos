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

/** @file engines/nwn/gui/main/newmodule.cpp
 *  The new module menu.
 */

#include "common/util.h"

#include "aurora/talkman.h"

#include "graphics/font.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/model.h"

#include "engines/nwn/module.h"
#include "engines/nwn/nwn.h"

#include "engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

WidgetListItemModule::WidgetListItemModule(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button = loadModelGUI("ctl_btn_txt407");
	assert(_button);

	_button->setClickable(true);

	Common::UString splitText;
	Graphics::Aurora::FontHandle f = FontMan.get(font);
	f.getFont().split(text, splitText, _button->getWidth() - 8.0);

	_text = new Graphics::Aurora::Text(f, splitText, 1.0, 1.0, 1.0, 1.0, 0.5);
}

WidgetListItemModule::~WidgetListItemModule() {
	delete _button;
	delete _text;
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

	x = x + (_button->getWidth () - _text->getWidth ()) / 2.0;
	y = y + (_button->getHeight() - _text->getHeight()) / 2.0;

	_text->setPosition(x, y, z - 1.0);
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


NewModuleMenu::NewModuleMenu(Module &module, GUI &charType) :
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

	NWNEngine::getModules(_modules);

	WidgetListBox &moduleList = *getListBox("ModuleListBox", true);

	moduleList.lock();

	moduleList.clear();
	moduleList.setMode(WidgetListBox::kModeSelectable);

	moduleList.reserve(_modules.size());
	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m)
		moduleList.add(new WidgetListItemModule(*this, "fnt_galahad14", *m, 2.0));

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
	uint n = getListBox("ModuleListBox", true)->getSelected();
	if (n >= _modules.size())
		return "";

	return _modules[n];
}

void NewModuleMenu::selectedModule() {
	Common::UString description = Module::getDescription(getSelectedModule());
	if (description.empty())
		description = TalkMan.getString(67741);

	getListBox("ModDescEditBox", true)->setText("fnt_galahad14", description, 1.0);
}

void NewModuleMenu::loadModule() {
	Common::UString module = getSelectedModule();
	if (_module->loadModule(module + ".mod")) {
		if (sub(*_charType, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		show();
	}
}

} // End of namespace NWN

} // End of namespace Engines
