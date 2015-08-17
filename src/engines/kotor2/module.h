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
 *  The context needed to run a Star Wars: Knights of the Old Republic II - The Sith Lords module.
 */

#ifndef ENGINES_KOTOR2_MODULE_H
#define ENGINES_KOTOR2_MODULE_H

#include "src/common/ustring.h"
#include "src/common/changeid.h"
#include "src/common/configman.h"

#include "src/aurora/ifofile.h"

#include "src/events/types.h"

#include "src/engines/kotor2/objectcontainer.h"
#include "src/engines/kotor2/object.h"

namespace Engines {

class Console;

namespace KotOR2 {

class Area;

class Module : public KotOR2::Object, public KotOR2::ObjectContainer {
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
	// '---

	// .--- Interact with the current module
	/** Show the ingame main menu. */
	void showMenu();
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
	typedef std::list<Events::Event> EventQueue;


	::Engines::Console *_console;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	/** Resources added by the current module. */
	std::list<Common::ChangeID> _resources;

	/** The current module's IFO. */
	Aurora::IFOFile _ifo;

	/** The current texture pack. */
	int _currentTexturePack;
	/** Resources added by the current texture pack. */
	Common::ChangeID _textures;

	bool _exit; //< Should we exit the module?

	Common::UString _module;    ///< The current module's name.
	Common::UString _newModule; ///< The module we should change to.

	Area *_area; ///< The current module's area.

	EventQueue  _eventQueue;


	// .--- Unloading
	void unload();
	void unloadResources();
	void unloadIFO();
	void unloadArea();
	void unloadTexturePack();
	// '---

	// .--- Loading
	void load();
	void loadResources();
	void loadIFO();
	void loadArea();
	void loadTexturePack();
	// '---

	/** Load the actual module. */
	void loadModule(const Common::UString &module);
	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module);
	/** Actually replace the currently running module. */
	void replaceModule();

	void handleEvents();
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_MODULE_H
