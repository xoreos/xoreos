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

/** @file events/joystick.cpp
 *  Joystick/Gamepad handling.
 */

#include "events/joystick.h"

namespace Events {

Joystick::Joystick(int index) : _index(index), _sdlJoy(0) {
	assert(_index >= 0);
}

Joystick::~Joystick() {
	disable();
}

int Joystick::getIndex() const {
	return _index;
}

const Common::UString &Joystick::getName() const {
	return _name;
}

bool Joystick::isEnabled() const {
	return _sdlJoy != 0;
}

bool Joystick::enable() {
	if (isEnabled())
		return true;

	_sdlJoy = SDL_JoystickOpen(_index);
	if (!_sdlJoy)
		return false;

	_name = SDL_JoystickName(_sdlJoy);

	return true;
}

void Joystick::disable() {
	if (_sdlJoy)
		SDL_JoystickClose(_sdlJoy);
}

int Joystick::getAxisCount() const {
	if (!_sdlJoy)
		return 0;

	return SDL_JoystickNumAxes(_sdlJoy);
}

int Joystick::getBallCount() const {
	if (!_sdlJoy)
		return 0;

	return SDL_JoystickNumBalls(_sdlJoy);
}

int Joystick::getHatCount() const {
	if (!_sdlJoy)
		return 0;

	return SDL_JoystickNumHats(_sdlJoy);
}

int Joystick::getButtonCount() const {
	if (!_sdlJoy)
		return 0;

	return SDL_JoystickNumButtons(_sdlJoy);
}

} // End of namespace Events
