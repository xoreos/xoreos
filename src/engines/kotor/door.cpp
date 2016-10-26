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
 *  A door in a Star Wars: Knights of the Old Republic area.
 */

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor/door.h"
#include "src/engines/kotor/module.h"

namespace Engines {

namespace KotOR {

Door::Door(Module &module, const Aurora::GFF3Struct &door) : Situated(kObjectTypeDoor),
	_module(&module), _genericType(Aurora::kFieldIDInvalid), _state(kStateClosed),
	_linkedToFlag(kLinkedToNothing), _linkedToType(kObjectTypeAll) {

	load(door);
}

Door::~Door() {
}

void Door::load(const Aurora::GFF3Struct &door) {
	Common::UString temp = door.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utd;
	if (!temp.empty())
		utd.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' ')));

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

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id) {
	if (_appearanceID == Aurora::kFieldIDInvalid)
		return;

	uint32 column = twoda.headerToColumn("ModelName");
	if (column == Aurora::kFieldIDInvalid)
		column = twoda.headerToColumn("Model");

	_modelName    = twoda.getRow(id).getString(column);
	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
}

void Door::hide() {
	leave();

	Situated::hide();
}

void Door::enter() {
	highlight(true);
}

void Door::leave() {
	highlight(false);
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
		_module->movePC(_linkedToModule, _linkedTo, _linkedToType);

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

	playSound(_soundOpened);
	runScript(kScriptOpen, this, opener);

	_state = kStateOpened1;

	return true;
}

bool Door::close(Object *closer) {
	if (!isOpen() || (_state == kStateDestroyed))
		return true;

	_lastClosedBy = closer;

	playSound(_soundClosed);
	runScript(kScriptClosed, this, closer);

	_state = kStateClosed;

	return true;
}

} // End of namespace KotOR

} // End of namespace Engines
