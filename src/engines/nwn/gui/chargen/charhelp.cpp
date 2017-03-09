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
 *  Help popup GUI.
 */

#include "src/engines/nwn/gui/widgets/portrait.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/chargen/charfeats.h"
#include "src/engines/nwn/gui/chargen/charhelp.h"

namespace Engines {

namespace NWN {

CharHelp::CharHelp(const Common::UString &GUIResRef, ::Engines::Console *console) : GUI(console) {
	load(GUIResRef);

	setPosition(0, 0, -220);

	_icon.reset(new PortraitWidget(*this, "FeatIcon", "", Portrait::kSizeIcon));

	float pX, pY, pZ;
	getEditBox("EditBox", true)->getPosition(pX, pY, pZ);
	_icon->setPosition(-200, 145, pZ - 10);

	getEditBox("EditBox", true)->addChild(*_icon);

	// Remove unnecessary button.
	if (hasWidget("OkButton")) {
		getButton("OkButton", true)->remove();
	}
}

CharHelp::~CharHelp() {
}

void CharHelp::setContent(const Common::UString title, const Common::UString desc, const Common::UString icon) {
	getLabel("Title", true)->setText(title);
	getEditBox("EditBox", true)->setText("fnt_galahad14", desc);
	_icon->setPortrait(icon);
}

void CharHelp::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton" || widget.getTag() == "CloseButton")
		_returnCode = 1;
}

} // End of namespace NWN

} // End of namespace Engines
