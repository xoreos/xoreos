/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/dialogs/yesnocancel.cpp
 *  The yes/no/cancel dialog.
 */

#include "engines/aurora/gui.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/dialogs/yesnocancel.h"

namespace Engines {

namespace NWN {

YesNoCancelDialog::YesNoCancelDialog(const Common::UString &msg, const Common::UString &yes,
			const Common::UString &no, const Common::UString &cancel) :
	_msg(msg), _yes(yes), _no(no), _cancel(cancel) {

	load("yesnocancelpanel");
}

YesNoCancelDialog::~YesNoCancelDialog() {
}

void YesNoCancelDialog::initWidget(Widget &widget) {
	if (widget.getTag() == "MessageLabel") {
		dynamic_cast<WidgetLabel &>(widget).setText(_msg);
	}

	if (widget.getTag() == "YesButton") {
		if (!_yes.empty())
			dynamic_cast<WidgetLabel &>(widget).setText(_yes);
		return;
	}

	if (widget.getTag() == "NoButton") {
		if (!_no.empty())
			dynamic_cast<WidgetLabel &>(widget).setText(_no);
		return;
	}

	if (widget.getTag() == "CancelButton") {
		if (!_cancel.empty())
			dynamic_cast<WidgetLabel &>(widget).setText(_cancel);
		return;
	}
}

void YesNoCancelDialog::show() {
	// Center the message
	WidgetLabel &msg = *getLabel("MessageLabel", true);
	WidgetPanel &pnl = *getPanel("PNL_OK"      , true);

	float pX, pY, pZ;
	pnl.getPosition(pX, pY, pZ);

	msg.setPosition(pX - msg.getWidth() / 2.0, pY - msg.getHeight() / 2.0, pZ);

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
