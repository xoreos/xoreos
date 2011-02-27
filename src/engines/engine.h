/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/engine.h
 *  Generic engine interface.
 */

#ifndef ENGINES_ENGINE_H
#define ENGINES_ENGINE_H

namespace Common {
	class UString;
}

namespace Engines {

/** The base class for an engine within BioWare's Aurora family. */
class Engine {
public:
	Engine();
	virtual ~Engine();

	/** Run the game. */
	virtual void run(const Common::UString &target) = 0;
};

} // End of namespace Engines

#endif // ENGINES_ENGINE_H
