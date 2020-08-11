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
 *  The KotOR 2 class selection.
 */

#include "src/aurora/talkman.h"

#include "src/engines/kotor2/gui/chargen/classselection.h"
#include "src/engines/kotor2/gui/chargen/charactergeneration.h"
#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

ClassSelection::ClassSelection(KotORBase::Module *module, Console *console) :
		KotORBase::GUI(console) {

	load("classsel_p");

	_module = module;

	_consularMaleInfo = CharacterGenerationInfo::createRandomMaleConsular();
	_guardianMaleInfo = CharacterGenerationInfo::createRandomMaleGuardian();
	_sentinelMaleInfo = CharacterGenerationInfo::createRandomMaleSentinel();
	_consularFemaleInfo = CharacterGenerationInfo::createRandomFemaleConsular();
	_guardianFemaleInfo = CharacterGenerationInfo::createRandomFemaleGuardian();
	_sentinelFemaleInfo = CharacterGenerationInfo::createRandomFemaleSentinel();

	Common::UString malePrefix, femalePrefix;
	malePrefix = TalkMan.getString(646);
	femalePrefix = TalkMan.getString(647);

	_guardianMaleTitle = malePrefix + " " + TalkMan.getString(355);
	_guardianFemaleTitle = femalePrefix + " " + TalkMan.getString(355);
	_consularMaleTitle = malePrefix + " " + TalkMan.getString(354);
	_consularFemaleTitle = femalePrefix + " " + TalkMan.getString(354);
	_sentinelMaleTitle = malePrefix + " " + TalkMan.getString(353);
	_sentinelFemaleTitle = femalePrefix + " " + TalkMan.getString(353);

	_guardianDescription = TalkMan.getString(48032);
	_sentinelDescription = TalkMan.getString(48033);
	_consularDescription = TalkMan.getString(48031);

	_labelDesc = getLabel("LBL_DESC");
	_labelDesc->setWrapped(true);

	_labelTitle = getLabel("LBL_CLASS");

	_consularMaleButton = getButton("BTN_SEL1");
	_guardianMaleButton = getButton("BTN_SEL2");
	_sentinelMaleButton = getButton("BTN_SEL3");
	_sentinelFemaleButton = getButton("BTN_SEL4");
	_guardianFemaleButton = getButton("BTN_SEL5");
	_consularFemaleButton = getButton("BTN_SEL6");

	_labelDesc->setText(_consularDescription);
	_labelTitle->setText(_consularMaleTitle);
	_hoveredButton = _consularMaleButton;
}

ClassSelection::~ClassSelection() {
	delete _consularMaleInfo;
	delete _guardianMaleInfo;
	delete _sentinelMaleInfo;
	delete _consularFemaleInfo;
	delete _guardianFemaleInfo;
	delete _sentinelFemaleInfo;
}

void ClassSelection::callbackRun() {
	if (_consularMaleButton->isHovered() && _hoveredButton != _consularMaleButton) {
		_labelDesc->setText(_consularDescription);
		_labelTitle->setText(_consularMaleTitle);
		_hoveredButton = _consularMaleButton;
	} else if (_consularFemaleButton->isHovered() && _hoveredButton != _consularFemaleButton) {
		_labelDesc->setText(_consularDescription);
		_labelTitle->setText(_consularFemaleTitle);
		_hoveredButton = _consularFemaleButton;
	} else if (_sentinelMaleButton->isHovered() && _hoveredButton != _sentinelMaleButton) {
		_labelDesc->setText(_sentinelDescription);
		_labelTitle->setText(_sentinelMaleTitle);
		_hoveredButton = _sentinelMaleButton;
	} else if (_sentinelFemaleButton->isHovered() && _hoveredButton != _sentinelFemaleButton) {
		_labelDesc->setText(_sentinelDescription);
		_labelTitle->setText(_sentinelFemaleTitle);
		_hoveredButton = _sentinelFemaleButton;
	} else if (_guardianMaleButton->isHovered() && _hoveredButton != _guardianMaleButton) {
		_labelDesc->setText(_guardianDescription);
		_labelTitle->setText(_guardianMaleTitle);
		_hoveredButton = _guardianMaleButton;
	} else if (_guardianFemaleButton->isHovered() && _hoveredButton != _guardianFemaleButton) {
		_labelDesc->setText(_guardianDescription);
		_labelTitle->setText(_guardianFemaleTitle);
		_hoveredButton = _guardianFemaleButton;
	}
}

void ClassSelection::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_SEL1") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _consularMaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}
	if (widget.getTag() == "BTN_SEL2") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _guardianMaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}
	if (widget.getTag() == "BTN_SEL3") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _sentinelMaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}
	if (widget.getTag() == "BTN_SEL4") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _sentinelFemaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}
	if (widget.getTag() == "BTN_SEL5") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _guardianFemaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}
	if (widget.getTag() == "BTN_SEL6") {
		_chargen = std::make_unique<CharacterGeneration>(_module, _consularFemaleInfo);
		if (sub(*_chargen) == 2) {
			_returnCode = 2;
		}
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		_returnCode = 1;
		return;
	}
}

void ClassSelection::initWidget(Widget &widget) {
	KotOR2::initWidget(widget);
}

} // End of namespace KotOR2

} // End of namespace Engines
