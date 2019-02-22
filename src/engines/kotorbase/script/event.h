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
 *  Event engine type for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_SCRIPT_EVENT_H
#define ENGINES_KOTORBASE_SCRIPT_EVENT_H

#include "src/aurora/nwscript/enginetype.h"

namespace Engines {

namespace KotORBase {

enum EventType {
	kEventUserDefined = 0
};

class Event : public Aurora::NWScript::EngineType {
public:
	Event(EventType type);

	EventType getType() const;
	int getUserDefinedNumber() const;

	void setUserDefinedNumber(int number);

	Aurora::NWScript::EngineType *clone() const;

private:
	EventType _type;
	int _userDefinedNumber;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_SCRIPT_EVENT_H
