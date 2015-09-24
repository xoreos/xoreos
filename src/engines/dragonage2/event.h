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

#ifndef ENGINES_DRAGONAGE_EVENT_H
#define ENGINES_DRAGONAGE_EVENT_H

#include "src/aurora/nwscript/enginetype.h"
#include "src/aurora/nwscript/variablecontainer.h"

#include "src/engines/dragonage2/types.h"

namespace Aurora {
	namespace NWScript {
		class Object;
	}
}

namespace Engines {

namespace DragonAge2 {

class Event : public Aurora::NWScript::EngineType, public Aurora::NWScript::VariableContainer {
public:
	Event(EventType type = kEventTypeInvalid,
	      Aurora::NWScript::Object *creator = 0, Aurora::NWScript::Object *target = 0);
	~Event();

	Event *clone() const;

	EventType getType() const;
	void setType(EventType type);

	Aurora::NWScript::Object *getCreator() const;
	void setCreator(Aurora::NWScript::Object *creator);

	Aurora::NWScript::Object *getTarget() const;
	void setTarget(Aurora::NWScript::Object *target);

private:
	EventType _type;

	Aurora::NWScript::Object *_creator;
	Aurora::NWScript::Object *_target;
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_EVENT_H
