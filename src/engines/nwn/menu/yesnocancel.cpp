/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/yesnocancel.cpp
 *  The yes/no/cancel dialog.
 */

#include "common/util.h"

#include "engines/nwn/menu/yesnocancel.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

YesNoCancelDialog::YesNoCancelDialog(const Common::UString &msg, bool hasCancel) :
	_msg(msg), _hasCancel(hasCancel) {

	load("yesnocancelpanel");

	if (!_hasCancel)
		getWidget("CancelButton", true)->setInvisible(true);
}

YesNoCancelDialog::~YesNoCancelDialog() {
}

void YesNoCancelDialog::initWidget(Widget &widget) {
	if (widget.getTag() == "MessageLabel") {
		dynamic_cast<WidgetLabel &>(widget).setText(_msg);
	}
}

void YesNoCancelDialog::show() {
	// Center the message
	WidgetLabel &msg = *getLabel("MessageLabel", true);
	WidgetPanel &pnl = *getPanel("PNL_OK"      , true);

	float pX, pY, pZ;
	pnl.getPosition(pX, pY, pZ);

	msg.setPosition(pX - msg.getWidth() / 2.0, pY - msg.getHeight() / 2.0, pZ);

	// If we have no cancel button, move the no button to the right
	if (!_hasCancel) {
		WidgetButton &cancel = *getButton("CancelButton", true);
		WidgetButton &no     = *getButton("NoButton"    , true);

		float cX, cY, cZ;
		cancel.getPosition(cX, cY, cZ);

		no.setPosition(cX, cY, cZ);
	}

	GUI::show();
}

void YesNoCancelDialog::callbackActive(Widget &widget) {
	if (widget.getTag() == "YesButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "NoButton") {
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 3;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
