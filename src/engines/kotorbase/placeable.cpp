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
 *  Placeable within an area in KotOR games.
 */

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/placeable.h"

namespace Engines {

namespace KotORBase {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) :
		Situated(kObjectTypePlaceable),
		_state(kStateDefault),
		_hasInventory(false) {

	load(placeable);
}

void Placeable::load(const Aurora::GFF3Struct &placeable) {
	_templateResRef = placeable.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utp;
	if (!_templateResRef.empty())
		utp.reset(loadOptionalGFF3(_templateResRef, Aurora::kFileTypeUTP, MKTAG('U', 'T', 'P', ' ')));

	Situated::load(placeable, utp ? &utp->getTopLevel() : 0);

	if (!utp)
		warning("Placeable \"%s\" has no blueprint", _tag.c_str());

	readScripts(utp->getTopLevel());
}

void Placeable::hide() {
	leave();

	Situated::hide();
}

void Placeable::loadObject(const Aurora::GFF3Struct &gff) {
	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);

	// Inventory

	_hasInventory = gff.getBool("HasInventory", _hasInventory);

	if (_hasInventory && gff.hasField("ItemList")) {
		Aurora::GFF3List classList = gff.getList("ItemList");
		for (Aurora::GFF3List::const_iterator iter = classList.begin(); iter != classList.end(); ++iter) {
			const Aurora::GFF3Struct &item = **iter;
			_inventory.addItem(item.getString("InventoryRes"));
		}
	}

	// Hit Points

	_currentHitPoints = gff.getSint("CurrentHP");
	_maxHitPoints = gff.getSint("HP");

	_minOneHitPoint = gff.getBool("Min1HP");
}

void Placeable::loadAppearance() {
	if (_appearanceID == Aurora::kFieldIDInvalid)
		return;

	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("placeables");

	_modelName    = twoda.getRow(_appearanceID).getString("ModelName");
	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
}

const Common::UString &Placeable::getCursor() const {
	static Common::UString cursor("use");
	return cursor;
}

void Placeable::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

bool Placeable::isOpen() const {
	return (_state == kStateOpen) || (_state == kStateActivated);
}

bool Placeable::isActivated() const {
	return isOpen();
}

bool Placeable::click(Object *triggerer) {
	CursorMan.set();

	// If the placeable is locked, just play the appropriate sound and bail
	if (isLocked()) {
		playSound(_soundLocked);
		return false;
	}

	// If the object was destroyed, nothing more can be done with it
	if (_state == kStateDestroyed)
		return true;

	_lastUsedBy = triggerer;

	// Objects with an inventory toggle between open and closed
	if (_hasInventory) {
		if (isOpen())
			return close(triggerer);

		return open(triggerer);
	}

	// Objects without an inventory toggle between activated and deactivated
	if (isActivated())
		return deactivate(triggerer);

	return activate(triggerer);
}

bool Placeable::open(Object *opener) {
	if (!_hasInventory)
		return false;

	if (isOpen())
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		return false;
	}

	_lastOpenedBy = opener;

	playSound(_soundOpened);
	runScript(kScriptOpen, this, opener);

	_state = kStateOpen;

	return true;
}

bool Placeable::close(Object *closer) {
	if (!_hasInventory)
		return false;

	if (!isOpen())
		return true;

	_lastClosedBy = closer;

	playSound(_soundClosed);
	runScript(kScriptClosed, this, closer);

	_state = kStateClosed;

	return true;
}

bool Placeable::activate(Object *user) {
	if (_hasInventory)
		return false;

	if (isActivated())
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		return false;
	}

	playSound(_soundUsed);
	runScript(kScriptUsed, this, user);

	_state = kStateActivated;

	return true;
}

bool Placeable::deactivate(Object *user) {
	if (_hasInventory)
		return false;

	if (!isActivated())
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		return false;
	}

	playSound(_soundUsed);
	runScript(kScriptUsed, this, user);

	_state = kStateDeactivated;

	return true;
}

bool Placeable::unlock(Object *UNUSED(unlocker)) {
	if (_locked) {
		_locked = false;
		return true;
	}

	return false;
}

bool Placeable::hasInventory() {
	return _hasInventory;
}

Inventory &Placeable::getInventory() {
	return _inventory;
}

} // End of namespace KotORBase

} // End of namespace Engines
