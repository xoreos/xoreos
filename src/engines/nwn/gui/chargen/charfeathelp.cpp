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
 *  Feat help GUI.
 */

#include "src/engines/nwn/gui/widgets/portrait.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/chargen/charfeats.h"
#include "src/engines/nwn/gui/chargen/charfeathelp.h"

namespace Engines {

namespace NWN {

CharFeatHelp::CharFeatHelp(::Engines::Console *console) : GUI(console) {
	load("cg_featinfo");

	setPosition(0, 0, -120);

	_icon = new PortraitWidget(*this, "FeatIcon", "", Portrait::kSizeIcon);

	float pX, pY, pZ;
	getEditBox("EditBox", true)->getPosition(pX, pY, pZ);
	_icon->setPosition(-200, 145, pZ - 10);

	getEditBox("EditBox", true)->addChild(*_icon);
}

CharFeatHelp::~CharFeatHelp() {
}

void CharFeatHelp::setFeat(Feat &feat) {
	getLabel("Title", true)->setText(feat.name);
	getEditBox("EditBox", true)->setText("fnt_galahad14", feat.description);
	_icon->setPortrait(feat.icon);
}

void CharFeatHelp::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton" || widget.getTag() == "CloseButton")
		_returnCode = 1;
}

} // End of namespace NWN

} // End of namespace Engines
