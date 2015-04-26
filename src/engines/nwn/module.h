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
 *  The context needed to run a NWN module.
 */

#ifndef ENGINES_NWN_MODULE_H
#define ENGINES_NWN_MODULE_H

#include <list>
#include <set>
#include <map>

#include "src/common/ustring.h"

#include "src/aurora/resman.h"
#include "src/aurora/ifofile.h"

#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/objectcontainer.h"

#include "src/graphics/aurora/types.h"

#include "src/events/types.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/script/container.h"

namespace Engines {

class Console;

namespace NWN {

class Version;

class Object;

class Area;

class IngameGUI;

class Module : public Aurora::NWScript::Object, public Aurora::NWScript::ObjectContainer,
               public ScriptContainer {
public:
	Module(::Engines::Console &console, const Version &gameVersion);
	~Module();

	const Version &getGameVersion() const;

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	void load(const Common::UString &module);
	/** Use this character as the player character. */
	void usePC(const Common::UString &bic, bool local);
	/** Run the currently loaded module. */
	void run();
	/** Exit the currently running module. */
	void exit();

	/** Show the ingame main menu. */
	void showMenu();

	/** Is a module currently running? */
	bool isRunning() const;
	/** Return the name of the currently loaded module. */
	const Common::UString &getName() const;

	/** Return the IFO of the currently loaded module. */
	const Aurora::IFOFile &getIFO() const;
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();
	/** Return the currently playing PC. */
	Creature *getPC();

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
	                       Engines::NWN::Object &obj, bool playHello = true);

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

	void delayScript(const Common::UString &script,
	                 const Aurora::NWScript::ScriptState &state,
	                 Aurora::NWScript::Object *owner, Aurora::NWScript::Object *triggerer,
	                 uint32 delay);


	static Common::UString getDescription(const Common::UString &module);


private:
	enum ActionType {
		kActionNone   = 0,
		kActionScript = 1
	};

	struct Action {
		ActionType type;

		Common::UString script;

		Aurora::NWScript::ScriptState state;
		Aurora::NWScript::Object *owner;
		Aurora::NWScript::Object *triggerer;

		uint32 timestamp;

		bool operator<(const Action &s) const;
	};

	typedef std::map<Common::UString, Area *> AreaMap;


	::Engines::Console *_console;

	const Version *_gameVersion;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	IngameGUI *_ingameGUI; ///< The ingame GUI elements.

	/** Resources added by the module. */
	Aurora::ResourceManager::ChangeID _resModule;

	/** Resources added by the HAKs of the module. */
	std::vector<Aurora::ResourceManager::ChangeID> _resHAKs;

	Aurora::IFOFile _ifo; ///< The module's IFO.

	Creature *_pc; ///< The player character we use.

	// Texture pack
	int _currentTexturePack;
	Aurora::ResourceManager::ChangeID _resTP[4];

	bool _exit; //< Should we exit the module?

	AreaMap         _areas;           ///< The areas in the current module.
	Common::UString _newArea;         ///< The new area to enter.
	Area           *_currentArea;     ///< The current area.

	Common::UString _newModule; ///< The module we should change to.

	std::multiset<Action> _delayedActions;


	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadPC();          ///< Unload the PC.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadTexturePack(); ///< Unload the texture pack.
	void unloadAreas();       ///< Unload the areas.

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadHAKs();        ///< Load the HAKs required by the module.
	void loadTexturePack(); ///< Load the texture pack.
	void loadAreas();       ///< Load the areas.

	void setPCTokens();
	void removePCTokens();

	void enter();     ///< Enter the currently loaded module.
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
