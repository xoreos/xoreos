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
 *  Dialog GUI for Star Wars: Knights of the Old Republic II - The Sith Lords.
 */

#include "src/common/configman.h"

#include "src/graphics/windowman.h"

#include "src/engines/kotor2/gui/dialog.h"

namespace Engines {

namespace KotOR2 {

DialogGUI::DialogGUI(KotORBase::Module &module) : KotORBase::DialogGUI(module) {
	load("dialog_p");
	update(WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

void DialogGUI::getTextColor(float &r, float &g, float &b) const {
	r = 0.101961f;
	g = 0.698039f;
	b = 0.549020f;
}

void DialogGUI::preprocessEntry(Common::UString &text) {
	if (ConfigMan.getBool("showdevnotes", false))
		return;

	while (true) {
		Common::UString::iterator obit = text.findFirst('{');
		if (obit == text.end())
			return;

		Common::UString::iterator cbit = text.findFirst('}');
		if (cbit == text.end())
			return;

		text.erase(obit, ++cbit);
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
