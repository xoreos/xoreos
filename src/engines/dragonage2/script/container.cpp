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

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/gff4fields.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/ncsfile.h"

#include "src/engines/dragonage2/event.h"

#include "src/engines/dragonage2/script/container.h"

namespace Engines {

namespace DragonAge2 {

using namespace ::Aurora::GFF4FieldNamesEnum;

static const char * const kScriptNames[] = {
	"EventScript",
	"Script",
	"ScriptResRefID"
};

static const GFF4FieldNames kScriptFields[] = {
	kGFF4CampaignCIFEntryClientScript,
	kGFF4CampaignCIFEntryScript
};

ScriptContainer::ScriptContainer() {
}

ScriptContainer::~ScriptContainer() {
}

const Common::UString &ScriptContainer::getScript() const {
	return _script;
}

bool ScriptContainer::hasScript() const {
	return !_script.empty();
}

void ScriptContainer::enableEvent(EventType event, bool enabled) {
	_eventEnabled[event] = enabled;
}

void ScriptContainer::enableEvents(bool enabled) {
	_eventEnabled[kEventTypeInvalid] = false;

	for (size_t i = (int) kEventTypeSpellCastAt; i < kEventTypeAOETargetingGUIOpened; i++)
		_eventEnabled[(EventType) i] = enabled;
}

void ScriptContainer::clearScript() {
	_script.clear();
}

void ScriptContainer::readScript(const Aurora::GFF3Struct &gff) {
	clearScript();

	for (size_t i = 0; i < ARRAYSIZE(kScriptNames); i++)
		_script = gff.getString(kScriptNames[i], _script);
}

void ScriptContainer::readScript(const Aurora::GFF4Struct &gff) {
	clearScript();

	for (size_t i = 0; i < ARRAYSIZE(kScriptFields); i++)
		_script = gff.getString(kScriptFields[i], _script);
}

bool ScriptContainer::runScript(EventType event, Aurora::NWScript::Object *owner,
                                Aurora::NWScript::Object *triggerer) {

	Event e(event, triggerer, owner);

	return runScript(e);
}

bool ScriptContainer::runScript(Event &event) {
	std::map<EventType, bool>::const_iterator enabled = _eventEnabled.find(event.getType());
	if ((enabled == _eventEnabled.end()) != !enabled->second)
		return true;

	return runScript(_script, event);
}

bool ScriptContainer::runScript(const Common::UString &script, EventType event,
                                Aurora::NWScript::Object *owner,
                                Aurora::NWScript::Object *triggerer) {

	return runScript(script, event, Aurora::NWScript::NCSFile::getEmptyState(), owner, triggerer);
}

bool ScriptContainer::runScript(const Common::UString &script, EventType event,
                                const Aurora::NWScript::ScriptState &state,
                                Aurora::NWScript::Object *owner,
                                Aurora::NWScript::Object *triggerer) {


	Event e(event, triggerer, owner);

	return runScript(script, e, state);
}

bool ScriptContainer::runScript(const Common::UString &script, Event &event) {
	return runScript(script, event, Aurora::NWScript::NCSFile::getEmptyState());
}

bool ScriptContainer::runScript(const Common::UString &script, Event &event,
                                const Aurora::NWScript::ScriptState &state) {

	if (script.empty())
		return true;

	try {
		Aurora::NWScript::NCSFile ncs(script);

		ncs.getEnvironment().setVariable("Event", static_cast<Aurora::NWScript::EngineType *>(&event));

		const Aurora::NWScript::Variable &retVal = ncs.run(state, event.getTarget(), event.getCreator());
		if (retVal.getType() == Aurora::NWScript::kTypeInt)
			return retVal.getInt() != 0;
		if (retVal.getType() == Aurora::NWScript::kTypeFloat)
			return retVal.getFloat() != 0.0f;

		return true;

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed running script \"%s\"", script.c_str());
		return false;
	}

	return true;
}

} // End of namespace DragonAge2

} // End of namespace Engines
