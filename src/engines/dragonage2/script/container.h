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
 *  An object containing scripts.
 */

#ifndef ENGINES_DRAGONAGE2_SCRIPT_CONTAINER_H
#define ENGINES_DRAGONAGE2_SCRIPT_CONTAINER_H

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/dragonage2/types.h"

namespace Aurora {
	namespace NWScript {
		class Object;
		struct ScriptState;
	}
}
namespace Engines {

namespace DragonAge2 {

class Event;

class ScriptContainer {
public:
	ScriptContainer();
	~ScriptContainer();

	const Common::UString &getScript() const;
	bool hasScript() const;

	/** Enable/Disable the handling of a specific event. */
	void enableEvent(EventType event, bool enabled);
	/** Enable/Disable the handling of all events. */
	void enableEvents(bool enabled);

	bool runScript(EventType event, Aurora::NWScript::Object *owner = 0,
	               Aurora::NWScript::Object *triggerer = 0);

	bool runScript(Event &event);

	static bool runScript(const Common::UString &script, EventType event,
	                      Aurora::NWScript::Object *owner = 0,
	                      Aurora::NWScript::Object *triggerer = 0);
	static bool runScript(const Common::UString &script, EventType event,
	                      const Aurora::NWScript::ScriptState &state,
	                      Aurora::NWScript::Object *owner = 0,
	                      Aurora::NWScript::Object *triggerer = 0);

	static bool runScript(const Common::UString &script, Event &event);
	static bool runScript(const Common::UString &script, Event &event,
	                      const Aurora::NWScript::ScriptState &state);


protected:
	void clearScript();

	void readScript(const Aurora::GFF3Struct &gff);
	void readScript(const Aurora::GFF4Struct &gff);

private:
	Common::UString _script;

	std::map<EventType, bool> _eventEnabled;
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_SCRIPT_CONTAINER_H
