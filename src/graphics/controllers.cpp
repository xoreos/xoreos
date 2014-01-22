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

/** @file graphics/controllers.cpp
 *  Basic animation controllers.
 */

#include "graphics/controllers.h"
#include "graphics/materialman.h"

namespace Graphics {

AnimationControllerFunction::AnimationControllerFunction(Ogre::Real sequenceTime, Ogre::Real offset,
		AnimationFunction function, bool loop) : Ogre::ControllerFunction<Ogre::Real>(false),
	_sequenceTime(sequenceTime), _currentTime(offset), _function(function), _loop(loop), _ended(false) {

}

AnimationControllerFunction::~AnimationControllerFunction() {
}

Ogre::Real AnimationControllerFunction::calculate(Ogre::Real source) {
	if (_ended)
		return doFunction();

	//Assume source is time since last update
	_currentTime += source;

	if (_sequenceTime == 0.0) {

		_ended       = true;
		_currentTime = 1.0;

	} else if (_currentTime >= _sequenceTime) {

		if (!_loop) {
			_ended       = true;
			_currentTime = 1.0;
		} else
			while (_currentTime >= _sequenceTime)
				_currentTime -= _sequenceTime;


	} else if (_currentTime < 0.0) {

		if (!_loop) {
			_ended       = true;
			_currentTime = 0.0;
		} else
			while (_currentTime < 0.0)
				_currentTime += _sequenceTime;

	}

	return doFunction();
}

Ogre::Real AnimationControllerFunction::doFunction() {
	Ogre::Real fraction = (_sequenceTime == 0.0) ? 1.0 : (_currentTime / _sequenceTime);

	switch (_function) {
		case kAnimationFunctionIncrease:
			return fraction;

		case kAnimationFunctionDecrease:
			return 1.0 - fraction;

		case kAnimationFunctionIncreaseDecrease:
			return (fraction <= 0.5) ? (fraction * 2) : ((1.0 - fraction) * 2);

		case kAnimationFunctionDecreaseIncrease:
			return 1.0 - ((fraction <= 0.5) ? (fraction * 2) : ((1.0 - fraction) * 2));

		default:
			break;
	}

	return 0.0;
}

void AnimationControllerFunction::setTime(Ogre::Real timeVal) {
	_currentTime = timeVal;
}

void AnimationControllerFunction::setSequenceTime(Ogre::Real seqVal) {
	_sequenceTime = seqVal;
}


MaterialAlphaControllerValue::MaterialAlphaControllerValue(Ogre::MaterialPtr material) {
	_materials.push_back(material);
}

MaterialAlphaControllerValue::MaterialAlphaControllerValue(std::list<Ogre::MaterialPtr> materials) : _materials(materials) {
}

MaterialAlphaControllerValue::~MaterialAlphaControllerValue() {
}

Ogre::Real MaterialAlphaControllerValue::getValue(void) const {
	// Not implemented; we don't need to use this value as a source

	return 0.0;
}

void MaterialAlphaControllerValue::setValue(Ogre::Real value) {
	for (std::list<Ogre::MaterialPtr>::iterator m = _materials.begin(); m != _materials.end(); ++m)
		MaterialMan.setAlphaModifier(*m, value);
}

} // End of namespace Graphics
