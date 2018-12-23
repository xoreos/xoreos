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
 *  Game controller handling.
 */

#include "src/common/error.h"

#include "src/events/gamecontroller.h"

namespace Events {

GameController::GameController(int index) : Joystick(index), _sdlController(0) {
	if (!isController())
		throw Common::Exception("GameController::GameController() joystick %i is not a controller", getIndex());
}

GameController::~GameController() {
	if (_sdlController)
		SDL_GameControllerClose(_sdlController);
}

Common::UString GameController::getControllerName() const {
	return SDL_GameControllerName(_sdlController);
}

bool GameController::isEnabled() const {
	return Joystick::isEnabled() && _sdlController != 0;
}

bool GameController::enable() {
	if (isEnabled())
		return true;

	Joystick::enable();

	_sdlController = SDL_GameControllerOpen(getIndex());
	if (!_sdlController)
		return false;

	return true;
}

void GameController::disable() {
	Joystick::disable();

	if (_sdlController)
		SDL_GameControllerClose(_sdlController);

	_sdlController = 0;
}

}
