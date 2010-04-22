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
	/** Add a KEY file to the resource manager, erroring out if the file does not exist. */
	void indexMandatoryKEY(const Common::UString &key, uint32 priority = 10);

	/** Add a KEY file to the resource manager, if it exists. */
	void indexOptionalKEY(const Common::UString &key, uint32 priority = 10);

	/** Add an ERF file to the resource manager, erroring out if the file does not exist. */
	void indexMandatoryERF(const Common::UString &erf, uint32 priority = 100);

	/** Add an ERF file to the resource manager, if it exists. */
	void indexOptionalERF(const Common::UString &erf, uint32 priority = 100);

	/** Add a RIM file to the resource manager, erroring out if the file does not exist. */
	void indexMandatoryRIM(const Common::UString &rim, uint32 priority = 100);

	/** Add a RIM file to the resource manager, if it exists. */
	void indexOptionalRIM(const Common::UString &rim, uint32 priority = 100);

	/** Add a ZIP file to the resource manager, erroring out if the file does not exist. */
	void indexMandatoryZIP(const Common::UString &zip, uint32 priority = 100);

	/** Add a ZIP file to the resource manager, if it exists. */
	void indexOptionalZIP(const Common::UString &zip, uint32 priority = 100);

	/** Play this video resource. */
	void playVideo(const Common::UString &video);

	/** Debug method to quickly dump a stream to disk. */
	void dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);
};

} // End of namespace Engines

#endif // ENGINES_ENGINE_H
