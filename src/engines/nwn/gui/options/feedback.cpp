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
 *  The NWN feedback options menu.
 */

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/slider.h"
#include "src/engines/nwn/gui/widgets/checkbox.h"

#include "src/engines/nwn/gui/options/feedback.h"

const int kStringSec = 2367;

namespace Engines {

namespace NWN {

OptionsFeedbackMenu::OptionsFeedbackMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_feedback");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}

	std::list<Widget *> bubbleMode;
	bubbleMode.push_back(getWidget("BubblesTextOnly"));
	bubbleMode.push_back(getWidget("BubblesFull"));
	bubbleMode.push_back(getWidget("BubblesOff"));
	declareGroup(bubbleMode);

	std::list<Widget *> targetingFeedback;
	targetingFeedback.push_back(getWidget("FeedbackNever"));
	targetingFeedback.push_back(getWidget("FeedbackPause"));
	targetingFeedback.push_back(getWidget("FeedbackAlways"));
	declareGroup(targetingFeedback);

	// TODO: Targeting feedback
	getWidget("FeedbackNever", true)->setDisabled(true);
	getWidget("FeedbackPause", true)->setDisabled(true);
	getWidget("FeedbackAlways", true)->setDisabled(true);

	// TODO: Floaty text feedback
	getWidget("FloatyText", true)->setDisabled(true);
}

OptionsFeedbackMenu::~OptionsFeedbackMenu() {
}

void OptionsFeedbackMenu::show() {
	uint32_t tooltipDelay = (CLIP(ConfigMan.getInt("tooltipdelay", 100), 100, 2700) / 100) - 1;

	getSlider("TooltipSlider", true)->setState(tooltipDelay);
	updateTooltipDelay(tooltipDelay);

	getCheckBox("MouseoverBox", true)->setState(ConfigMan.getBool("mouseoverfeedback"));

	uint32_t feedbackMode = CLIP(ConfigMan.getInt("feedbackmode", 2), 0, 2);
	if      (feedbackMode == 0)
		getCheckBox("BubblesOff", true)->setState(true);
	else if (feedbackMode == 1)
		getCheckBox("BubblesTextOnly", true)->setState(true);
	else if (feedbackMode == 2)
		getCheckBox("BubblesFull", true)->setState(true);

	GUI::show();
}

void OptionsFeedbackMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "TooltipSlider") {
		dynamic_cast<WidgetSlider &>(widget).setSteps(26);
		return;
	}
}

void OptionsFeedbackMenu::callbackActive(Widget &widget) {
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

	if (widget.getTag() == "TooltipSlider") {
		updateTooltipDelay(dynamic_cast<WidgetSlider &>(widget).getState());
		return;
	}
}

void OptionsFeedbackMenu::updateTooltipDelay(uint32_t UNUSED(tooltipDelay)) {
	WidgetLabel  &ttDelayLabel  = *getLabel ("ToolTipValue" , true);
	WidgetSlider &ttDelaySlider = *getSlider("TooltipSlider", true);

	const float ttDelay = ((float) (ttDelaySlider.getState() + 1)) / 10.0f;

	const Common::UString secString   = TalkMan.getString(kStringSec);
	const Common::UString ttDelayText =
		Common::UString::format("%3.1f %s", ttDelay, secString.c_str());

	ttDelayLabel.setText(ttDelayText);
}

void OptionsFeedbackMenu::adoptChanges() {
	uint32_t tooltipDelay = (getSlider("TooltipSlider", true)->getState() + 1) * 100;
	ConfigMan.setInt("tooltipdelay", tooltipDelay, true);

	bool mouseoverFeedback = getCheckBox("MouseoverBox", true)->getState();
	ConfigMan.setBool("mouseoverfeedback", mouseoverFeedback, true);

	uint32_t feedbackMode = 2;
	if      (getCheckBox("BubblesOff", true)->getState())
		feedbackMode = 0;
	else if (getCheckBox("BubblesTextOnly", true)->getState())
		feedbackMode = 1;
	else if (getCheckBox("BubblesFull", true)->getState())
		feedbackMode = 2;

	ConfigMan.setInt("feedbackmode", feedbackMode, true);
}

void OptionsFeedbackMenu::revertChanges() {
}

} // End of namespace NWN

} // End of namespace Engines
