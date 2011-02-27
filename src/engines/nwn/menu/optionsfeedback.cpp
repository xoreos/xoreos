/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsfeedback.cpp
 *  The feedback options menu.
 */

#include "engines/nwn/menu/optionsfeedback.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsFeedbackMenu::OptionsFeedbackMenu(bool isMain) {
	load("options_feedback");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel("PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
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

	// TODO: Tooltip delay
	getWidget("TooltipSlider", true)->setDisabled(true);
}

OptionsFeedbackMenu::~OptionsFeedbackMenu() {
}

void OptionsFeedbackMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {
		_returnCode = 2;
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
