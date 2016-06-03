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
 *  The class chooser in CharGen.
 */

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/listitembutton.h"

#include "src/engines/nwn/gui/chargen/charclass.h"

namespace Engines {

namespace NWN {

CharClass::CharClass(CharGenChoices &choices, ::Engines::Console *console) :
	CharGenBase(console) {

	_choices = &choices;
	load("cg_class");

	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(484), 1.0f);
	_classesListBox = getListBox("ClassListBox", true);

	_firstPrestigeClass = 0;
	createClassList();
}

CharClass::~CharClass() {
}

void CharClass::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "ClassListBox")
		type = NWN::GUI::kWidgetTypeListBox;
}

void CharClass::reset() {
	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", "");
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(484), 1.0f);

	_classesListBox->select(0);
}

void CharClass::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
	// Set previous choice if any.

		// Get previous choice.
		size_t previousChoice = 0;
		for (size_t it = 0; it < _classesId.size(); ++it) {
			if (_classesId[it] == _choices->getClass()) {
					previousChoice = it;
					break;
				}
		}

		// Setting previous choice.
		_classesListBox->select(previousChoice);
		getEditBox("HelpBox", true)->setText("fnt_galahad14", _helpTexts[previousChoice]);
		getEditBox("HelpBox", true)->setTitle("fnt_galahad14", _classNames[previousChoice]);
	}
}

void CharClass::createClassList() {
	_classesListBox->lock();

	_classesListBox->clear();
	_classesListBox->setMode(WidgetListBox::kModeSelectable);

	_classNames.clear();
	_helpTexts.clear();

	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("classes");
	for (size_t it = 0; it < twoda.getRowCount(); ++it) {
		const Aurora::TwoDARow &row = twoda.getRow(it);
		if (row.getInt("PlayerClass") == 0)
			continue;

		_classNames.push_back(TalkMan.getString(row.getInt("Name")));
		_helpTexts.push_back(TalkMan.getString(row.getInt("Description")));
		WidgetListItemButton *itemClass = new WidgetListItemButton(*this, "ctl_cg_btn_class",
                                                     _classNames.back(), row.getString("Icon"));
		_classesListBox->add(itemClass);
		_classesId.push_back(it);

		//TODO Implement a real check.
		if (!row.empty("PreReqTable")) {
			itemClass->setTextColor(0.5f, 0.5f, 0.5f, 1.0f);
		} else {
			++_firstPrestigeClass;
		}
	}

	_classesListBox->unlock();
	_classesListBox->select(0);
}

void CharClass::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setClass(_classesId[_classesListBox->getSelected()]);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		// Recommend fighter.
		_classesListBox->select(4);
		getEditBox("HelpBox", true)->setText("fnt_galahad14", _helpTexts[4]);
		getEditBox("HelpBox", true)->setTitle("fnt_galahad14", _classNames[4]);
		return;
	}

	if (widget.getTag() == "ClassListBox") {
		getEditBox("HelpBox", true)->setText("fnt_galahad14", _helpTexts[_classesListBox->getSelected()]);
		getEditBox("HelpBox", true)->setTitle("fnt_galahad14", _classNames[_classesListBox->getSelected()]);

		if (_classesListBox->getSelected() >= _firstPrestigeClass)
			getButton("OkButton", true)->setDisabled(true);
		else
			getButton("OkButton", true)->setDisabled(false);
	}
}

} // End of namespace NWN

} // End of namespace Engines
