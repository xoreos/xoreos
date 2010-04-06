/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/nwn.h
 *  Engine class handling Neverwinter Nights
 */

#ifndef ENGINES_NWN_NWN_H
#define ENGINES_NWN_NWN_H

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace NWN {

class NWNEngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	bool probe(const std::string &directory, const Common::FileList &rootFiles) const;

	Engines::Engine *createEngine() const;
};

extern const NWNEngineProbe kNWNEngineProbe;

class NWNEngine : public Engines::Engine {
public:
	NWNEngine();
	~NWNEngine();

	bool run(const std::string &directory);

private:
	std::string _baseDirectory;

	bool init();
};

} // End of namespace NWN

#endif // ENGINES_NWN_NWN_H
