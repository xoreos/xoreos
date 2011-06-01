/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/door.cpp
 *  NWN door.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn/door.h"

namespace Engines {

namespace NWN {

Door::Door() : Situated(kObjectTypeDoor) {
	clear();
}

Door::~Door() {
	clear();
}

void Door::clear() {
	Situated::clear();

	_genericType = Aurora::kFieldIDInvalid;

	_state = kStateClosed;
}

void Door::load(const Aurora::GFFStruct &door) {
	Common::UString temp = door.getString("TemplateResRef");

	Aurora::GFFFile *utd = 0;
	if (!temp.empty()) {
		try {
			utd = loadGFF(temp, Aurora::kFileTypeUTD, MKID_BE('UTD '));
		} catch (...) {
		}
	}

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	if (!utd)
		warning("Door \"%s\" has no blueprint", _tag.c_str());

	delete utd;

	setModelState();
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
			throw Common::Exception("Door \"%s\" has no appearance ID and no generic type",
			                        _tag.c_str());

		loadAppearance(TwoDAReg.get("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get("doortypes"), _appearanceID);
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id) {
	uint32 column = twoda.headerToColumn("ModelName");
	if (column == Aurora::kFieldIDInvalid)
		column = twoda.headerToColumn("Model");

	_modelName    = twoda.getRow(id).getString(column);
	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
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

void Door::hide() {
	leave();

	Situated::hide();
}

void Door::enter() {
	highlight(true);
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

bool Door::click(Object *triggerer) {
	// If the door is closed, try to open it
	if (!isOpen())
		return open(triggerer);

	// If the door is open and has a click script, call that
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	// TODO: Door::click(): LinkedTo

	// If the door is open and has no script, close it
	return close(triggerer);
}

bool Door::open(Object *opener) {
	// TODO: Door::open(): Animate
	// TODO: Door::open(): Open in direction of the opener

	if (isOpen())
		return true;

	if (isLocked()) {
		playSound(_soundLocked);
		runScript(kScriptFailToOpen, this, opener);
		return false;
	}

	_state = kStateOpened1;
	setModelState();

	playSound(_soundOpened);
	runScript(kScriptOpen, this, opener);
	return true;
}

bool Door::close(Object *closer) {
	// TODO: Door::close(): Animate

	if (!isOpen())
		return true;

	_state = kStateClosed;
	setModelState();

	playSound(_soundClosed);
	runScript(kScriptClosed, this, closer);
	return true;
}

bool Door::isOpen() const {
	return (_state == kStateOpened1) || (_state == kStateOpened2);
}

} // End of namespace NWN

} // End of namespace Engines
