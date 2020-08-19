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
 *  A door in a Neverwinter Nights area.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/door.h"
#include "src/engines/nwn/waypoint.h"
#include "src/engines/nwn/module.h"

namespace Engines {

namespace NWN {

Door::Door(Module &module, const Aurora::GFF3Struct &door) : Situated(kObjectTypeDoor),
	_module(&module), _invisible(false), _genericType(Aurora::kFieldIDInvalid),
	_state(kStateClosed), _linkedToFlag(kLinkedToNothing), _evaluatedLink(false),
	_link(0), _linkedDoor(0), _linkedWaypoint(0) {

	load(door);
}

Door::~Door() {
}

void Door::load(const Aurora::GFF3Struct &door) {
	const Common::UString temp = door.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> utd;
	if (!temp.empty())
		utd.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' '), true));

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	setModelState();
}

void Door::loadObject(const Aurora::GFF3Struct &gff) {
	// Generic type

	_genericType = gff.getUint("GenericType", _genericType);

	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);

	// Linked to

	_linkedToFlag = (LinkedToFlag) gff.getUint("LinkedToFlags", (uint) _linkedToFlag);
	_linkedTo     = gff.getString("LinkedTo", _linkedTo);
}

void Door::loadAppearance() {
	if (_appearanceID == 0) {
		if (_genericType == Aurora::kFieldIDInvalid)
			_invisible = true;
		else
			loadAppearance(TwoDAReg.get2DA("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get2DA("doortypes"), _appearanceID);

	// Invisible doors have no model and are always open
	if (_invisible) {
		_modelName.clear();
		_state = kStateOpened1;
	}

	// Doors have a default, fallback portrait
	if (_portrait.empty())
		_portrait = "po_door01_";
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32_t id) {
	uint32_t modelColumn = twoda.headerToColumn("ModelName");
	if (modelColumn == Aurora::kFieldIDInvalid)
		modelColumn = twoda.headerToColumn("Model");

	_invisible    = twoda.getRow(id).getInt("VisibleModel") == 0;
	_modelName    = twoda.getRow(id).getString(modelColumn);
	_soundAppType = twoda.getRow(id).getInt("SoundAppType");
}

void Door::setModelState() {
	if (!_model)
		return;

	switch (_state) {
		case kStateClosed:
			_model->setState("closed");
			break;

		case kStateOpened1:
			_model->setState("opened1");
			break;

		case kStateOpened2:
			_model->setState("opened2");
			break;

		case kStateDestroyed:
			_model->setState("dead");
			break;

		default:
			_model->setState("");
			break;
	}

}

void Door::show() {
	setModelState();

	Situated::show();
}

void Door::hide() {
	leave();

	Situated::hide();
}

void Door::enter() {
	highlight(true);

	evaluateLink();
	if (isOpen() && _link)
		CursorMan.set("trans", "up");
	else
		CursorMan.set("door", "up");
}

void Door::leave() {
	highlight(false);
	CursorMan.set();
}

void Door::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

void Door::setLocked(bool locked) {
	if (isLocked() == locked)
		return;

	Situated::setLocked(locked);
	// Also lock/unlock the linked door
	evaluateLink();
	if (_linkedDoor)
		_linkedDoor->setLocked(locked);
}

bool Door::click(Object *triggerer) {
	_lastUsedBy = triggerer;

	// If the door is closed, try to open it
	if (!isOpen())
		return open(triggerer);

	// If the door is open and has a click script, call that
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	evaluateLink();
	if (_link) {
		float x, y, z;
		_link->getPosition(x, y, z);

		_module->movePC(_link->getArea(), x, y, z);
		return true;
	}

	// If the door is open and has no script, close it
	return close(triggerer);
}

bool Door::open(Object *opener) {
	// TODO: Door::open(): Open in direction of the opener

	if (isOpen() || (_state == kStateDestroyed))
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		runScript(kScriptFailToOpen, this, opener);
		return false;
	}

	_lastOpenedBy = opener;

	playAnimation(kAnimationDoorOpen1);
	runScript(kScriptOpen, this, opener);

	_state = kStateOpened1;

	// Also open the linked door
	evaluateLink();
	if (_linkedDoor)
		_linkedDoor->open(opener);

	return true;
}

bool Door::close(Object *closer) {
	if (!isOpen() || (_state == kStateDestroyed))
		return true;

	if (_invisible)
		return false;

	_lastClosedBy = closer;

	playAnimation(kAnimationDoorClose);
	runScript(kScriptClosed, this, closer);

	_state = kStateClosed;

	// Also close the linked door
	evaluateLink();
	if (_linkedDoor)
		_linkedDoor->close(closer);

	return true;
}

bool Door::isOpen() const {
	return (_state == kStateOpened1) || (_state == kStateOpened2);
}

void Door::evaluateLink() {
	if (_evaluatedLink)
		return;

	if ((_linkedToFlag != 0) && !_linkedTo.empty()) {
		Aurora::NWScript::Object *object = _module->getFirstObjectByTag(_linkedTo);

		if      (_linkedToFlag == kLinkedToDoor)
			_link = _linkedDoor     = dynamic_cast<Door *>(object);
		else if (_linkedToFlag == kLinkedToWaypoint)
			_link = _linkedWaypoint = dynamic_cast<Waypoint *>(object);
	}

	_evaluatedLink = true;
}

const Common::UString &Door::getAnimationSound(Animation animation) {
	static const Common::UString kEmptySound;

	switch (animation) {
		case kAnimationDoorClose:
			return _soundClosed;

		case kAnimationDoorOpen1:
			return _soundOpened;

		case kAnimationDoorOpen2:
			return _soundOpened;

		case kAnimationDoorDestroy:
			return _soundDestroyed;

		default:
			break;
	}

	return kEmptySound;
}

void Door::playAnimation(const Common::UString &animation, bool restart, float length, float speed) {
	Situated::playAnimation(animation, restart, length, speed);
}

void Door::playAnimation(Animation animation, bool restart, float length, float speed) {
	if (!isAnimationLooping(animation))
		length = 0.0f;

	const bool alternate = _state == kStateOpened2;

	const Common::UString  anim  = getDoorAnimationName(animation, alternate);
	const Common::UString &sound = getAnimationSound(animation);

	playSound(sound);
	if (_model)
		_model->playAnimation(anim, restart, length, speed);
}

} // End of namespace NWN

} // End of namespace Engines
