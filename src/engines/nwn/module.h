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
 *  The context needed to run a Neverwinter Nights module.
 */

#ifndef ENGINES_NWN_MODULE_H
#define ENGINES_NWN_MODULE_H

#include <list>
#include <map>
#include <set>
#include <memory>

#include "src/common/ptrmap.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/ifofile.h"

#include "src/aurora/nwscript/objectref.h"

#include "src/graphics/aurora/types.h"

#include "src/events/types.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"

namespace Engines {

class Console;

namespace NWN {

class Version;

class Area;
class Creature;

class IngameGUI;

class Module : public NWN::Object, public NWN::ObjectContainer {
public:
	Module(::Engines::Console &console, const Version &gameVersion);
	~Module();

	const Version &getGameVersion() const;

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
	void usePC(const Common::UString &bic, bool local);
	void usePC(Creature *creature);
	/** Exit the currently running module. */
	void exit();
	// '---

	// .--- Information about the current module
	/** Return the IFO of the currently loaded module. */
	const Aurora::IFOFile &getIFO() const;
	// '---

	// .--- Elements of the current module
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();
	/** Return the currently playing PC. */
	Creature *getPC();
	/** Return a map between surface type and walkability. */
	const std::vector<bool> &getWalkableSurfaces() const;
	// '---

	// .--- Interact with the current module
	/** Show the ingame main menu. */
	void showMenu();

	/** Start a conversation.
	 *
	 *  @param  conv      The dialog ResRef to use.
	 *  @param  pc        The PC that initiated the conversation.
	 *  @param  obj       The object the PC is talking with.
	 *  @param  playHello Should the obj play its "Hello" sound?
   *
	 *  @return true if the conversation was started successfully.
	 */
	bool startConversation(const Common::UString &conv, Creature &pc,
	                       NWN::Object &obj, bool playHello = true);
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
	static Common::UString getDescription(const Common::UString &module);
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

	void toggleWalkmesh();

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

	const Version *_gameVersion;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	std::unique_ptr<IngameGUI> _ingameGUI; ///< The ingame GUI elements.

	/** Resources added by the module. */
	Common::ChangeID _resModule;
	/** Resources added for the custom TLK. */
	Common::ChangeID _resTLK;

	/** Resources added by the HAKs of the module. */
	ChangeList _resHAKs;

	Aurora::IFOFile _ifo; ///< The module's IFO.

	std::unique_ptr<Creature> _pc; ///< The player character we use.

	// Texture pack
	int _currentTexturePack;
	Common::ChangeID _resTP[4];

	bool _exit; ///< Should we exit the module?

	AreaMap         _areas;           ///< The areas in the current module.
	Common::UString _newArea;         ///< The new area to enter.
	Area           *_currentArea;     ///< The current area.

	Common::UString _newModule; ///< The module we should change to.

	EventQueue  _eventQueue;
	ActionQueue _delayedActions;

	// Surface types
	/** A map between surface type and walkability. */
	std::vector<bool> _walkableSurfaces;

	// .--- Unloading
	/** Unload the whole shebang.
	 *
	 *  @param completeUnload Also unload the PC and texture packs.
	 *                        true:  completely quit the module
	 *                        false: the PC can be transferred to a new module.
	 */
	void unload(bool completeUnload = true);

	void unloadModule();      ///< Unload the module.
	void unloadPC();          ///< Unload the PC.
	void unloadTLK();         ///< Unload the TLK used by the module.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadTexturePack(); ///< Unload the texture pack.
	void unloadAreas();       ///< Unload the areas.
	// '---

	// .--- Loading
	/** Check if this is premium module, and if so, prepare its loading. */
	void preparePremiumModule(const Common::UString &module);

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadTLK();          ///< Load the TLK used by the module.
	void loadHAKs();         ///< Load the HAKs required by the module.
	void loadTexturePack();  ///< Load the texture pack.
	void loadAreas();        ///< Load the areas.
	void loadSurfaceTypes(); ///< Load the surface types.
	// '---

	static Common::UString getDescriptionExtra   (Common::UString module);
	static Common::UString getDescriptionCampaign(Common::UString module);

	void setPCTokens();
	void removePCTokens();

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

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MODULE_H
