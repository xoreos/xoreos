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
 *  The yes/no/cancel dialog.
 */

#include "src/engines/aurora/gui.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/label.h"

#include "src/engines/nwn/gui/dialogs/yesnocancel.h"

namespace Engines {

namespace NWN {

YesNoCancelDialog::YesNoCancelDialog(const Common::UString &msg, const Common::UString &yes,
			const Common::UString &no, const Common::UString &cancel,
			::Engines::Console *console) : GUI(console),
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

	msg.setPosition(pX - msg.getWidth() / 2.0f, pY - msg.getHeight() / 2.0f, pZ);

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
