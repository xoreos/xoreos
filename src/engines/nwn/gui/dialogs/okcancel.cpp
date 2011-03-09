/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/dialogs/okcancel.cpp
 *  The okay/cancel dialog.
 */

#include "engines/aurora/gui.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/dialogs/okcancel.h"

namespace Engines {

namespace NWN {

OKCancelDialog::OKCancelDialog(const Common::UString &msg,
		const Common::UString &ok, const Common::UString &cancel) :
	_msg(msg), _ok(ok), _cancel(cancel) {

	load("okcancelpanel");
}

OKCancelDialog::~OKCancelDialog() {
}

void OKCancelDialog::initWidget(Widget &widget) {
	if (widget.getTag() == "MessageLabel") {
		dynamic_cast<WidgetLabel &>(widget).setText(_msg);
		return;
	}

	if (widget.getTag() == "OkButton#Caption") {
		if (!_ok.empty())
			dynamic_cast<WidgetLabel &>(widget).setText(_ok);
		return;
	}

	if (widget.getTag() == "CancelButton#Caption") {
		if (!_cancel.empty())
			dynamic_cast<WidgetLabel &>(widget).setText(_cancel);
		return;
	}
}

void OKCancelDialog::show() {
	// Center the message
	WidgetLabel &msg = *getLabel("MessageLabel", true);
	WidgetPanel &pnl = *getPanel("PNL_OK"      , true);

	float pX, pY, pZ;
	pnl.getPosition(pX, pY, pZ);

	msg.setPosition(pX - msg.getWidth() / 2.0, pY - msg.getHeight() / 2.0, pZ);

	GUI::show();
}

void OKCancelDialog::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 2;
		return;
	}

}

} // End of namespace NWN

} // End of namespace Engines
