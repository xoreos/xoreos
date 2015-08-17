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

#ifndef ENGINES_KOTOR_SCRIPT_CONTAINER_H
#define ENGINES_KOTOR_SCRIPT_CONTAINER_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/kotor/types.h"

namespace Aurora {
	namespace NWScript {
		class Object;
		struct ScriptState;
	}
}
namespace Engines {

namespace KotOR {

class ScriptContainer {
public:
	ScriptContainer();
	~ScriptContainer();

	const Common::UString &getScript(Script script) const;

	bool hasScript(Script script) const;

	bool runScript(Script script, Aurora::NWScript::Object *owner = 0,
	               Aurora::NWScript::Object *triggerer = 0);

	static bool runScript(const Common::UString &script,
	                      Aurora::NWScript::Object *owner = 0,
	                      Aurora::NWScript::Object *triggerer = 0);
	static bool runScript(const Common::UString &script,
	                      const Aurora::NWScript::ScriptState &state,
	                      Aurora::NWScript::Object *owner = 0,
	                      Aurora::NWScript::Object *triggerer = 0);

protected:
	void clearScripts();

	void readScripts(const Aurora::GFF3Struct &gff);
	void readScripts(const ScriptContainer &container);

private:
	Common::UString _scripts[kScriptMAX];
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_SCRIPT_CONTAINER_H
