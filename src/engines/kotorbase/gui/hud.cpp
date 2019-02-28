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
 *  Base in-game HUD for KotOR games.
 */

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/progressbar.h"

#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/gui/hud.h"

namespace Engines {

namespace KotORBase {

HUD::HUD(Module &module, Console *console) :
		GUI(console),
		_module(module),
		_hoveredCircle(new SelectionCircle()),
		_targetCircle(new SelectionCircle()) {

	_hoveredCircle->setHovered(true);
	_targetCircle->setTarget(true);

	_targetActionMap[0] = _targetActionMap[1] = _targetActionMap[2] = kActionInvalid;
}

void HUD::setMinimap(const Common::UString &UNUSED(map), int UNUSED(northAxis),
                     float UNUSED(worldPt1X), float UNUSED(worldPt1Y), float UNUSED(worldPt2X), float UNUSED(worldPt2Y),
                     float UNUSED(mapPt1X), float UNUSED(mapPt1Y), float UNUSED(mapPt2X), float UNUSED(mapPt2Y)) {
}

void HUD::setPosition(float UNUSED(x), float UNUSED(y)) {
}

void HUD::setRotation(float UNUSED(angle)) {
}

void HUD::setReturnStrref(uint32 UNUSED(id)) {
}

void HUD::setReturnQueryStrref(uint32 UNUSED(id)) {
}

void HUD::setReturnEnabled(bool UNUSED(enabled)) {
}

void HUD::showContainer(Inventory &UNUSED(inv)) {
}

void HUD::setPartyLeader(Creature *UNUSED(creature)) {
}

void HUD::setPartyMember1(Creature *UNUSED(creature)) {
}

void HUD::setPartyMember2(Creature *UNUSED(creature)) {
}

Object *HUD::getHoveredObject() const {
	return _hoveredObject;
}

Object *HUD::getTargetObject() const {
	return _targetObject;
}

void HUD::setHoveredObject(Object *object) {
	_hoveredObject = object;
}

void HUD::setTargetObject(Object *object) {
	_targetObject = object;
}

void HUD::resetSelection() {
	_hoveredObject = nullptr;
	_targetObject = nullptr;
}

void HUD::updateSelection() {
	if (_targetObject) {
		_targetCircle->setHovered(_hoveredObject == _targetObject);

		Situated *situated = ObjectContainer::toSituated(_targetObject);
		if (situated) {
			float sX, sY;
			_targetCircle->moveTo(situated, sX, sY);
			_targetCircle->show();
			updateTargetInformation(_targetObject, sX, sY);
			showTargetInformation(_targetObject);
		} else {
			_targetCircle->hide();
			hideTargetInformation();
		}
	} else {
		_targetCircle->hide();
		hideTargetInformation();
	}

	if (_hoveredObject && (_hoveredObject != _targetObject)) {
		Situated *situated = ObjectContainer::toSituated(_hoveredObject);
		if (situated) {
			float _, __;
			_hoveredCircle->moveTo(situated, _, __);
			_hoveredCircle->show();
		} else {
			_hoveredCircle->hide();
		}
	} else {
		_hoveredCircle->hide();
	}
}

void HUD::hideSelection() {
	_hoveredCircle->hide();
	_targetCircle->hide();
	hideTargetInformation();
}

void HUD::init() {
	_targetName = getLabel("LBL_NAME");
	_targetNameBackground = getLabel("LBL_NAMEBG");
	_targetHealth = getProgressbar("PB_HEALTH");
	_targetHealthBackground = getLabel("LBL_HEALTHBG");
	_firstTargetButton = getButton("BTN_TARGET0");
	_secondTargetButton = getButton("BTN_TARGET1");
	_thirdTargetButton = getButton("BTN_TARGET2");
}

void HUD::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_TARGET0") {
		return;
	}
	if (widget.getTag() == "BTN_TARGET1") {
		if (_targetActionMap[1] == kActionOpenLock) {
			Action action(kActionOpenLock);
			action.object = _targetObject;
			_module.getPartyLeader()->enqueueAction(action);
		}
		return;
	}
	if (widget.getTag() == "BTN_TARGET2") {
		return;
	}
}

void HUD::getTargetButtonSize(float &width, float &height) const {
	if (!_firstTargetButton)
		return;

	width = _firstTargetButton->getWidth();
	height = _firstTargetButton->getHeight();
}

