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
 *  The character information GUI in CharGen.
 */

#include "src/common/util.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/ssffile.h"

#include "src/engines/aurora/util.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/chargen/charinfovoice.h"

namespace Engines {

namespace NWN {

WidgetListItemVoice::WidgetListItemVoice(::Engines::GUI &gui, const Common::UString &title, const Common::UString &soundSet, uint16_t soundSetID) :
    WidgetListItemButton(gui, "ctl_cg_btn_snds", title, "", kNoButton), _soundSetID(soundSetID) {

	_soundSet = std::make_unique<Aurora::SSFFile>(soundSet);
	_currentSound = SIZE_MAX;
}

WidgetListItemVoice::~WidgetListItemVoice() {
}

void WidgetListItemVoice::mouseDown(uint8_t state, float x, float y) {
	if (_currentSound < _soundSet->getSoundCount()) {
		++_currentSound;
	} else {
		_currentSound = 0;
	}

	setSound(_soundSet->getSoundFile(_currentSound));

	WidgetListItemButton::mouseDown(state, x, y);
}

bool WidgetListItemVoice::deactivate() {
	_currentSound = SIZE_MAX;

	return WidgetListItemButton::deactivate();
}

CharInfoVoice::CharInfoVoice(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;

	load("cg_char_info");

	initVoicesList();

	// TODO: Random button.
	getWidget("RandomButton", true)->setDisabled(true);
	// TODO: Deity button.
	getWidget("DeityButton", true)->setDisabled(true);
	// TODO: Enable name and description.
}

CharInfoVoice::~CharInfoVoice() {
}

void CharInfoVoice::reset() {
}

void CharInfoVoice::show() {
	CharGenBase::show();

	WidgetListItemVoice *firstItem =
		dynamic_cast<WidgetListItemVoice *>(getListBox("SoundSetEdit", true)->getSelectedItem());

	if (firstItem)
		playSound(firstItem->_soundSet->getSoundFile(0), Sound::kSoundTypeSFX);
}

void CharInfoVoice::callbackActive(Widget& widget) {
	if (widget.getTag() == "OkButton") {
		WidgetListItemVoice *item =
			dynamic_cast<WidgetListItemVoice *>(getListBox("SoundSetEdit", true)->getSelectedItem());

		if (!item)
			return;

		_choices->setSoundSet(item->_soundSetID);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}
}

void CharInfoVoice::initVoicesList() {
	WidgetListBox *voicesListBox = getListBox("SoundSetEdit", true);
	voicesListBox->lock();

	voicesListBox->clear();
	voicesListBox->setMode(WidgetListBox::kModeSelectable);

	const Aurora::TwoDAFile &twodaSoundSet = TwoDAReg.get2DA("soundset");
	for (size_t it = 0; it < twodaSoundSet.getRowCount(); ++it) {
		const Aurora::TwoDARow &row = twodaSoundSet.getRow(it);
		// Take only sound set for players.
		if (row.getInt("TYPE") != 0)
			continue;

		// Filter by gender.
		if (static_cast<Gender>(row.getInt("GENDER")) != _choices->getCharacter().getGender())
			continue;

		const Common::UString voiceName = TalkMan.getString(row.getInt("STRREF"));
		const Common::UString resRef = row.getString("RESREF");

		WidgetListItemVoice *item = new WidgetListItemVoice(*this, voiceName, resRef, (uint16_t) it);
		voicesListBox->add(item);
	}
	voicesListBox->unlock();

	voicesListBox->select(0);
}

} // End of namespace NWN

} // End of namespace Engines
