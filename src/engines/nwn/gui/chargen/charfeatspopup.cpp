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

#include "src/common/util.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/chargen/charfeatspopup.h"

namespace Engines {

namespace NWN {

CharFeatsPopup::CharFeatsPopup(::Engines::Console *console) : GUI(console) {
	load("cg_feats_popup");

	setPosition(0, 0, -120);

	_chosenFeat.featId = 0xFFFFFFFF;

	getButton("OkButton", true)->setDisabled(true);
}

CharFeatsPopup::~CharFeatsPopup() {
}

void CharFeatsPopup::setFeats(std::vector<Feat> feats) {
	_feats = feats;
	warning("adding to popup.");
	for (std::vector<Feat>::iterator f = _feats.begin(); f != _feats.end(); ++f)
		warning("feat %s, id %u", (*f).name.c_str(), (*f).featId);

	Engines::NWN::WidgetListBox *featBox = getListBox("FeatsList", true);
	featBox->lock();
	featBox->clear();
	getListBox("FeatsList", true)->setMode(WidgetListBox::kModeSelectable);
	for (std::vector<Feat>::iterator f = feats.begin(); f != feats.end(); ++f) {
		WidgetListItemTextLine *item = new WidgetListItemTextLine(*this, "fnt_galahad14", (*f).name);
		featBox->add(item);
	}
	featBox->unlock();
}

Feat CharFeatsPopup::getChosenFeat() const {
	return _chosenFeat;
}

void CharFeatsPopup::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_chosenFeat = _feats[getListBox("FeatsList", true)->getSelected()];

		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_chosenFeat.featId = 0xFFFFFFFF;
		getListBox("FeatsList", true)->deselect();
		getButton("OkButton", true)->setDisabled(true);

		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "FeatsList" && getListBox("FeatsList", true)->getSelected() != 0xFFFFFFFF) {
		getButton("OkButton", true)->setDisabled(false);
	}
}

} // End of namespace NWN

} // End of namespace Engines
