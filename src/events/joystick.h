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
 *  Joystick/Gamepad handling.
 */

#ifndef EVENTS_JOYSTICK_H
#define EVENTS_JOYSTICK_H

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL_joystick.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include <boost/noncopyable.hpp>

#include "src/common/ustring.h"

namespace Events {

class Joystick : boost::noncopyable {
public:
	Joystick(int index);
	virtual ~Joystick();

	/** Return the joystick's name. */
	const Common::UString &getName() const;

	/** Return the joystick's index within the available joysticks. */
	int getIndex() const;

	/** Is the joystick currently enabled? */
	virtual bool isEnabled() const;
	/** Can this joystick be a game controller? */
	bool isController() const;

	/** Enable the joystick. */
	virtual bool enable();
	/** Disable the joystick. */
	virtual void disable();

	// Properties, only available when the joystick is enabled.

	/** Return the number of axes the joystick has. */
	int getAxisCount  () const;
	/** Return the number of balls the joystick has. */
	int getBallCount  () const;
	/** Return the number of hats the joystick has. */
	int getHatCount   () const;
	/** Return the number of buttons the joystick has. */
	int getButtonCount() const;

private:
	int _index;
	Common::UString _name;
	SDL_Joystick *_sdlJoy;
};

} // End of namespace Events

#endif // EVENTS_JOYSTICK_H
