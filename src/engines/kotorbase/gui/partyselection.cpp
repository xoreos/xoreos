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
 *  Party selection GUI for KotOR games.
 */

#include "src/common/endianness.h"
#include "src/common/strutil.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/aurora/widget.h"
#include "src/engines/aurora/util.h"

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/checkbox.h"
#include "src/engines/odyssey/button.h"

#include "src/engines/kotorbase/gui/partyselection.h"

namespace Engines {

namespace KotORBase {

PartySelectionGUI::PartySelectionGUI(int slotCount) :
		_slotCount(slotCount),
		_numSelectedSlots(0),
		_activeSlot(-1) {
}

void PartySelectionGUI::loadConfiguration(const PartyConfiguration &config) {
	_config = config;
	_numSelectedSlots = 0;
	_activeSlot = -1;

	if (_config.forceNPC1 != -1) {
		_config.slotSelected[_config.forceNPC1] = true;
		++_numSelectedSlots;
	}

	if (_config.forceNPC2 != -1) {
		_config.slotSelected[_config.forceNPC2] = true;
		++_numSelectedSlots;
	}

	for (int i = 0; i < _slotCount; ++i) {
		if (_config.slotTemplate[i].empty())
			toggleSlot(i, false);
		else {
			const Common::UString portrait(getPortrait(_config.slotTemplate[i]));
			setSlotTexture(i, portrait);
			toggleSlot(i, true);
		}

		Odyssey::WidgetCheckBox *checkBox = getCheckBox("BTN_NPC" + Common::composeString(i));
		const bool selected = _config.slotSelected[i];
		checkBox->setState(selected);
		checkBox->setHighlight(selected);
	}

	getButton("BTN_BACK")->setDisabled(!config.canCancel);

	onSelectionChanged();
}

void PartySelectionGUI::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (tag == "BTN_BACK")
		_returnCode = 1;
	else if (tag == "BTN_DONE")
		_returnCode = 2;
	else if (tag.beginsWith("BTN_NPC")) {
		Common::UString tmp(tag);
		tmp.replaceAll("BTN_NPC", "");
		int index;
		Common::parseString(tmp, index);
		_activeSlot = index;
		onSelectionChanged();

	} else if (tag == "BTN_ACCEPT") {
		if ((_activeSlot != _config.forceNPC1) &&
		    (_activeSlot != _config.forceNPC2)) {;
			if (!_config.slotSelected[_activeSlot]) {
				if (_numSelectedSlots < 2) {
					_config.slotSelected[_activeSlot] = true;
					++_numSelectedSlots;
				}
			} else {
				_config.slotSelected[_activeSlot] = false;
				--_numSelectedSlots;
			}
		}

		onSelectionChanged();
	}
}

const Common::UString PartySelectionGUI::getPortrait(const Common::UString &templ) {
	std::unique_ptr<Aurora::GFF3File> utc(loadOptionalGFF3(templ,
	                                                         Aurora::kFileTypeUTC,
	                                                         MKTAG('U', 'T', 'C', ' ')));

	if (!utc)
		throw Common::Exception("Invalid creature template: %s", templ.c_str());

	const Aurora::GFF3Struct &gff = utc->getTopLevel();
	Common::UString portrait;

	uint32_t portraitId = gff.getUint("PortraitId");
	if (portraitId != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");
		portrait = twoda.getRow(portraitId).getString("BaseResRef");
	}

	return gff.getString("Portrait", portrait);
}

void PartySelectionGUI::setSlotTexture(int index, const Common::UString &texture) {
	Odyssey::WidgetCheckBox *checkBox = getCheckBox("BTN_NPC" + Common::composeString(index));
	checkBox->setFill(texture);
	checkBox->setSelected(texture);
	checkBox->setUnselected(texture);
	checkBox->setSelectedHighlighted(texture);
	checkBox->setUnselectedHighlighted(texture);
}

void PartySelectionGUI::toggleSlot(int index, bool enabled) {
	getLabel("LBL_NA" + Common::composeString(index))->setInvisible(enabled);
	getLabel("LBL_CHAR" + Common::composeString(index))->setInvisible(enabled);
}

void PartySelectionGUI::onSelectionChanged() {
	getLabel("LBL_COUNT")->setText(Common::composeString(2 - _numSelectedSlots));

	for (int i = 0; i < _slotCount; ++i) {
		Odyssey::WidgetCheckBox *checkBox = getCheckBox("BTN_NPC" + Common::composeString(i));

		if (_config.slotSelected[i])
			checkBox->setBorderColor(0.0f, 1.0f, 0.0f, 1.0f);
		else if (_activeSlot == i)
			checkBox->setBorderColor(1.0f, 1.0f, 0.0f, 1.0f);
		else
			checkBox->setBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	const bool activeSlotSelected = (_activeSlot != -1) && (_config.slotSelected[_activeSlot]);
	getButton("BTN_ACCEPT")->setText(activeSlotSelected ? "Remove" : "Add");
}

} // End of namespace KotOR

} // End of namespace Engines
