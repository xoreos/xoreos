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
 *  The package selection GUI.
 */

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/talkman.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/listitembutton.h"

#include "src/engines/nwn/gui/chargen/charskills.h"
#include "src/engines/nwn/gui/chargen/charfeats.h"
#include "src/engines/nwn/gui/chargen/charschool.h"
#include "src/engines/nwn/gui/chargen/chardomain.h"
#include "src/engines/nwn/gui/chargen/charspells.h"
#include "src/engines/nwn/gui/chargen/charpackage.h"

namespace Engines {

namespace NWN {

CharPackage::CharPackage(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_package");

	_packageListBox = getListBox("ClassListBox", true);
}

CharPackage::~CharPackage() {
}

void CharPackage::reset() {
	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(483));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(487));

	_choices->resetPackage();

	_subGUIs.clear();
}

void CharPackage::show() {
	createPackageList();

	Engines::GUI::show();
}

void CharPackage::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setPackage(_packageID[_packageListBox->getSelected()]);

		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
		}

	if (widget.getTag() == "ClassListBox") {
		size_t choice = _packageListBox->getSelected();
		getEditBox("HelpBox", true)->setText("fnt_galahad14", _helpTexts[choice]);
		getEditBox("HelpBox", true)->setTitle("fnt_galahad14", _packageNames[choice]);
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		// Recommend first package.
		_packageListBox->select(0);
	}

	if (widget.getTag() == "ConfigurePckg") {
		if (_subGUIs.size() == 0) {
			CharSkills *charSkills = new CharSkills(*_choices, _console);
			_subGUIs.push_back(charSkills);
			// TODO Check if new feats are needed.
			CharFeats *charFeats = new CharFeats(*_choices, _console);
			_subGUIs.push_back(charFeats);

			// Add spell GUI if needed
			const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
			const Aurora::TwoDARow &rowClass = twodaClasses.getRow(_choices->getClass());
			if (rowClass.getInt("SpellCaster") > 0) {
				if (rowClass.getString("SpellGainTable") == "CLS_SPGN_WIZ" &&
				    _choices->getCharacter().getHitDice() == 0) {
					CharSchool *charSchool = new CharSchool(*_choices, _console);
					_subGUIs.push_back(charSchool);
					CharSpells *charSpells = new CharSpells(*_choices, _console);
					_subGUIs.push_back(charSpells);
				} else if (rowClass.getString("SpellGainTable") == "CLS_SPGN_CLER" &&
				           _choices->getCharacter().getHitDice() == 0) {
					CharDomain *charDomain = new CharDomain(*_choices, _console);
					_subGUIs.push_back(charDomain);
				} else if (!rowClass.empty("SpellKnownTable")) {
					CharSpells *charSpells = new CharSpells(*_choices, _console);
					_subGUIs.push_back(charSpells);
				}
			}
			//TODO: Add animal companion/familiar GUI.
		}

		uint32_t subReturnCode;
		for (Common::PtrVector<CharGenBase>::iterator g = _subGUIs.begin(); g != _subGUIs.end(); ++g) {
			subReturnCode = sub(**g, 0, false);
			if (subReturnCode == 1) {
				reset();
				_returnCode = 1;
				return;
			}
		}

		_returnCode = 2;
	}
}

void CharPackage::createPackageList() {
	_packageListBox->lock();
	_packageListBox->clear();
	_packageListBox->setMode(WidgetListBox::kModeSelectable);

	_helpTexts.clear();
	_packageID.clear();
	_packageNames.clear();

	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("packages");
	for (size_t it = 0; it < twoda.getRowCount(); ++it) {
		const Aurora::TwoDARow &row = twoda.getRow(it);
		if (row.getInt("PlayerClass") == 0 ||
		    row.getInt("ClassID") != (int) _choices->getClass() ||
		    row.getInt("Name") == 0)
			continue;

		_helpTexts.push_back(TalkMan.getString(row.getInt("Description")));
		_packageNames.push_back(TalkMan.getString(row.getInt("Name")));
		WidgetListItemButton *packagetItem = new WidgetListItemButton(*this, "ctl_cg_btn_class", TalkMan.getString(row.getInt("Name")), "");
		_packageListBox->add(packagetItem);

		_packageID.push_back((uint8_t) it);
	}
	_packageListBox->unlock();

	// Set previous choice if any.
	uint8_t package = _choices->getPackage();
	if (package == UINT8_MAX) {
		getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(483));
		getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(487));

		_packageListBox->select(0);
		return;
	}

	size_t p = 0;
	for (; p < _packageID.size(); ++p) {
		if (_packageID[p] == package)
			break;
	}

		_packageListBox->select(p);
}

} // End of namespace NWN

} // End of namespace Engines
