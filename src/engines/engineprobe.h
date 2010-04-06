/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/engineprobe.h
 *  A probe that checks if an engine can handle game data found in a specific directory and
 *  creates an instance of that engine.
 */

#ifndef ENGINES_ENGINEPROBE_H
#define ENGINES_ENGINEPROBE_H

#include <string>

#include "aurora/types.h"

namespace Common {
	class FileList;
}

namespace Engines {

class Engine;

class EngineProbe {
public:
	virtual Aurora::GameID getGameID() const = 0;

	virtual bool probe(const std::string &directory, const Common::FileList &rootFiles) const = 0;

	virtual Engine *createEngine() const = 0;
};

} // End of namespace Engines

#endif // ENGINES_ENGINEPROBE_H