float HUD::getTargetButtonsDistance() const {
	if (!_firstTargetButton || !_secondTargetButton)
		throw Common::Exception("HUD::getTargetButtonsDistance(): Buttons not initialized");

	float x1, x2, _, __;
	_firstTargetButton->getPosition(x1, _, __);
	_secondTargetButton->getPosition(x2, _, __);

	return x2 - x1;
}

void HUD::positionTargetButtons(float originX, float originY) {
	if (!_firstTargetButton || !_secondTargetButton || !_thirdTargetButton)
		return;

	float dist = getTargetButtonsDistance();

	float width = 0.0f, _;
	getTargetButtonSize(width, _);
	float halfWidth = width / 2.0f;

	float halfSelectionSize = kSelectionCircleSize / 2.0f;
	float y = originY + halfSelectionSize + 1.0f;

	_firstTargetButton->setPosition(originX - dist - halfWidth, y, -100.0f);
	_secondTargetButton->setPosition(originX - halfWidth, y, -100.0f);
	_thirdTargetButton->setPosition(originX + dist - halfWidth, y, -100.0f);
}

void HUD::updateTargetActions() {
	_targetActionMap[0] = _targetActionMap[1] = _targetActionMap[2] = kActionInvalid;

	for (int action : _targetObject->getPossibleActions()) {
		switch (action) {
			case kActionOpenLock:
				_targetActionMap[1] = kActionOpenLock;
				break;
			default:
				break;
		}
	}

	if (_targetActionMap[1] == kActionOpenLock)
		_secondTargetButton->setIcon("isk_security");
	else
		_secondTargetButton->setIcon("");
}

void HUD::showTargetInformation(Object *object) {
	if (_targetNameBackground) {
		_targetNameBackground->setInvisible(false);
		_targetNameBackground->show();
	}
	if (_targetName) {
		_targetName->setInvisible(false);
		_targetName->show();
		_targetName->setText(object->getName());
	}

	if (_targetHealthBackground) {
		_targetHealthBackground->setInvisible(false);
		_targetHealthBackground->show();
	}
	if (_targetHealth) {
		_targetHealth->setInvisible(false);
		_targetHealth->show();
	}

	if (object->getPossibleActions().empty())
		return;

	if (_firstTargetButton) {
		_firstTargetButton->setInvisible(false);
		_firstTargetButton->show();
	}
	if (_secondTargetButton) {
		_secondTargetButton->setInvisible(false);
		_secondTargetButton->show();
	}
	if (_thirdTargetButton) {
		_thirdTargetButton->setInvisible(false);
		_thirdTargetButton->show();
	}
}

void HUD::updateTargetInformation(KotORBase::Object *object, float x, float y) {
	float halfSelectionSize = kSelectionCircleSize / 2.0f;
	float elementY = y + halfSelectionSize + 1.0f;

	if (!object->getPossibleActions().empty()) {
		float _, targetBtnHeight = 0.0f;
		getTargetButtonSize(_, targetBtnHeight);
		elementY += targetBtnHeight + 1.0f;
	}

	if (_targetHealthBackground)
		_targetHealthBackground->setPosition(x - 100, elementY, -FLT_MAX);

	if (_targetHealth) {
		_targetHealth->setPosition(x - 100, elementY, -FLT_MAX);

		_targetHealth->setMaxValue(object->getMaxHitPoints());
		_targetHealth->setCurrentValue(object->getCurrentHitPoints());
	}

	elementY += _targetHealth->getHeight() + 1.0f;

	if (_targetNameBackground)
		_targetNameBackground->setPosition(x - 100, elementY, -100);

	if (_targetName)
		_targetName->setPosition(x - 100, elementY, -FLT_MAX);

	positionTargetButtons(x, y);
	updateTargetActions();
}

void HUD::hideTargetInformation() {
	if (_targetNameBackground) {
		_targetNameBackground->setInvisible(true);
		_targetNameBackground->hide();
	}
	if (_targetName) {
		_targetName->setInvisible(true);
		_targetName->hide();
	}

	if (_targetHealthBackground) {
		_targetHealthBackground->setInvisible(true);
		_targetHealthBackground->hide();
	}
	if (_targetHealth) {
		_targetHealth->setInvisible(true);
		_targetHealth->hide();
	}

	if (_firstTargetButton) {
		_firstTargetButton->setInvisible(true);
		_firstTargetButton->hide();
	}
	if (_secondTargetButton) {
		_secondTargetButton->setInvisible(true);
		_secondTargetButton->hide();
	}
	if (_thirdTargetButton) {
		_thirdTargetButton->setInvisible(true);
		_thirdTargetButton->hide();
	}
}

} // End of namespace KotORBase

} // End of namespace Engines
