/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/module.h
 *  The context needed to run a NWN module.
 */

#ifndef ENGINES_NWN_MODULE_H
#define ENGINES_NWN_MODULE_H

#include <list>

#include "aurora/resman.h"

#include "engines/nwn/ifofile.h"
#include "engines/nwn/creature.h"

namespace Engines {

namespace NWN {

class CharacterID;
class Area;

class IngameGUI;

class Module {
public:
	Module();
	~Module();

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	bool loadModule(const Common::UString &module);
	/** Use this character as the player character. */
	bool usePC(const CharacterID &c);

	void run();

private:
	bool _hasModule; ///< Do we have a module?
	bool _hasPC;     ///< Do we have a PC?

	IngameGUI *_ingameGUI; ///< The ingame GUI elements.

	/** Resources added by the module. */
	Aurora::ResourceManager::ChangeID _resModule;

	/** Resources added by the HAKs of the module. */
	std::vector<Aurora::ResourceManager::ChangeID> _resHAKs;

	IFOFile _ifo; ///< The module's IFO.

	Creature _pc; ///< The player character we use.

	// Texture pack
	int _currentTexturePack;
	Aurora::ResourceManager::ChangeID _resTP[4];

	bool _exit; //< Should we exit the module?

	Common::UString _newArea; ///< The new area to enter.
	Area           *_area;    ///< The current area.

	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadPC();          ///< Unload the PC.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadTexturePack(); ///< Unload the texture pack.
	void unloadArea();        ///< Unload the current area.

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadHAKs();        ///< Load the HAKs required by the module.
	void loadTexturePack(); ///< Load the texture pack.
	void loadArea();        ///< Load a new area.

	void showMenu();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MODULE_H
