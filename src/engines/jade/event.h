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
 *  A scripting event.
 */

#ifndef ENGINES_JADE_EVENT_H
#define ENGINES_JADE_EVENT_H

#include "src/common/types.h"

#include "src/aurora/nwscript/enginetype.h"

#include "src/engines/jade/types.h"

namespace Aurora {
	namespace NWScript {
		class Object;
	}
}

namespace Engines {

namespace Jade {

class Event : public Aurora::NWScript::EngineType {
public:
	Event(Script script, int32_t eventNumber = 0);
	~Event();

	Event *clone() const;

	Script getScript() const;
	int32_t getEventNumber() const;

private:
	Script _script;
	int32_t _eventNumber;
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_EVENT_H
