/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/game.cpp
 *  The NWN game options menu.
 */

#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/slider.h"
#include "engines/nwn/gui/widgets/label.h"
#include "engines/nwn/gui/widgets/listbox.h"

#include "engines/nwn/gui/options/game.h"
#include "engines/nwn/gui/options/gorepass.h"
#include "engines/nwn/gui/options/feedback.h"

namespace Engines {

namespace NWN {

OptionsGameMenu::OptionsGameMenu(bool isMain) {
	load("options_game");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
		addWidget(backdrop);
	}

	std::list<Widget *> hideTiles;
	hideTiles.push_back(getWidget("NeverBox"));
	hideTiles.push_back(getWidget("AutoBox"));
	hideTiles.push_back(getWidget("AlwaysBox"));
	declareGroup(hideTiles);

	// TODO: Hide second story tiles setting
	getWidget("NeverBox" , true)->setDisabled(true);
	getWidget("AutoBox"  , true)->setDisabled(true);
	getWidget("AlwaysBox", true)->setDisabled(true);

	// TODO: Violence level
	getWidget("ViolenceSlider", true)->setDisabled(true);

	_gorepass = new OptionsGorePasswordMenu(isMain);
	_feedback = new OptionsFeedbackMenu(isMain);
}

OptionsGameMenu::~OptionsGameMenu() {
	delete _feedback;
	delete _gorepass;
}

void OptionsGameMenu::show() {
	_difficulty = CLIP(ConfigMan.getInt("difficulty", 0), 0, 2);

	getSlider("DiffSlider", true)->setState(_difficulty);

	updateDifficulty(_difficulty);

	GUI::show();
}

void OptionsGameMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "DiffEdit")
		type = kWidgetTypeListBox;
}

void OptionsGameMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "DiffSlider") {
		dynamic_cast<WidgetSlider &>(widget).setSteps(3);
		return;
	}

	if (widget.getTag() == "DiffEdit") {
		dynamic_cast<WidgetListBox &>(widget).setMode(WidgetListBox::kModeStatic);
		return;
	}
}

void OptionsGameMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {

		revertChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {

		adoptChanges();
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "PasswordButton") {
		sub(*_gorepass);
		return;
	}

	if (widget.getTag() == "FeedbackButton") {
		sub(*_feedback);
		return;
	}

	if (widget.getTag() == "DiffSlider") {
		updateDifficulty(dynamic_cast<WidgetSlider &>(widget).getState());
		return;
	}
}

void OptionsGameMenu::updateDifficulty(int difficulty) {
	WidgetLabel   &diffLabel = *getLabel("DifficultyLabel", true);
	WidgetListBox &diffDesc  = *getListBox("DiffEdit", true);

	diffDesc.setText("fnt_galahad14", TalkMan.getString(67578 + difficulty), 1.0);

	if      (difficulty == 0)
		diffLabel.setText(TalkMan.getString(66786));
	else if (difficulty == 1)
		diffLabel.setText(TalkMan.getString(66788));
	else if (difficulty == 2)
		diffLabel.setText(TalkMan.getString(66790));
	else if (difficulty == 3)
		diffLabel.setText(TalkMan.getString(66792));
}


void OptionsGameMenu::adoptChanges() {
	ConfigMan.setInt("difficulty", getSlider("DiffSlider", true)->getState(), true);
}

void OptionsGameMenu::revertChanges() {
}

} // End of namespace NWN

} // End of namespace Engines
