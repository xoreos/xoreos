/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/console.h
 *  KotOR (debug) console.
 */

#ifndef ENGINES_KOTOR_CONSOLE_H
#define ENGINES_KOTOR_CONSOLE_H

#include "engines/aurora/console.h"

namespace Engines {

namespace KotOR {

class Module;

class Console : public ::Engines::Console {
public:
	Console(Module &module);
	~Console();

private:
	Module *_module;

	void cmdGotoModule(const CommandLine &cl);
};

} // End of namespace KOTOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CONSOLE_H
