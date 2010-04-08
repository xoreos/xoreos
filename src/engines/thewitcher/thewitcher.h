/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace TheWitcher {

class TheWitcherEngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const std::string &getGameName() const;

	bool probe(const std::string &directory, const Common::FileList &rootFiles) const;

	Engines::Engine *createEngine() const;
};

extern const TheWitcherEngineProbe kTheWitcherEngineProbe;

class TheWitcherEngine : public Engines::Engine {
public:
	TheWitcherEngine();
	~TheWitcherEngine();

	bool run(const std::string &directory);

private:
	std::string _baseDirectory;

	bool init();
};

} // End of namespace TheWitcher

#endif // ENGINES_THEWITCHER_THEWITCHER_H
