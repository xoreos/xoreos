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
 *  The feats selection GUI.
 */

#include <algorithm>

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/graphics/aurora/text.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/chargen/charfeathelp.h"
#include "src/engines/nwn/gui/chargen/charfeatspopup.h"
#include "src/engines/nwn/gui/chargen/charfeats.h"

namespace Engines {

namespace NWN {

WidgetListItemFeat::WidgetListItemFeat(::Engines::GUI &gui, Feat &feat) :
                                       WidgetListItemButton(gui, "ctl_cg_btn_feat", feat.name,
                                                            feat.icon, kMoveButton | kHelpButton),
                                       _feat(feat) {

	// Set maximum width for text.
	_text->set(feat.name, 230.f);

	setTag("Item#" + feat.name);
}

WidgetListItemFeat::~WidgetListItemFeat() {
}

void WidgetListItemFeat::callbackHelp() {
	dynamic_cast<CharFeats &>(*_gui).showFeatHelp(_feat);
}

void WidgetListItemFeat::subActive(Engines::Widget &widget) {
	if ((widget.getTag().endsWith("#MoveButtonLeft") || widget.getTag().endsWith("#MoveButtonRight"))
	        && movable()) {
		dynamic_cast<CharFeats &>(*_gui).moveFeat(this);
	}
}

CharFeats::CharFeats(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_feats");

	_normalFeats = 0;
	_bonusFeats = 0;

	// TODO: Recommend button
	getButton("RecommendButton", true)->setDisabled(true);
	// TODO: Reset button
	getButton("ResetButton", true)->setDisabled(true);

	_featHelp   = new CharFeatHelp(console);
	_featsPopup = new CharFeatsPopup(console);

	_availListBox = getListBox("AvailBox", true);
	_knownListBox = getListBox("KnownBox", true);

	makeAvailList();
	makeKnownList();
}

CharFeats::~CharFeats() {
	_availListBox->lock();
	_availListBox->clear();
	_availListBox->unlock();
	_knownListBox->lock();
	_knownListBox->clear();
	_knownListBox->unlock();

	delete _featHelp;
}

void CharFeats::reset() {
}

void CharFeats::fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type) {
	if (tag == "AvailBox" || tag == "KnownBox")
		type = NWN::GUI::kWidgetTypeListBox;
}

void CharFeats::showFeatHelp(Feat &feat) {
	_featHelp->setFeat(feat);

	_featHelp->show();
	_featHelp->run();
	_featHelp->hide();
}

void CharFeats::moveFeat(WidgetListItemFeat *item) {
	WidgetListBox *fromListBox = dynamic_cast<WidgetListBox *>(item->_owner);
	WidgetListBox *toListBox = 0;
	if (fromListBox->getTag() == "AvailBox") {
		toListBox = getListBox("KnownBox", true);

		// Check if there is a feat available.
		if (_normalFeats == 0)
			return;

		if (item->_feat.isMasterFeat) {
			std::vector<Feat> &featsList = _masterFeats[item->_feat];
			std::sort(featsList.begin(), featsList.end());
			_featsPopup->setFeats(featsList);
			_featsPopup->show();
			uint32 returnCode = _featsPopup->run();
			_featsPopup->hide();

			if (returnCode == 1)
				return;

			Feat chosenFeat = _featsPopup->getChosenFeat();
			removeFromMasterFeats(chosenFeat, item);
			changeAvailFeats(-1);
			return;
		}

		changeAvailFeats(-1);
	} else {
		toListBox = getListBox("AvailBox", true);
		changeAvailFeats(1);
		if (item->_feat.masterFeat != 0xFFFFFFFF) {
			addToMasterFeats(item->_feat, item);
			return;
		}
	}

	item->hide();
	item->deactivate();

	fromListBox->lock();
	fromListBox->remove(item);
	fromListBox->unlock();

	item->changeArrowDirection();

	toListBox->lock();
	toListBox->add(item, true);

	if (toListBox->getTag() == "AvailBox")
		toListBox->sortByTag();

	toListBox->unlock();
	item->activate();
}

void CharFeats::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		// Add chosen feats to choices.
		for (std::vector<WidgetListItem *>::iterator it = _knownListBox->begin(); it != _knownListBox->end(); ++it) {
			WidgetListItemFeat *featItem = dynamic_cast<WidgetListItemFeat *>(*it);
			if (!featItem->movable())
				continue;

			_choices->setCharFeat(featItem->_feat.featId);
		}

		_returnCode = 2;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
	}
}

void CharFeats::callbackRun() {
	if (_featsTrash.empty())
		return;

	removeFocus();
	for (std::vector<WidgetListItemFeat *>::iterator f = _featsTrash.begin(); f != _featsTrash.end(); ++f) {
		(*f)->remove();
	}
	_featsTrash.clear();
}

