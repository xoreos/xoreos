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
	Console();
	~Console();

	void setModule(Module *module = 0);

private:
	Module *_module;

	void cmdQuitModule(const CommandLine &cl);
	void cmdListAreas (const CommandLine &cl);
	void cmdGotoArea  (const CommandLine &cl);
	void cmdListMusic (const CommandLine &cl);
	void cmdStopMusic (const CommandLine &cl);
	void cmdPlayMusic (const CommandLine &cl);
	void cmdListVideos(const CommandLine &cl);
	void cmdPlayVideo (const CommandLine &cl);
	void cmdListSounds(const CommandLine &cl);
	void cmdPlaySound (const CommandLine &cl);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CONSOLE_H
