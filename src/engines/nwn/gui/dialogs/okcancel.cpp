/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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

	setPosition(0.0, 0.0, -250.0);
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

void OKCancelDialog::setVisible(bool visible) {
	if (visible) {
		// Center the message
		WidgetLabel &msg = *getLabel("MessageLabel", true);
		WidgetPanel &pnl = *getPanel("PNL_OK"      , true);

		float pX, pY, pZ;
		pnl.getPosition(pX, pY, pZ);

		msg.setPosition(floor(pX - msg.getWidth() / 2.0f), floor(pY + msg.getHeight() / 2.0f), pZ + 1.0f);
	}

	GUI::setVisible(visible);
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
