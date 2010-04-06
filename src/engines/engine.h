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

#include <string>

namespace Common {
	class SeekableReadStream;
}

namespace Engines {

class Engine {
public:
	Engine();
	virtual ~Engine();

	virtual bool run(const std::string &directory) = 0;

protected:
	/** Add a KEY file to the resource manager, erroring out if the file does not exist. */
	bool indexMandatoryKEY(const std::string &key);

	/** Add a KEY file to the resource manager, if it exists. */
	bool indexOptionalKEY(const std::string &key);

	/** Debug method to quickly dump a stream to disk. */
	void dumpStream(Common::SeekableReadStream &stream, const std::string &fileName);
};

} // End of namespace Engines

#endif // ENGINES_ENGINE_H