void CharFeats::makeAvailList() {
	uint8 level = _choices->getCharacter().getHitDice();
	// Get an additional feat each new level multiple of 3.
	if ((level + 1) % 3)
		++_normalFeats;
	// Get an additional feat at level 1.
	if (level == 0)
		++_normalFeats;
	// Get an additional level if the character is human (Quick Master feat).
	if (_choices->getRace() == 6)
		++_normalFeats;

	// Update text about the available feats.
	changeAvailFeats(0);
	// TODO Make bonus list.

	// Build the list of available feats.
	_availListBox->lock();
	_availListBox->clear();
	_availListBox->setMode(WidgetListBox::kModeSelectable);

	// Build list from all possible feats.
	const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");
	const Aurora::TwoDAFile &twodaMasterFeats = TwoDAReg.get2DA("masterfeats");

	for (size_t it = 0; it < twodaFeats.getRowCount(); ++it) {
		if (!_choices->hasPrereqFeat(it, false))
			continue;

		const Aurora::TwoDARow &featRow = twodaFeats.getRow(it);

		Feat feat;
		feat.featId = it;
		feat.name = TalkMan.getString(featRow.getInt("FEAT"));
		feat.icon = featRow.getString("ICON");
		feat.description = TalkMan.getString(featRow.getInt("DESCRIPTION"));
		feat.masterFeat = 0xFFFFFFFF;
		feat.isMasterFeat = false;

		// Check is the feat belongs to a masterfeat.
		if (!featRow.empty("MASTERFEAT")) {
			feat.masterFeat = featRow.getInt("MASTERFEAT");

			// Check if the masterfeat is already there.
			MasterFeatsMap::iterator ms = findMasterFeat(feat);
			if (ms == _masterFeats.end()) {
				Feat bareFeat = feat;
				const Aurora::TwoDARow &masterFeatRow = twodaMasterFeats.getRow(feat.masterFeat);
				feat.name = TalkMan.getString(masterFeatRow.getInt("STRREF"));
				feat.icon = masterFeatRow.getString("ICON");
				feat.isMasterFeat = true;
				feat.featId = feat.masterFeat;
				feat.masterFeat = 0xFFFFFFFF;
				_masterFeats[feat] = std::vector<Feat>(1, bareFeat);
			} else {
				ms->second.push_back(feat);
				continue;
			}
		}

		WidgetListItemFeat *featItem = new WidgetListItemFeat(*this, feat);
		_availListBox->add(featItem, true);
	}

	_availListBox->sortByTag();
	_availListBox->unlock();
}

void CharFeats::makeKnownList() {
	_knownListBox->lock();
	_knownListBox->clear();
	_knownListBox->setMode(WidgetListBox::kModeSelectable);
	std::vector<uint32> feats = _choices->getFeats();
	for (std::vector<uint32>::iterator f = feats.begin(); f != feats.end(); ++f) {
		const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");
		const Aurora::TwoDARow &featRow = twodaFeats.getRow(*f);

		Feat feat;
		feat.name = TalkMan.getString(featRow.getInt("FEAT"));
		feat.icon = featRow.getString("ICON");
		feat.description = TalkMan.getString(featRow.getInt("DESCRIPTION"));
		feat.masterFeat = 0xFFFFFFFF;
		feat.isMasterFeat = false;

		WidgetListItemFeat *featItem = new WidgetListItemFeat(*this, feat);
		_knownListBox->add(featItem, true);

		featItem->changeArrowDirection();
		featItem->setUnmovable();
	}

	_knownListBox->sortByTag();
	_knownListBox->unlock();
}

void CharFeats::changeAvailFeats(int8 diff) {
	_normalFeats += diff;
	getLabel("RemainLabel", true)->setText(Common::composeString<uint8>(_normalFeats));
}

void CharFeats::addToMasterFeats(Feat &feat, WidgetListItemFeat *featItem) {
	MasterFeatsMap::iterator ms = findMasterFeat(feat);
	if (ms == _masterFeats.end()) {
		error("Master feat from (%s) not found", feat.name.c_str());
		return;
	}

	// If there is no feat available, create the master feat item.
	if (ms->second.empty()) {
		Feat masterFeat = ms->first;
		WidgetListItemButton *item = new WidgetListItemFeat(*this, masterFeat);
		_availListBox->lock();
		_availListBox->add(item, true);
		_availListBox->sortByTag();
		_availListBox->unlock();
	}

	// Add feat to masterFeats list.
	ms->second.push_back(feat);

	// Remove item from the knownList.
	_knownListBox->lock();
	_knownListBox->remove(featItem);
	featItem->movePosition(0.f, 0.f, 100.f);
	_featsTrash.push_back(featItem);
	_knownListBox->unlock();
}

void CharFeats::removeFromMasterFeats(Feat &feat, WidgetListItemFeat *masterFeatItem) {
	MasterFeatsMap::iterator ms = findMasterFeat(feat);
	if (ms == _masterFeats.end()) {
		error("Master feat from (%s) not found", feat.name.c_str());
		return;
	}

	// Erase feat from masterFeats list.
	for (std::vector<Feat>::iterator f = ms->second.begin(); f != ms->second.end(); ++f) {
		if ((*f).featId != feat.featId)
			continue;

		ms->second.erase(f);
		break;
	}

	// If there is no feat left in the list, remove the master feat from the availables.
	if (ms->second.empty()) {
		_availListBox->lock();
		_availListBox->remove(masterFeatItem);
		_featsTrash.push_back(masterFeatItem);
		_availListBox->unlock();
	}

	// Add the chosen feat to the known feats.
	_knownListBox->lock();
	WidgetListItemFeat *featItem = new WidgetListItemFeat(*this, feat);
	_knownListBox->add(featItem, true);
	featItem->changeArrowDirection();
	_knownListBox->unlock();
}

MasterFeatsMap::iterator CharFeats::findMasterFeat(Feat &feat) {
	for (MasterFeatsMap::iterator it = _masterFeats.begin(); it != _masterFeats.end(); ++it) {
		if (it->first.featId == feat.masterFeat)
			return it;
	}

	return _masterFeats.end();
}

} // End of namespace NWN

} // End of namespace Engines
