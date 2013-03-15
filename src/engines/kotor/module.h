/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
	bool handleCameraKeyboardInput(const Events::Event &e);
	bool handleCameraMouseInput(const Events::Event &e);

	virtual Area *createArea() const;

	friend class Console;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODULE_H
