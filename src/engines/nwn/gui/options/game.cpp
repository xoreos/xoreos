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
 *  The NWN game options menu.
 */

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/slider.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/listbox.h"

#include "src/engines/nwn/gui/options/game.h"
#include "src/engines/nwn/gui/options/gorepass.h"
#include "src/engines/nwn/gui/options/feedback.h"

namespace Engines {

namespace NWN {

OptionsGameMenu::OptionsGameMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_game");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
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

	_gorepass.reset(new OptionsGorePasswordMenu(isMain, _console));
	_feedback.reset(new OptionsFeedbackMenu    (isMain, _console));
}

OptionsGameMenu::~OptionsGameMenu() {
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

	diffDesc.setText("fnt_galahad14", TalkMan.getString(67578 + difficulty), 1.0f);

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
