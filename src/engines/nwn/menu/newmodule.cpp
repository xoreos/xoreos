/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newmodule.cpp
 *  The new module menu.
 */

#include "common/util.h"
#include "common/configman.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/erffile.h"
#include "aurora/locstring.h"
#include "aurora/talkman.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/nwn/types.h"

#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

namespace Engines {

namespace NWN {

WidgetListItemModule::WidgetListItemModule(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui), _spacing(spacing) {

	_button = loadModelGUI("ctl_btn_txt407");

	Common::UString splitText;
	Graphics::Aurora::FontHandle f = FontMan.get(font);
	f.getFont().split(text, splitText, _button->getWidth() - 8.0);

	_text = new Graphics::Aurora::Text(f, splitText, 0.5);

	assert(_button);
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
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);

	x = x + (_button->getWidth () - _text->getWidth ()) / 2.0;
	y = y + (_button->getHeight() - _text->getHeight()) / 2.0;

	_text->setPosition(x, y, -z);
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


NewModuleMenu::NewModuleMenu(ModuleContext &moduleContext) : _moduleContext(&moduleContext) {
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

	Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
	if (moduleDir.empty())
		return;

	WidgetListBox &moduleList = *getListBox("ModuleListBox", true);

	moduleList.lock();

	moduleList.clear();
	moduleList.setMode(WidgetListBox::kModeSelectable);

	Common::FileList moduleDirList;

	moduleDirList.addDirectory(moduleDir);

	std::list<Common::UString> modules;
	uint n = moduleDirList.getFileNames(modules);

	modules.sort(Common::UString::iless());

	_modules.reserve(n);
	moduleList.reserve(n);
	for (std::list<Common::UString>::const_iterator m = modules.begin(); m != modules.end(); ++m) {
		if (Common::FilePath::getExtension(*m).equalsIgnoreCase(".mod")) {

			_modules.push_back(Common::FilePath::getStem(*m));

			WidgetListItemModule *M =
				new WidgetListItemModule(*this, "fnt_galahad14", _modules.back(), 2.0);
			moduleList.add(M);
		}
	}

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
		_returnCode = 3;
		return;
	}

	if (widget.getTag() == "ModuleListBox") {
		selectedModule();

		if (dynamic_cast<WidgetListBox &>(widget).wasDblClicked()) {
			loadModule();
			_returnCode = 3;
		}

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
	Common::UString description;

	try {
		Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
		Common::UString modFile   = getSelectedModule();

		Aurora::ERFFile mod(moduleDir + "/" + modFile + ".mod", true);

		Aurora::LocString locString = mod.getDescription();

		if (locString.hasString(TalkMan.getMainLanguage()))
			description = locString.getString(TalkMan.getMainLanguage());
		else
			description = locString.getFirstString();
	} catch (...) {
		description.clear();
	}

	if (description.empty())
		description = TalkMan.getString(67741);

	getListBox("ModDescEditBox", true)->setText("fnt_galahad14", description, 1.0);
}

void NewModuleMenu::loadModule() {
	Common::UString module = getSelectedModule();
	if (module.empty())
		return;

	_moduleContext->module = module + ".mod";
}

} // End of namespace NWN

} // End of namespace Engines
