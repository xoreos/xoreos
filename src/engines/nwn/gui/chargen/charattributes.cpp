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
 *  The attributes chooser in CharGen.
 */

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/common/strutil.h"
#include "src/common/util.h"

#include "src/graphics/aurora/modelnode.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/buttonsgroup.h"
#include "src/engines/nwn/gui/widgets/editbox.h"

#include "src/engines/nwn/gui/chargen/charattributes.h"

namespace Engines {

namespace NWN {

CharAttributes::CharAttributes(CharGenChoices &choices, ::Engines::Console *console) :
	CharGenBase(console) {

	_choices = &choices;
	load("cg_attributes");

	getButton("OkButton", true)->setDisabled(true);

	// Init attribute values.
	_attributes.assign(6, 8);
	_attrAdjust.assign(6, 0);
	_pointLeft = 30;

	init();
}

CharAttributes::~CharAttributes() {
}

void CharAttributes::reset() {
	getEditBox("HelpEdit", true)->setTitle("fnt_galahad14", TalkMan.getString(261));
	getEditBox("HelpEdit", true)->setText("fnt_galahad14", TalkMan.getString(457));

	_attributes.assign(6, 8);
	_attrAdjust.assign(6, 0);
	_pointLeft = 30;

	getButton("PointsEdit", true)->setText(Common::composeString<uint32_t>(_pointLeft));
	getButton("CostEdit", true)->setText("");

	getButton("OkButton", true)->setDisabled(true);
}

void CharAttributes::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setAbilities(_attributes, _attrAdjust);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		setRecommend();
		return;
	}

	const std::vector< WidgetButton* > buttonsList = _attrButtons->getButtonsList();

	// Update texts when attribute buttons are clicked.
	if (widget.getTag().endsWith("Label")) {
		_attrButtons->setActive(dynamic_cast<WidgetButton *>(&widget));

		for (uint b = 0; b < 6; ++b) {
			if (buttonsList[b] != &widget)
				continue;

			updateText(b);
			break;
		}
	}

	// Compute and update attributes when any Up and Down attribute buttons are triggered.
	for (size_t a = 0; a< 6; ++a) {
		if (buttonsList[a] != widget.getParent())
			continue;

		if (widget.getTag().endsWith("UpButton")) {
			uint cost = pointCost(_attributes[a] + 1);
			if ((_attributes[a] == 18) || (cost > _pointLeft))
				break;

			_pointLeft -= cost;
			_attributes[a]++;
		} else if (widget.getTag().endsWith("DownButton")) {
			if (_attributes[a] == 8)
					break;

				_pointLeft += pointCost(_attributes[a]);
				_attributes[a]--;
		}

		genTextAttributes(a);
		_attrButtons->setActive(a);
		updateText(a);

		if (_pointLeft == 0)
				getButton("OkButton", true)->setDisabled(false);
			else
				getButton("OkButton", true)->setDisabled(true);

		break;
	}
}

void CharAttributes::init() {
	// Set default text.
	getEditBox("HelpEdit", true)->setTitle("fnt_galahad14", TalkMan.getString(261));
	getEditBox("HelpEdit", true)->setText("fnt_galahad14", TalkMan.getString(457));

	// Get WidgetLabels of the attribute values.
	_labelAttributes.push_back(getLabel("StrEdit", true));
	_labelAttributes.push_back(getLabel("DexEdit", true));
	_labelAttributes.push_back(getLabel("ConEdit", true));
	_labelAttributes.push_back(getLabel("WisEdit", true));
	_labelAttributes.push_back(getLabel("IntEdit", true));
	_labelAttributes.push_back(getLabel("ChaEdit", true));
}

