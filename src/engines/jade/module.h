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
 *  A module.
 */

#ifndef ENGINES_JADE_MODULE_H
#define ENGINES_JADE_MODULE_H

#include <list>
#include <set>

#include <memory>
#include "src/common/ustring.h"
#include "src/common/changeid.h"
#include "src/common/configman.h"

#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/objectref.h"

#include "src/events/types.h"

#include "src/engines/jade/objectcontainer.h"

namespace Engines {

class Console;

namespace Jade {

class Area;
class Creature;
class CharacterInfo;

/** A Jade module. */
class Module : public Jade::ObjectContainer {
public:
	Module(::Engines::Console &console);
	~Module();

	/** Clear the whole context. */
	void clear();

	// .--- Module management
	/** Is a module currently loaded and ready to run? */
	bool isLoaded() const;
	/** Is a module currently running? */
	bool isRunning() const;

	/** Load a module. */
	void load(const Common::UString &module);
	/** Use this character as the player character. */
	void usePC(Creature *pc);
	/** Use the given character information to generate a character as pc. */
	void usePC(const CharacterInfo &info);
	/** Exit the currently running module. */
	void exit();
	// '---

	// .--- Information about the current module
	/** Return the module's name. */
	const Common::UString &getName() const;
	// '---

	// .--- Elements of the current module
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();
	/** Return the currently playing PC. */
	Creature *getPC();
	// '---

	// .--- Interact with the current module
	/** Show the ingame main menu. */
	void showMenu();
	// '---

	/** Start a conversation.
	 *
	 *  @param  conv         The dialog ResRef to use.
	 *  @param  pc           The PC that initiated the conversation.
	 *  @param  obj          The object the PC is talking with.
	 *  @param  noWidescreen True to start with short bars, false to start with tall bars
	 *  @param  resetZoom    Whether to set the zoom back to standard when starting the conversation
	 *
	 *  @return true if the conversation was started successfully.
	 */
	bool startConversation(const Common::UString &conv, Creature &pc,
	                       Object &obj, bool noWidescreen = false, bool resetZoom = true);
	// '---

	void delayScript(const Common::UString &script,
	                 const Aurora::NWScript::ScriptState &state,
	                 Aurora::NWScript::Object *owner, Aurora::NWScript::Object *triggerer,
	                 uint32_t delay);

	// .--- PC management
	/** Move the player character to this position within the current area. */
	void movePC(float x, float y, float z);
	/** Move the player character to this object within this area. */
	void movePC(const Common::UString &module);
	/** Notify the module that the PC was moved. */
	void movedPC();
	// '---

	// .--- Module main loop (called by the Game class)
	/** Enter the loaded module, starting it. */
	void enter();
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

	typedef std::list<Events::Event> EventQueue;
	typedef std::multiset<Action> ActionQueue;


	::Engines::Console *_console;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	/** Resources added by the current module. */
	std::list<Common::ChangeID> _resources;

	std::unique_ptr<Creature> _pc; ///< The player character we use.

	bool _exit; ///< Should we exit the module?

	Common::UString _module;    ///< The current module's name.
	Common::UString _newModule; ///< The module we should change to.

	std::unique_ptr<Area> _area; ///< The current module's area.

	EventQueue  _eventQueue;
	ActionQueue _delayedActions;


	// .--- Unloading
	/** Unload the whole shebang.
	 *
	 *  @param completeUnload Also unload the PC.
	 *                        true:  completely quit the module
	 *                        false: the PC can be transferred to a new module.
	 */
	void unload(bool completeUnload = true);

	void unloadPC();
	void unloadArea();
	// '---

	// .--- Loading
	void load();

	void loadArea();
	// '---

	/** Load the actual module. */
	void loadModule(const Common::UString &module);
	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module);
	/** Actually replace the currently running module. */
	void replaceModule();

	void enterArea();
	void leaveArea();

	void handleEvents();

	void handleActions();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_MODULE_H
