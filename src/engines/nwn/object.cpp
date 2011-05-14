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

/** @file engines/nwn/object.h
 *  An object within a NWN area.
 */

#include "common/util.h"

#include "aurora/dlgfile.h"

#include "engines/aurora/util.h"

#include "engines/nwn/object.h"

namespace Engines {

namespace NWN {

Object::Object() : _dlg(0) {
	clear();
}

Object::~Object() {
}

const Common::UString &Object::getTag() const {
	return _tag;
}

const Common::UString &Object::getName() const {
	return _name;
}

const Common::UString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getPortrait() const {
	return _portrait;
}

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

const std::list<uint32> &Object::getIDs() const {
	return _ids;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
}

void Object::clear() {
	_loaded = false;

	_tag.clear();
	_name.clear();
	_description.clear();

	_portrait.clear();

	_conversation.clear();

	delete _dlg;
	_dlg = 0;

	_static = false;
	_usable = true;

	_ids.clear();

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

void Object::loadDLG() {
	if (_dlg || _conversation.empty())
		return;

	try {
		_dlg = new Aurora::DLGFile(_conversation);
	} catch (...) {
		warning("Failed to load DLG \"%s\" (object \"%s\")", _conversation.c_str(), _tag.c_str());
		delete _dlg;
		_dlg = 0;
	}
}

void Object::click() {
	Common::UString text, sound;

	if (_dlg)
		_dlg->getStart(text, sound);

	if (!text.empty())
		status("%s: \"%s\"", _name.c_str(), text.c_str());
	if (!sound.empty())
		playSound(sound, Sound::kSoundTypeVoice);
}

} // End of namespace NWN

} // End of namespace Engines