void CharAttributes::initButtonsGroup() {
	//TODO Help text should implement racial bonus as well as current attribute value.

	_attrButtons = std::make_unique<ButtonsGroup>(getEditBox("HelpEdit", true));
	_attrButtons->addButton(getButton("StrLabel", true), TalkMan.getString(473), TalkMan.getString(459));
	_attrButtons->addButton(getButton("DexLabel", true), TalkMan.getString(474), TalkMan.getString(460));
	_attrButtons->addButton(getButton("ConLabel", true), TalkMan.getString(475), TalkMan.getString(461));
	_attrButtons->addButton(getButton("WisLabel", true), TalkMan.getString(476), TalkMan.getString(462));
	_attrButtons->addButton(getButton("IntLabel", true), TalkMan.getString(477), TalkMan.getString(463));
	_attrButtons->addButton(getButton("ChaLabel", true), TalkMan.getString(479), TalkMan.getString(478));

	// Set buttons to unchanged mode.
	const std::vector< WidgetButton* > buttonsList = _attrButtons->getButtonsList();
	for (std::vector<WidgetButton *>::const_iterator it = buttonsList.begin(); it != buttonsList.end(); ++it)
		(*it)->setMode(WidgetButton::kModeUnchanged);
}

void CharAttributes::show() {
	// Check attribute adjustment from racial type.
	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("racialtypes");
	const Aurora::TwoDARow  &row   = twoda.getRow(_choices->getRace());
	_attrAdjust.clear();
	_attrAdjust.push_back(row.getInt(8));
	_attrAdjust.push_back(row.getInt(9));
	_attrAdjust.push_back(row.getInt(13));
	_attrAdjust.push_back(row.getInt(12));
	_attrAdjust.push_back(row.getInt(10));
	_attrAdjust.push_back(row.getInt(11));

	// Update attribute text accordingly.
	for (size_t it = 0; it < 6; ++it) {
		genTextAttributes(it);
	}

	// Init attribute buttons.
	initButtonsGroup();

	Engines::GUI::show();
}

void CharAttributes::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
		// Get previous choice.
		uint sum = 0;
		for (size_t ab = 0; ab < 6; ++ab) {
			_attributes[ab] = _choices->getAbility(static_cast<Ability>(ab));
			sum += _attributes[ab];
		}
		_pointLeft = 0;
		getButton("PointsEdit", true)->setText("0");

		// Attributes were never saved if sum equals 48.
		if (sum == 48)
			reset();
	}
}

void CharAttributes::genTextAttributes(size_t attribute) {
	// Compute modifier.
	uint8_t realValue = _attributes[attribute] + _attrAdjust.at(attribute);
	realValue -= 6;
	int8_t modifier = (realValue - realValue % 2) / 2;
	modifier -= 2;

	// Transform numbers into strings.
	Common::UString sign = (modifier < 0) ? "" : "+";
	Common::UString attr = Common::composeString<uint8_t>(_attributes[attribute]
	                                                     + _attrAdjust.at(attribute));
	Common::UString modifStr = Common::composeString<int8_t>(modifier);

	Common::UString output = attr + " (" + sign + modifStr + ")";

	// Update text.
	_labelAttributes[attribute]->setText(output);
}

uint8_t CharAttributes::pointCost(uint8_t attrValue) {
	if (attrValue < 15) {
		getButton("CostEdit", true)->setText("1");
		return 1;
	}

	attrValue -= 15;
	uint8_t cost = ((attrValue - attrValue % 2) / 2) + 2;

	getButton("CostEdit", true)->setText(Common::composeString<uint8_t>(cost));

	return cost;
}

void CharAttributes::updateText(uint8_t attribute) {
	getButton("PointsEdit", true)->setText(Common::composeString<uint8_t>(_pointLeft));
	pointCost(_attributes[attribute] + 1);
}

void CharAttributes::setRecommend() {
	_pointLeft = 0;
	const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
	const Aurora::TwoDARow  &row = twodaClasses.getRow(_choices->getClass());
	for (uint it = 0; it < 6; ++it) {
		_attributes.at(it) = row.getInt(17 + it);
		genTextAttributes(it);
	}

	getButton("PointsEdit", true)->setText("0");

	getButton("OkButton", true)->setDisabled(false);
}

} // End of namespace NWN

} // End of namespace Engines
