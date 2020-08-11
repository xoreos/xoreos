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
 *  The KotOR 2 character generation.
 */

#include "src/aurora/talkman.h"

#include "src/engines/odyssey/label.h"

#include "src/engines/kotor2/creature.h"

#include "src/engines/kotor2/gui/gui.h"

#include "src/engines/kotor2/gui/chargen/charactergeneration.h"
#include "src/engines/kotor2/gui/chargen/quickorcustom.h"
#include "src/engines/kotor2/gui/chargen/quickchar.h"
#include "src/engines/kotor2/gui/chargen/customchar.h"
#include "src/engines/kotor2/gui/chargen/chargenname.h"
#include "src/engines/kotor2/gui/chargen/chargenportrait.h"

namespace Engines {

namespace KotOR2 {

CharacterGeneration::CharacterGeneration(KotORBase::Module *module,
                                         CharacterGenerationInfo *info,
                                         Engines::Console *console) :
		KotORBase::GUI(console),
		_module(module),
		_chargenInfo(info),
		_step(0) {

	load("maincg_p");

	switch (_chargenInfo->getClass()) {
		case KotORBase::kClassJediConsular:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(354));
			break;
		case KotORBase::kClassJediGuardian:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(355));
			break;
		case KotORBase::kClassJediSentinel:
			getLabel("LBL_CLASS")->setText(TalkMan.getString(353));
			break;
		default:
			getLabel("LBL_CLASS")->setText("");
	}

	getLabel("LBL_NAME")->setText("");
	getLabel("LBL_LEVEL_VAL")->setText("");

	getLabel("PORTRAIT_LBL")->setFill(_chargenInfo->getPortrait());

	showQuickOrCustom();
}

void CharacterGeneration::showQuickOrCustom() {
	if (_quickCharPanel)
		removeChild(_quickCharPanel.get());
	if (_customCharPanel)
		removeChild(_customCharPanel.get());

	_quickOrCustomPanel = std::make_unique<QuickOrCustomPanel>(this);
	addChild(_quickOrCustomPanel.get());
}

void CharacterGeneration::showQuickChar() {
	removeChild(_quickOrCustomPanel.get());

	_quickCharPanel = std::make_unique<QuickCharPanel>(this);
	addChild(_quickCharPanel.get());
}

void CharacterGeneration::showCustomChar() {
	removeChild(_quickOrCustomPanel.get());

	_customCharPanel = std::make_unique<CustomCharPanel>(this);
	addChild(_customCharPanel.get());
}

void CharacterGeneration::showName() {
	CharacterGenerationInfo info = *_chargenInfo;

	_charGenMenu = std::make_unique<CharacterGenerationNameMenu>(info);
	sub(*_charGenMenu, kStartCodeNone, true, false);

	if (_charGenMenu->isAccepted()) {
		_step += 1;
		_chargenInfo->setName(info.getName());
		getLabel("LBL_NAME")->setText(_chargenInfo->getName());
	}
}

void CharacterGeneration::showPortrait() {
	CharacterGenerationInfo info = *_chargenInfo;

	_charGenMenu = std::make_unique<CharacterGenerationPortraitMenu>(info);
	sub(*_charGenMenu);

	if (_charGenMenu->isAccepted()) {
		_step += 1;
		_chargenInfo->setFace(info.getFace());
		_chargenInfo->setSkin(info.getSkin());
		getLabel("PORTRAIT_LBL")->setFill(_chargenInfo->getPortrait());
	}
}

int CharacterGeneration::getStep() {
	return _step;
}

void CharacterGeneration::decStep() {
	_step = MIN(0, _step - 1);
}

void CharacterGeneration::resetStep() {
	_step = 0;
}

void CharacterGeneration::start() {
	try {
		_module->usePC(*_chargenInfo);
		_module->load("001EBO");
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}

	_returnCode = 2;
}

void CharacterGeneration::initWidget(Widget &widget) {
	KotOR2::initWidget(widget);
}

} // End of namespace KotOR2

} // End of namespace Engines
