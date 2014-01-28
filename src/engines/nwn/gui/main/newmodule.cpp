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

/** @file engines/nwn/gui/main/newmodule.cpp
 *  The new module menu.
 */

#include "common/util.h"

#include "aurora/talkman.h"

#include "graphics/graphics.h"
#include "graphics/guiman.h"
#include "graphics/font.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model_nwn.h"

#include "engines/nwn/model.h"
#include "engines/nwn/module.h"
#include "engines/nwn/nwn.h"

#include "engines/nwn/gui/main/newmodule.h"

namespace Engines {

namespace NWN {

WidgetListItemModule::WidgetListItemModule(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui), _button(0), _text(0), _spacing(spacing) {

	_button = createGUIModel("ctl_btn_txt407");
	_button->setSelectable(true);
	GUIMan.addRenderable(_button);

	Common::UString splitText;
	Graphics::Aurora::FontHandle f = FontMan.get(font);
	f.getFont().split(text, splitText, _width - 8.0);

	_text = new Graphics::Aurora::Text(f, splitText, 1.0, 1.0, 1.0, 1.0);
	GUIMan.addRenderable(_text);

	_ids.push_back(_button->getID());

	updateSize();
}

WidgetListItemModule::~WidgetListItemModule() {
	GUIMan.removeRenderable(_text);
	GUIMan.removeRenderable(_button);

	destroyModel(_button);
	delete _text;
}

void WidgetListItemModule::updateSize() {
	_button->getSize(_width, _height, _depth);

	_width  = floor(_width);
	_height = floor(_height);
}

void WidgetListItemModule::setVisible(bool visible) {
	_button->setVisible(visible);
	_text->setVisible(visible);
}

void WidgetListItemModule::setPosition(float x, float y, float z) {
	float buttonWidth, buttonHeight, buttonDepth;
	_button->getSize(buttonWidth, buttonHeight, buttonDepth);

	float textWidth, textHeight, textDepth;
	_text->getSize(textWidth, textHeight, textDepth);

	NWNWidget::setPosition(x, y, z);
	getPosition(x, y, z);

	float buttonPos[3] = { x +                                    + 4.0f, y -  buttonHeight                     , z        };
	float textPos  [3] = { x + (buttonWidth  - textWidth ) / 2.0f + 4.0f, y - (buttonHeight - textHeight) / 2.0f, z + 1.0f };

	_button->setPosition(floor(buttonPos[0]), floor(buttonPos[1]), buttonPos[2]);
	_text->setPosition  (floor(textPos  [0]), floor(textPos  [1]), textPos  [2]);
}

float WidgetListItemModule::getWidth() const {
	return _width;
}

float WidgetListItemModule::getHeight() const {
	return _height + _spacing;
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

	setPosition(0.0, 0.0, -300.0);
	load("pre_loadmod");
}

NewModuleMenu::~NewModuleMenu() {
}

void NewModuleMenu::setVisible(bool visible) {
	if (visible && _modules.empty())
		initModuleList();

	GUI::setVisible(visible);
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
	GUIMan.update();
}

void NewModuleMenu::loadModule() {
	Common::UString module = getSelectedModule();
	if (_module->loadModule(module + ".mod")) {
		if (sub(*_charType, 0, false) == 2) {
			_returnCode = 2;
			return;
		}

		setVisible(true);
	}
}

} // End of namespace NWN

} // End of namespace Engines
