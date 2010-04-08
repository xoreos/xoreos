/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn2/nwn2.h
 *  Engine class handling Neverwinter Nights 2
 */

#ifndef ENGINES_NWN2_NWN2_H
#define ENGINES_NWN2_NWN2_H

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace NWN2 {

class NWN2EngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const std::string &getGameName() const;

	bool probe(const std::string &directory, const Common::FileList &rootFiles) const;

	Engines::Engine *createEngine() const;
};

extern const NWN2EngineProbe kNWN2EngineProbe;

class NWN2Engine : public Engines::Engine {
public:
	NWN2Engine();
	~NWN2Engine();

	bool run(const std::string &directory);

private:
	std::string _baseDirectory;

	bool init();
};

} // End of namespace NWN2

#endif // ENGINES_NWN2_NWN2_H
