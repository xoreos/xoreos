/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/console.h
 *  NWN (debug) console.
 */

#ifndef ENGINES_NWN_CONSOLE_H
#define ENGINES_NWN_CONSOLE_H

#include "engines/aurora/console.h"

namespace Engines {

namespace NWN {

class Module;

class Console : public ::Engines::Console {
public:
	Console(Module &module);
	~Console();

protected:
	bool cmdCallback(Common::UString cmd, Common::UString args);
	void handleHelp(Common::UString args);

private:
	Module *_module;

	bool listAreas(Common::UString args);
	bool gotoArea(Common::UString args);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CONSOLE_H
