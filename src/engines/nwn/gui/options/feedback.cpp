/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/feedback.cpp
 *  The NWN feedback options menu.
 */

#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"
#include "engines/nwn/gui/widgets/slider.h"

#include "engines/nwn/gui/options/feedback.h"

const int kStringSec = 2367;

namespace Engines {

namespace NWN {

OptionsFeedbackMenu::OptionsFeedbackMenu(bool isMain) {
	load("options_feedback");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, 100.0);
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

	// TODO: Show mouse-over feedback
	getWidget("MouseoverBox", true)->setDisabled(true);

	// TODO: Text buble mode
	getWidget("BubblesTextOnly", true)->setDisabled(true);
	getWidget("BubblesFull", true)->setDisabled(true);
	getWidget("BubblesOff", true)->setDisabled(true);

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
	uint32 tooltipDelay = (CLIP(ConfigMan.getInt("tooltipdelay", 100), 100, 2700) / 100) - 1;

	getSlider("TooltipSlider", true)->setState(tooltipDelay);
	updateTooltipDelay(tooltipDelay);

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

void OptionsFeedbackMenu::updateTooltipDelay(uint32 tooltipDelay) {
	WidgetLabel  &ttDelayLabel  = *getLabel ("ToolTipValue" , true);
	WidgetSlider &ttDelaySlider = *getSlider("TooltipSlider", true);

	const float ttDelay = ((float) (ttDelaySlider.getState() + 1)) / 10.0;

	const Common::UString secString   = TalkMan.getString(kStringSec);
	const Common::UString ttDelayText =
		Common::UString::sprintf("%3.1f %s", ttDelay, secString.c_str());

	ttDelayLabel.setText(ttDelayText);
}

void OptionsFeedbackMenu::adoptChanges() {
	uint32 tooltipDelay = (getSlider("TooltipSlider", true)->getState() + 1) * 100;
	ConfigMan.setInt("tooltipdelay", tooltipDelay, true);
}

void OptionsFeedbackMenu::revertChanges() {
}

} // End of namespace NWN

} // End of namespace Engines
