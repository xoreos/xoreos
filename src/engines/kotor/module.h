/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/module.h
 *  A module.
 */

#ifndef ENGINES_KOTOR_MODULE_H
#define ENGINES_KOTOR_MODULE_H

#include "common/ustring.h"
#include "common/configman.h"

#include "aurora/resman.h"

#include "events/types.h"

#include "engines/nwn/ifofile.h"

namespace Engines {

class Console;

namespace KotOR {

class Area;

/** A KotOR module. */
class Module {
public:
	Module(Console &console);
	virtual ~Module();

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	bool load(const Common::UString &module);

	/** Replace the currently running module. */
	bool replaceModule(const Common::UString &module);

	void run();

	void showMenu();

protected:
	Console *_console;

	/** The current module's name. */
	Common::UString _module;

	/** The current module's area. */
	Area *_area;

	/** Resources added by the current module. */
	std::list<Aurora::ResourceManager::ChangeID> _resources;

	/** The current texture pack. */
	int _currentTexturePack;
	/** Resources added by the current texture pack. */
	Aurora::ResourceManager::ChangeID _textures;

	/** The current module's IFO. */
	::Engines::NWN::IFOFile _ifo;

	bool _exit; //< Should we exit the module?


	void load();
	void loadResources();
	void loadIFO();
	void loadArea();
	void loadTexturePack();

	void unload();
	void unloadResources();
	void unloadIFO();
	void unloadArea();
	void unloadTexturePack();

	void enter(); ///< Enter the current module.
	void leave(); ///< Leave the current module.

	void handleEvents();
	bool handleCamera(const Events::Event &e);

	virtual Area *createArea() const;

	friend class Console;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODULE_H
