/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/thewitcher/thewitcher.h
 *  Engine class handling The Witcher
 */

#ifndef ENGINES_THEWITCHER_THEWITCHER_H
#define ENGINES_THEWITCHER_THEWITCHER_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

namespace TheWitcher {

class TheWitcherEngineProbe : public Engines::EngineProbe {
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

extern const TheWitcherEngineProbe kTheWitcherEngineProbe;

class TheWitcherEngine : public Engines::Engine {
public:
	TheWitcherEngine();
	~TheWitcherEngine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;

	void init();
	void initCursors();
};

} // End of namespace TheWitcher

} // End of namespace Engines

#endif // ENGINES_THEWITCHER_THEWITCHER_H
