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

/** @file engines/kotor/object.h
 *  An object within a KotOR area.
 */

#include <OgreMath.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include "common/util.h"
#include "common/error.h"

#include "aurora/ssffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/dlgfile.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"

#include "engines/kotor/types.h"
#include "engines/kotor/object.h"

namespace Engines {

namespace KotOR {

Object::Object(ObjectType type) : _type(type),
	_soundSet(Aurora::kFieldIDInvalid), _ssf(0), _static(false), _usable(true), _highlight(false) {

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 1.0;
	_orientation[2] = 0.0;
	_orientation[3] = 0.0;
}

Object::~Object() {
	delete _ssf;
}

ObjectType Object::getType() const {
	return _type;
}

void Object::loadModel() {
}

void Object::unloadModel() {
}

bool Object::isVisible() const {
	return _visible;
}

void Object::setVisible(bool visible) {
	_visible = visible;
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

const Common::UString &Object::getConversation() const {
	return _conversation;
}

const Aurora::SSFFile *Object::getSSF() {
	loadSSF();

	return _ssf;
}

const std::vector<Common::UString> &Object::getModelIDs() const {
	return _modelIDs;
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

bool Object::getHighlight() const {
	return _highlight;
}

void Object::setHighlight(bool highlight) {
	_highlight = highlight;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &radian, float &x, float &y, float &z) const {
	radian = _orientation[0];
	x      = _orientation[1];
	y      = _orientation[2];
	z      = _orientation[3];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float radian, float x, float y, float z) {
	_orientation[0] = radian;
	_orientation[1] = x;
	_orientation[2] = y;
	_orientation[3] = z;
}

void Object::move(float x, float y, float z) {
	_position[0] += x;
	_position[1] += y;
	_position[2] += z;
}

void Object::rotate(float radian, float x, float y, float z) {
	Ogre::Quaternion cQ(Ogre::Radian(_orientation[0]), Ogre::Vector3(_orientation[1], _orientation[2], _orientation[3]));
	Ogre::Quaternion nQ(Ogre::Radian(radian), Ogre::Vector3(x, y, z));

	nQ.normalise();

	const Ogre::Quaternion nM = cQ * nQ;

	Ogre::Radian  newAngle;
	Ogre::Vector3 newAxis;

	nM.ToAngleAxis(newAngle, newAxis);

	_orientation[0] = newAngle.valueRadians();
	_orientation[1] = newAxis.x;
	_orientation[2] = newAxis.y;
	_orientation[3] = newAxis.z;
}

void Object::loadSSF() {
	if (_ssf || (_soundSet == Aurora::kFieldIDInvalid))
		return;

	const Aurora::TwoDAFile &soundSets = TwoDAReg.get("soundset");

	Common::UString ssfFile = soundSets.getRow(_soundSet).getString("RESREF");
	if (ssfFile.empty())
		return;

	try {
		_ssf = new Aurora::SSFFile(ssfFile);
	} catch (...) {
		warning("Failed to load SSF \"%s\" (object \"%s\")", ssfFile.c_str(), _tag.c_str());
		delete _ssf;
		_ssf = 0;
	}
}

void Object::stopSound() {
	SoundMan.stopChannel(_sound);
}

void Object::playSound(const Common::UString &sound, bool pitchVariance) {
	stopSound();
	if (sound.empty())
		return;

	_sound = ::Engines::playSound(sound, Sound::kSoundTypeVoice, false, 1.0, pitchVariance);
}

} // End of namespace KotOR

} // End of namespace Engines
