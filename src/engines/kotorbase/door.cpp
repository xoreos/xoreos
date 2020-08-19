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
 *  Door within an area in KotOR games.
 */

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include <memory>
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/door.h"
#include "src/engines/kotorbase/module.h"

namespace Engines {

namespace KotORBase {

Door::Door(Module &module, const Aurora::GFF3Struct &door) :
		Situated(kObjectTypeDoor),
		_module(&module),
		_genericType(Aurora::kFieldIDInvalid),
		_state(kStateClosed),
		_linkedToFlag(kLinkedToNothing),
		_linkedToType(kObjectTypeAll) {

	load(door);
}

void Door::load(const Aurora::GFF3Struct &door) {
	_templateResRef = door.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> utd;
	if (!_templateResRef.empty())
		utd.reset(loadOptionalGFF3(_templateResRef, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' ')));

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	if (!utd)
		warning("Door \"%s\" has no blueprint", _tag.c_str());
}

void Door::loadObject(const Aurora::GFF3Struct &gff) {
	_genericType = gff.getUint("GenericType", _genericType);

	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);

	// Linked to

	_linkedToFlag   = (LinkedToFlag) gff.getUint("LinkedToFlags", (uint) _linkedToFlag);
	_linkedTo       = gff.getString("LinkedTo", _linkedTo);
	_linkedToModule = gff.getString("LinkedToModule", _linkedToModule);

	_transitionDestination = gff.getString("TransitionDestin", _transitionDestination);

	_linkedToType = (ObjectType) (kObjectTypeDoor | kObjectTypeWaypoint);
}

void Door::loadAppearance() {
	if (_appearanceID == 0) {
		if (_genericType == Aurora::kFieldIDInvalid)
			throw Common::Exception("Door \"%s\" has no appearance ID and no generic type",
			                        _tag.c_str());

		loadAppearance(TwoDAReg.get2DA("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get2DA("doortypes"), _appearanceID);
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32_t id) {
	if (_appearanceID == Aurora::kFieldIDInvalid)
		return;

	uint32_t column = twoda.headerToColumn("ModelName");
	if (column == Aurora::kFieldIDInvalid)
		column = twoda.headerToColumn("Model");

	_modelName    = twoda.getRow(id).getString(column);
	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
}

void Door::hide() {
	leave();

	Situated::hide();
}

void Door::notifyNotSeen() {
}

const Common::UString &Door::getCursor() const {
	static Common::UString cursor("door");
	return cursor;
}

void Door::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

bool Door::isOpen() const {
	return (_state == kStateOpened1) || (_state == kStateOpened2);
}

bool Door::click(Object *triggerer) {
	_lastUsedBy = triggerer;

	// If the door is closed, try to open it
	if (!isOpen())
		return open(triggerer);

	// If the door is open and has a click script, call that
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	if (!_linkedTo.empty()) {
		_module->moveParty(_linkedToModule, _linkedTo, _linkedToType);

		return true;
	}

	// If the door is open and has no script, close it
	return close(triggerer);
}

void Door::getTooltipAnchor(float &x, float &y, float &z) const {
	if (!_model) {
		Situated::getTooltipAnchor(x, y, z);
		return;
	}

	_model->getAbsolutePosition(x, y, z);
	z += _model->getDepth() / 2;
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

	if (_model)
		_model->playAnimation("opening1");

	playSound(_soundOpened);
	runScript(kScriptOpen, this, opener);
	setUsable(false);

	_state = kStateOpened1;

	return true;
}

bool Door::close(Object *closer) {
	if (!isOpen() || (_state == kStateDestroyed))
		return true;

	_lastClosedBy = closer;

	if (_model)
		_model->playAnimation("closing1");

	playSound(_soundClosed);
	runScript(kScriptClosed, this, closer);
	setUsable(true);

	_state = kStateClosed;

	return true;
}

bool Door::unlock(Object *unlocker) {
	if (_locked) {
		_locked = false;
		open(unlocker);
		return true;
	}

	return false;
}

} // End of namespace KotORBase

} // End of namespace Engines
