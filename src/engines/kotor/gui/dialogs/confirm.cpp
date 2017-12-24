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
 *  Confirmation dialog.
 */

#include "src/engines/kotor/gui/dialogs/confirm.h"

#include "src/engines/kotor/gui/widgets/panel.h"
#include "src/engines/kotor/gui/widgets/listbox.h"

namespace Engines {

namespace KotOR {

ConfirmDialog::ConfirmDialog(Console *console) : GUI(console), _accepted(false) {
	load("confirm");

	WidgetPanel *guiPanel = getPanel("TGuiPanel");
	guiPanel->setPosition(-guiPanel->getWidth()/2, -guiPanel->getHeight()/2, 0);

	_label = getListBox("LB_MESSAGE")->createItem("LBL_MESSAGE");
	addWidget(_label);
}

void ConfirmDialog::setText(const Common::UString &text) {
	_label->setText(text);
}

bool ConfirmDialog::getAccepted() {
	return _accepted;
}

void ConfirmDialog::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_OK") {
		_accepted = true;
		_returnCode = kReturnCodeAbort;
		return;
	}
	if (widget.getTag() == "BTN_CANCEL") {
		_accepted = false;
		_returnCode = kReturnCodeAbort;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
