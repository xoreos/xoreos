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
 *  Feat popup GUI.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARFEATSPOPUP_H
#define ENGINES_NWN_GUI_CHARGEN_CHARFEATSPOPUP_H

#include <vector>

#include "src/engines/nwn/gui/chargen/charfeats.h"
#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class CharFeatsPopup : public GUI {
public:
	CharFeatsPopup(::Engines::Console *console = 0);
	~CharFeatsPopup();

	void setFeats(std::vector<FeatItem> feats);
	FeatItem getChosenFeat() const;

private:
	void callbackActive(Widget &widget);

	FeatItem              _chosenFeat;
	std::vector<FeatItem> _feats;
};


} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARFEATSPOPUP_H
