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
 *  The character appearance GUI in CharGen.
 */

#include "src/graphics/aurora/modelnode.h"

#include "src/engines/nwn/gui/widgets/button.h"

#include "src/engines/nwn/gui/chargen/charinfovoice.h"
#include "src/engines/nwn/gui/chargen/charappearance.h"

namespace Engines {

namespace NWN {

CharAppearance::CharAppearance(CharGenChoices &choices, ::Engines::Console *console) :
	CharGenBase(console) {

	_choices = &choices;
	load("cg_appearance");

	// TODO: Show character.
	adjustButtons("ClothingButton");
	adjustButtons("SkinButton");
	adjustButtons("TattooButton");
	adjustButtons("TattooColButton");
	adjustButtons("HairButton");
	adjustButtons("BodyButton");
	adjustButtons("HeadButton");
}

CharAppearance::~CharAppearance() {
}

void CharAppearance::reset() {
}

void CharAppearance::callbackActive(Widget& widget) {
	if (widget.getTag() == "OkButton") {
		if (!_charInfo)
			_charInfo.reset(new CharInfoVoice(*_choices, _console));

		if (sub(*_charInfo, 0, false) == 1) {
			reset();
			_returnCode = 1;
			return;
		}
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}
}

void CharAppearance::adjustButtons(const Common::UString &tag) {
	float wX, wY, wZ;
	float cX, cY, cZ;
	getWidget(tag, true)->getPosition(wX,wY,wZ);
	getButton(tag, true)->getNode("text")->getPosition(cX,cY,cZ);
	getWidget(tag, true)->setPosition(wX + cX, wY + cY - 7.f, wZ - cZ);
}

} // End of namespace NWN

} // End of namespace Engines
