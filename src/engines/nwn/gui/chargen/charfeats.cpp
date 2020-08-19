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
#include "src/common/uuid.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/text.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/chargen/charhelp.h"
#include "src/engines/nwn/gui/chargen/charfeatspopup.h"
#include "src/engines/nwn/gui/chargen/charfeats.h"

namespace Engines {

namespace NWN {

WidgetListItemFeat::WidgetListItemFeat(::Engines::GUI &gui, FeatItem &feat) :
                                       WidgetListItemButton(gui, "ctl_cg_btn_feat",
                                                            feat.name + "#" + Common::generateIDNumberString(),
                                                            feat.icon, kMoveButton | kHelpButton),
                                       _feat(feat) {

	// Set maximum width for text.
	_text->set(feat.name, 230.f);

	setTag("Item#" + feat.name + "#" + Common::generateIDNumberString());
}

WidgetListItemFeat::~WidgetListItemFeat() {
}

void WidgetListItemFeat::callbackHelp() {
	dynamic_cast<CharFeats &>(*_gui).showFeatHelp(_feat);
}

void WidgetListItemFeat::subActive(Engines::Widget &widget) {
	if ((widget.getTag().endsWith("#MoveButtonLeft") || widget.getTag().endsWith("#MoveButtonRight")) &&
	    movable()) {
		dynamic_cast<CharFeats &>(*_gui).moveFeat(this);
	}
}

CharFeats::CharFeats(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_feats");

	_normalFeats = 0;
	_bonusFeats = 0;

	_featHelp = std::make_unique<CharHelp>("cg_featinfo", console);
	_featsPopup = std::make_unique<CharFeatsPopup>(console);

	_availListBox = getListBox("AvailBox", true);
	_knownListBox = getListBox("KnownBox", true);

	_choices->getFeatItems(_availFeats, _normalFeats, _bonusFeats);
	_hasBonusFeats = _bonusFeats > 0;

	// Update text about the available feats.
	changeAvailFeats(0, true);

	// By default, show both general and bonus feats.
	makeAvailList(1);
	makeKnownList();

	getButton("OkButton", true)->setDisabled(true);
}

CharFeats::~CharFeats() {
	_availListBox->lock();
	_availListBox->clear();
	_availListBox->unlock();
	_knownListBox->lock();
	_knownListBox->clear();
	_knownListBox->unlock();
}

void CharFeats::reset() {
	while (_knownFeats.size() > 0) {
		moveFeat(_knownFeats.front(), false);
	}
}

void CharFeats::fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type) {
	if (tag == "AvailBox" || tag == "KnownBox")
		type = NWN::GUI::kWidgetTypeListBox;
}

void CharFeats::showFeatHelp(FeatItem &feat) {
	_featHelp->setContent(feat.name, feat.description, feat.icon);

	_featHelp->show();
	_featHelp->run();
	_featHelp->hide();
}

void CharFeats::moveFeat(const FeatItem &feat, bool toKnownFeats, bool rebuild) {
	// Check if there is enough remaining feats available.
	if (toKnownFeats) {
		if (_bonusFeats + _normalFeats == 0)
			return;
		if (_bonusFeats == 0 && feat.list > 1)
			return;
		if (_normalFeats == 0 && feat.list < 1)
			return;
	}

	std::list<FeatItem> &toList = (toKnownFeats) ? _knownFeats : _availFeats;
	std::list<FeatItem> &fromList = (toKnownFeats) ? _availFeats : _knownFeats;
	int8_t diff = (toKnownFeats) ? -1 : 1;

	for (std::list<FeatItem>::iterator aF = fromList.begin(); aF != fromList.end(); ++aF) {
		if ((*aF).featId != feat.featId)
			continue;

		toList.push_back((*aF));
		fromList.erase(aF);
		break;
	}

	bool isNormalFeat = true;
	if (toKnownFeats) {
		isNormalFeat = (feat.list < 2 && _bonusFeats == 0) || (feat.list == 0);
	} else {
		isNormalFeat = (feat.list == 0) || (feat.list < 2 && _bonusFeats > 0);
	}

	changeAvailFeats(diff, isNormalFeat, rebuild);
}

void CharFeats::moveFeat(WidgetListItemFeat *item) {
	WidgetListBox *fromListBox = dynamic_cast<WidgetListBox *>(item->_owner);

	// Check if there is enough remaining feats available.
	if (!fromListBox || ((fromListBox->getTag() == "AvailBox") && ((_bonusFeats + _normalFeats) == 0)))
		return;

	if (!item->_feat.isMasterFeat) {
		item->hide();
		removeFocus();
		fromListBox->lock();
		fromListBox->remove(item);
		fromListBox->unlock();

		FeatItem featItem = item->_feat;
		_featsTrash.push_back(item);
		moveFeat(featItem, fromListBox->getTag() == "AvailBox");
		return;
	}

	// For master feat items.

	// Find feats that belong to the master feat.
	std::vector<FeatItem> slaveFeatsList;
	for (std::list<FeatItem>::iterator f = _availFeats.begin(); f != _availFeats.end(); ++f) {
		if ((*f).masterFeat != item->_feat.featId)
			continue;

		slaveFeatsList.push_back(*f);
	}

	// Show the popup.
	std::sort(slaveFeatsList.begin(), slaveFeatsList.end());
	_featsPopup->setFeats(slaveFeatsList);
	removeFocus();
	_featsPopup->show();
	uint32_t returnCode = _featsPopup->run();
	_featsPopup->hide();

	if (returnCode == 1)
		return;

	GfxMan.lockFrame();
	fromListBox->lock();
	fromListBox->remove(item);
	fromListBox->unlock();
	_featsTrash.push_back(item);

	FeatItem chosenFeat = _featsPopup->getChosenFeat();
	moveFeat(chosenFeat, true);
	GfxMan.unlockFrame();
}

void CharFeats::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		// Add chosen feats to choices.
		for (std::vector<WidgetListItem *>::iterator it = _knownListBox->begin(); it != _knownListBox->end(); ++it) {
			WidgetListItemFeat &featItem = dynamic_cast<WidgetListItemFeat &>(**it);
			if (!featItem.movable())
				continue;

			_choices->setFeat(featItem._feat.featId);
		}

		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "ResetButton") {
		GfxMan.lockFrame();
		reset();
		GfxMan.unlockFrame();
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		GfxMan.lockFrame();
		setRecommendedFeats();
		GfxMan.unlockFrame();
		return;
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

void CharFeats::makeAvailList(uint8_t list) {
	// Build the list of available feats.
	_availListBox->lock();
	_availListBox->clear();
	_availListBox->setMode(WidgetListBox::kModeSelectable);

	const Aurora::TwoDAFile &twodaMasterFeats = TwoDAReg.get2DA("masterfeats");

	std::list<uint32_t> masterFeats;
	for (std::list<FeatItem>::iterator f = _availFeats.begin(); f != _availFeats.end(); ++f) {
		if (list == 0 && (*f).list > 1)
			continue;

		if (list == 2 && (*f).list < 1)
			continue;

		FeatItem feat = *f;
		// Check if the feat belongs to a masterfeat.
		if (feat.masterFeat < 0xFFFFFFFF) {
			// Check if the masterfeat is already there.
			bool found = false;
			for (std::list<uint32_t>::iterator mf = masterFeats.begin(); mf != masterFeats.end(); ++mf) {
				if (feat.masterFeat == *mf) {
					found = true;
					break;
				}
			}

			if (found)
				continue;

			masterFeats.push_back(feat.masterFeat);

			const Aurora::TwoDARow &masterFeatRow = twodaMasterFeats.getRow(feat.masterFeat);
			feat.name = TalkMan.getString(masterFeatRow.getInt("STRREF"));
			feat.icon = masterFeatRow.getString("ICON");
			feat.isMasterFeat = true;
			feat.featId = feat.masterFeat;
			feat.masterFeat = 0xFFFFFFFF;
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
	std::vector<uint32_t> feats;
	_choices->getFeats(feats);
	for (std::vector<uint32_t>::iterator f = feats.begin(); f != feats.end(); ++f) {
		const Aurora::TwoDAFile &twodaFeats = TwoDAReg.get2DA("feat");
		const Aurora::TwoDARow &featRow = twodaFeats.getRow(*f);

		FeatItem feat;
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

	for (std::list<FeatItem>::iterator f = _knownFeats.begin(); f != _knownFeats.end(); ++f) {
		WidgetListItemFeat *featItem = new WidgetListItemFeat(*this, *f);
		featItem->changeArrowDirection();
		_knownListBox->add(featItem, true);
	}

	_knownListBox->unlock();
}

void CharFeats::changeAvailFeats(int8_t diff, bool normalFeat, bool rebuild) {
	if (normalFeat) {
		_normalFeats += diff;
	} else {
		_bonusFeats  += diff;
	}

	int8_t list = 1;
	if (_bonusFeats == 0 && _hasBonusFeats)
		list = 0;

	if (_bonusFeats > 0 && _normalFeats == 0)
		list = 2;

	if (rebuild) {
		size_t startItem = 0;
		GfxMan.lockFrame();
		if (_normalFeats + _bonusFeats == 0) {
			_availListBox->lock();
			_availListBox->clear();
			_availListBox->unlock();
		} else {
			startItem = _availListBox->getStartItem();
			makeAvailList(list);
			_availListBox->setStartItem(startItem);
		}

		startItem = _knownListBox->getStartItem();
		makeKnownList();
		_knownListBox->setStartItem(startItem);
		GfxMan.unlockFrame();
	}

	getLabel("RemainLabel", true)->setText(Common::composeString<uint8_t>(_normalFeats + _bonusFeats));

	// Enable/Disable the OK button.
	if (_normalFeats + _bonusFeats != 0) {
		getButton("OkButton", true)->setDisabled(true);
	} else {
		getButton("OkButton", true)->setDisabled(false);
	}
}

void CharFeats::setRecommendedFeats() {
	// Set list to the initial state.
	reset();

	// Retrieve recommended feats.
	std::vector<uint32_t> recFeats;
	_choices->getPrefFeats(recFeats);

	for (std::vector<uint32_t>::iterator rF = recFeats.begin(); rF != recFeats.end(); ++rF) {
		if (_bonusFeats + _normalFeats == 0)
			break;

		for (std::list<FeatItem>::iterator aF = _availFeats.begin(); aF != _availFeats.end(); ++aF) {
			if (*rF != (*aF).featId)
				continue;

			// Avoid unnecessary list rebuild.
			bool rebuild = _bonusFeats + _normalFeats == 1;
			moveFeat(*aF, true, rebuild);
			break;
		}
	}

	if (_bonusFeats + _normalFeats != 0)
		error("Unable to select the recommended feats");
}

} // End of namespace NWN

} // End of namespace Engines
