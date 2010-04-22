/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/engine.h
 *  Generic engine interface
 */

#ifndef ENGINES_ENGINE_H
#define ENGINES_ENGINE_H

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Engines {

/** The base class for an engine within BioWare's Aurora family. */
class Engine {
public:
	Engine();
	virtual ~Engine();

	/** Run the game. */
	virtual void run(const Common::UString &target) = 0;

protected:
	/** Add an archive file to the resource manager, erroring out if the file does not exist. */
	void indexMandatoryArchive(Aurora::Archive archive, const Common::UString &file, uint32 priority = 10);

	/** Add an archive file to the resource manager, if it exists. */
	void indexOptionalArchive(Aurora::Archive archive, const Common::UString &file, uint32 priority = 10);

	/** Play this video resource. */
	void playVideo(const Common::UString &video);

	/** Debug method to quickly dump a stream to disk. */
	void dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);
};

} // End of namespace Engines

#endif // ENGINES_ENGINE_H
