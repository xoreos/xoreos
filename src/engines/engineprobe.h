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

#include "aurora/types.h"

namespace Common {
	class UString;
	class SeekableReadStream;
	class FileList;
}

namespace Engines {

class Engine;

/** A probe able to detect one specific game. */
class EngineProbe {
public:
	/** Get the GameID that the probe is able to detect. */
	virtual Aurora::GameID getGameID() const = 0;

	/** Return a string of the full game name. */
	virtual const Common::UString &getGameName() const = 0;

	/** Check for the game in that directory, containing these files. */
	virtual bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const = 0;

	/** Check for the game in that file. */
	virtual bool probe(Common::SeekableReadStream &stream) const = 0;

	/** Create the respective engine for the GameID. */
	virtual Engine *createEngine() const = 0;

	/** Get the GameID that the probe is able to detect. */
	virtual Aurora::Platform getPlatform() const = 0;
};

} // End of namespace Engines

#endif // ENGINES_ENGINEPROBE_H
