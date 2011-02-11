/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/jade/jade.h
 *  Engine class handling Jade Empire
 */

#ifndef ENGINES_JADE_JADE_H
#define ENGINES_JADE_JADE_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

namespace Jade {

class JadeEngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const JadeEngineProbe kJadeEngineProbe;

class JadeEngine : public Engines::Engine {
public:
	JadeEngine();
	~JadeEngine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;

	void init();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_JADE_H
