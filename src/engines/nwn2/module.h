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
 *  The context needed to run a Neverwinter Nights 2 module.
 */

#ifndef ENGINES_NWN2_MODULE_H
#define ENGINES_NWN2_MODULE_H

#include <vector>
#include <list>
#include <map>
#include <set>
#include <memory>

#include "src/common/ptrmap.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/ifofile.h"
#include "src/aurora/gff3file.h"

#include "src/aurora/nwscript/objectref.h"

#include "src/events/types.h"

#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

class Console;

namespace NWN2 {

class Area;
class Creature;
class Factions;
class Roster;
class Journal;

class Module : public NWN2::Object, public NWN2::ObjectContainer {
public:
	Module(::Engines::Console &console);
	Module();
	~Module();

	// .--- Module management
	/** Is a module currently loaded and ready to run? */
	bool isLoaded() const;
	/** Is a module currently running? */
	bool isRunning() const;
	// '---

	// .--- Information about the current module
	/** Return the IFO of the currently loaded module. */
	const Aurora::IFOFile &getIFO() const;

	/** Return the module's name. */
	const Common::UString &getName() const;
	/** Return the module's description. */
	const Common::UString &getDescription() const;
	// '---

	// .--- Elements of the current module
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();
	/** Return the currently playing PC. */
	Creature *getPC();
	/** Return the module's factions. */
	Factions &getFactions();
	/** Return the module's roster. */
	Roster &getRoster();
	/** Return the module's journal. */
	Journal &getJournal();
	/** Load the campaign journal. */
	void loadCampaignJournal(const Aurora::GFF3Struct &gff);
	// '---

	void delayScript(const Common::UString &script,
	                 const Aurora::NWScript::ScriptState &state,
	                 Aurora::NWScript::Object *owner, Aurora::NWScript::Object *triggerer,
	                 uint32_t delay);

	// .--- PC management
	/** Move the player character to this area. */
	void movePC(const Common::UString &area);
	/** Move the player character to this position within the current area. */
	void movePC(float x, float y, float z);
	/** Move the player character to this position within this area. */
	void movePC(const Common::UString &area, float x, float y, float z);
	/** Move the player character to this position within this area. */
	void movePC(Area *area, float x, float y, float z);
	/** Notify the module that the PC was moved. */
	void movedPC();
	// '---

	// .--- Static utility methods
	static Common::UString getName(const Common::UString &module);
	static Common::UString getDescription(const Common::UString &module);
	// '---

	// .--- Module main loop (called by the Campaign class)
	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	void load(const Common::UString &module);
	/** Exit the currently running module. */
	void exit();

	/** Enter the loaded module, starting it. */
	void enter(Creature &pc, bool isNewCampaign = true);
	/** Leave the running module, quitting it. */
	void leave();

	/** Add a single event for consideration into the event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();
	// '---

private:
	enum ActionType {
		kActionNone   = 0,
		kActionScript = 1
	};

	struct Action {
		ActionType type;

		Common::UString script;

		Aurora::NWScript::ScriptState state;
		Aurora::NWScript::ObjectReference owner;
		Aurora::NWScript::ObjectReference triggerer;

		uint32_t timestamp;

		bool operator<(const Action &s) const;
	};

	typedef Common::PtrMap<Common::UString, Area> AreaMap;

	typedef std::list<Events::Event> EventQueue;
	typedef std::multiset<Action> ActionQueue;


	::Engines::Console *_console;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?
	bool _exit;      ///< Should we exit the module?

	/** Resources added by the module. */
	Common::ChangeID _resModule;
	/** Resources added for the custom TLK. */
	Common::ChangeID _resTLK;

	/** Resources added by the HAKs of the module. */
	std::vector<Common::ChangeID> _resHAKs;

	Aurora::IFOFile _ifo; ///< The module's IFO.

	/** The player character we use. */
	Creature *_pc;

	AreaMap         _areas;       ///< The areas in the current module.
	Common::UString _newArea;     ///< The new area to enter.
	Area           *_currentArea; ///< The current area.

	std::unique_ptr<Factions> _factions; ///< The factions in the current module.
	std::unique_ptr<Roster>   _roster;   ///< The roster for the current module.

	std::unique_ptr<Journal> _moduleJournal;   ///< The current module's journal.
	std::unique_ptr<Journal> _campaignJournal; ///< The campaign journal.

	bool _ranPCSpawn; ///< Did we run the PC spawn script?

	Common::UString _newModule; ///< The module we should change to.

	EventQueue  _eventQueue;
	ActionQueue _delayedActions;


	// .--- Unloading
	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadPC();          ///< Unload the PC.
	void unloadTLK();         ///< Unload the TLK used by the module.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadAreas();       ///< Unload the areas.
	void unloadFactions();    ///< Unload the factions.
	void unloadRoster();      ///< Unload the roster.
	void unloadJournal();     ///< Unload the journal.
	// '---

	// .--- Loading
	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadTLK();         ///< Load the TLK used by the module.
	void loadHAKs();        ///< Load the HAKs required by the module.
	void loadAreas();       ///< Load the areas.
	void loadFactions();    ///< Load the factions.
	void loadRoster();      ///< Load the roster.
	void loadJournal();     ///< Load the journal.
	// '---

	void enterArea(); ///< Enter a new area.

	/** Load the actual module. */
	void loadModule(const Common::UString &module);
	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module);
	/** Actually replace the currently running module. */
	void replaceModule();

	void handleEvents();

	void handleActions();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_MODULE_H
