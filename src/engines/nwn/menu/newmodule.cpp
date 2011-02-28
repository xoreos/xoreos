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

#include "common/configman.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/erffile.h"
#include "aurora/locstring.h"
#include "aurora/talkman.h"

#include "engines/nwn/menu/newmodule.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

NewModuleMenu::NewModuleMenu() {
	load("pre_loadmod");
}

NewModuleMenu::~NewModuleMenu() {
}

void NewModuleMenu::show() {
	if (_modules.empty())
		initModuleList();

	GUI::show();
}

void NewModuleMenu::initModuleList() {
	WidgetEditBox &moduleList = *getEditBox("ModuleListBox", true);

	moduleList.clear();
	moduleList.setMode(WidgetEditBox::kModeSelectable);

	Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
	if (moduleDir.empty())
		return;

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
			moduleList.addLine(_modules.back());
		}
	}

	moduleList.selectLine(0);
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

		if (dynamic_cast<WidgetEditBox &>(widget).wasDblClicked()) {
			loadModule();
			_returnCode = 3;
		}

		return;
	}
}

Common::UString NewModuleMenu::getSelectedModule() {
	uint n = getEditBox("ModuleListBox", true)->getSelectedLineNumber();
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

	getEditBox("ModDescEditBox", true)->set(description);
}

void NewModuleMenu::loadModule() {
	Common::UString module = getSelectedModule();
	if (module.empty())
		return;

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_moduleToLoad", module + ".mod");
}

} // End of namespace NWN

} // End of namespace Engines
