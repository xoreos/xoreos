/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/door.cpp
 *  KotOR door.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model_kotor.h"

#include "engines/aurora/util.h"

#include "engines/kotor/door.h"

namespace Engines {

namespace KotOR {

Door::Door(const Aurora::GFFStruct &door) : Situated(kObjectTypeDoor),
	_invisible(false), _genericType(Aurora::kFieldIDInvalid), _state(kStateClosed) {

	load(door);
}

Door::~Door() {
}

void Door::setVisible(bool visible) {
	if (visible)
		setModelState();

	Situated::setVisible(visible);
}

void Door::load(const Aurora::GFFStruct &door) {
	Common::UString temp = door.getString("TemplateResRef");

	Aurora::GFFFile *utd = 0;
	if (!temp.empty()) {
		try {
			utd = new Aurora::GFFFile(temp, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' '));
		} catch (...) {
			delete utd;
		}
	}

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	delete utd;
}

void Door::loadObject(const Aurora::GFFStruct &gff) {
	// Generic type

	_genericType = gff.getUint("GenericType", _genericType);

	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);
}

void Door::loadAppearance() {
	if (_appearanceID == 0) {
		if (_genericType == Aurora::kFieldIDInvalid)
			_invisible = true;
		else
			loadAppearance(TwoDAReg.get("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get("doortypes"), _appearanceID);

	// Invisible doors have no model and are always open
	if (_invisible) {
		_modelName.clear();
		_state = kStateOpened1;
	}
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id) {
	uint32 modelColumn = twoda.headerToColumn("ModelName");
	if (modelColumn == Aurora::kFieldIDInvalid)
		modelColumn = twoda.headerToColumn("Model");

	_invisible    = twoda.getRow(id).getInt("VisibleModel") == 0;
	_modelName    = twoda.getRow(id).getString(modelColumn);
	_soundAppType = twoda.getRow(id).getInt("SoundAppType");
}

void Door::setLocked(bool locked) {
	if (isLocked() == locked)
		return;

	Situated::setLocked(locked);
}

bool Door::open(Object *opener) {
	// TODO: Door::open(): Open in direction of the opener

	if (isOpen())
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		return false;
	}

	_state = kStateOpened1;

	playSound(_soundOpened);

	return true;
}

bool Door::close(Object *closer) {
	if (!isOpen())
		return true;

	if (_invisible)
		return false;

	_state = kStateClosed;

	playSound(_soundClosed);

	return true;
}

bool Door::isOpen() const {
	return (_state == kStateOpened1) || (_state == kStateOpened2);
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

		default:
			_model->setState("");
			break;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
