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
 *  The alignment chooser in CharGen.
 */

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/buttonsgroup.h"
#include "src/engines/nwn/gui/widgets/editbox.h"

#include "src/engines/nwn/gui/chargen/charalignment.h"

namespace Engines {

namespace NWN {

CharAlignment::CharAlignment(CharGenChoices &choices, ::Engines::Console *console) :
	CharGenBase(console) {

	_choices  = &choices;
	_loyalty  = 101;
	_goodness = 101;

	load("cg_alignment");

	getEditBox("HelpEdit", true)->setTitle("fnt_galahad14", TalkMan.getString(111));
	getEditBox("HelpEdit", true)->setText("fnt_galahad14", TalkMan.getString(458));

	_buttons = new ButtonsGroup(getEditBox("HelpEdit", true));

	uint textID  = 448;

	_buttons->addButton(getButton("CEButton"), TalkMan.getString(120), TalkMan.getString(textID));
	_buttons->addButton(getButton("CGButton"), TalkMan.getString(118), TalkMan.getString(++textID));
	_buttons->addButton(getButton("CNButton"), TalkMan.getString(119), TalkMan.getString(++textID));
	_buttons->addButton(getButton("LEButton"), TalkMan.getString(114), TalkMan.getString(++textID));
	_buttons->addButton(getButton("LGButton"), TalkMan.getString(112), TalkMan.getString(++textID));
	_buttons->addButton(getButton("LNButton"), TalkMan.getString(113), TalkMan.getString(++textID));
	_buttons->addButton(getButton("NEButton"), TalkMan.getString(117), TalkMan.getString(++textID));
	_buttons->addButton(getButton("NGButton"), TalkMan.getString(115), TalkMan.getString(++textID));
	_buttons->addButton(getButton("TNButton"), TalkMan.getString(116), TalkMan.getString(++textID));

	getButton("OkButton", true)->setDisabled(true);

	//TODO Recomend button for alignment.
	getWidget("RecommendButton", true)->setDisabled(true);
}

CharAlignment::~CharAlignment() {
	delete _buttons;
}

void CharAlignment::reset() {
	getEditBox("HelpEdit", true)->setTitle("fnt_galahad14", TalkMan.getString(111));
	getEditBox("HelpEdit", true)->setText("fnt_galahad14", TalkMan.getString(458));

	getButton("OkButton", true)->setDisabled(true);
	_buttons->setAllInactive();
	_goodness = 101;
	_loyalty = 101;
	_choices->setCharAlign(_goodness, _loyalty);
}

void CharAlignment::show() {
	Engines::GUI::show();

	setRestrict();
}

void CharAlignment::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
		// If alignment has been previously select, set it back.
		if (_choices->getAlign(_goodness, _loyalty)) {
			float point[3] = { 0, 100, 50 };

			uint loyalButton = 0;
			uint goodnessButton = 0;
			for (uint a = 0; a < 3; ++a) {
				if (point[a] == _loyalty)
					loyalButton = 3 * a;

				if (point[a] == _goodness)
					goodnessButton = a;
			}
			_buttons->setActive(loyalButton + goodnessButton);
		} else {
			reset();
		}
	}
}

void CharAlignment::callbackActive(Widget& widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setCharAlign(_goodness, _loyalty);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	_buttons->setActive((WidgetButton *) &widget);
	getAlignment();
	getButton("OkButton", true)->setDisabled(false);
}

void CharAlignment::setRestrict() {
	// If restriction has already been set, directly return.
	if (_goodness < 101)
		return;

	const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
	const Aurora::TwoDARow &row = twodaClasses.getRow(_choices->getClass());

	uint alignRestrict = row.getInt("AlignRestrict");
	bool invertRestrict = row.getInt("InvertRestrict") != 0;

	// Build restriction axis.
	uint axisRestrict[5] = { 0x01, 0x02, 0x04, 0x08, 0x10 };
	uint good[] = { 1, 4, 7 };
	uint evil[] = { 0, 3, 6 };
	uint loyal[] = { 3, 4, 5 };
	uint chaos[] = { 0, 1, 2 };

	std::vector<uint> goodAxis(good, good + sizeof(good) / sizeof(uint));
	std::vector<uint> evilAxis(evil, evil + sizeof(evil) / sizeof(uint));
	std::vector<uint> loyalAxis(loyal, loyal + sizeof(loyal) / sizeof(uint));
	std::vector<uint> chaosAxis(chaos, chaos + sizeof(chaos) / sizeof(uint));
	std::vector<uint> neutralAxis;

	neutralAxis.push_back(8);

	uint alignRstrctType = row.getInt("AlignRstrctType");

	if (alignRstrctType == 0x1) {
		neutralAxis.push_back(6);
		neutralAxis.push_back(7);
	} else if (alignRstrctType == 0x2) {
		neutralAxis.push_back(2);
		neutralAxis.push_back(5);
	} else if (alignRstrctType == 0x3) {
		neutralAxis.push_back(6);
		neutralAxis.push_back(7);
		neutralAxis.push_back(2);
		neutralAxis.push_back(5);
	}

	std::vector<std::vector<uint> > axis;
	axis.push_back(neutralAxis);
	axis.push_back(loyalAxis);
	axis.push_back(chaosAxis);
	axis.push_back(goodAxis);
	axis.push_back(evilAxis);

	const std::vector<WidgetButton *> bList = _buttons->getButtonsList();

	for (std::vector<WidgetButton *>::const_iterator it = bList.begin(); it != bList.end(); ++it)
		(*it)->setDisabled(invertRestrict);

	for (uint it = 0; it < 5; ++it) {
		if (axisRestrict[it] & alignRestrict) {
			for (std::vector<uint>::iterator ax = axis[it].begin(); ax != axis[it].end(); ++ax)
				bList[*ax]->setDisabled(!invertRestrict);
		}
	}
}

void CharAlignment::getAlignment() {
	size_t button = _buttons->getChoice();

	float  point[3] = { 0, 100, 50 };
	size_t rest     = button % 3;
	_goodness = point[rest];
	_loyalty  = point[(button - rest) / 3];
}

} // End of namespace NWN

} // End of namespace Engines
