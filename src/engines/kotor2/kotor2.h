/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/kotor2.h
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#ifndef ENGINES_KOTOR2_KOTOR2_H
#define ENGINES_KOTOR2_KOTOR2_H

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace KotOR2 {

class KotOR2EngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const std::string &getGameName() const;

	bool probe(const std::string &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream *stream) const;

	Engines::Engine *createEngine() const;

private:
	static const std::string kGameName;
};

extern const KotOR2EngineProbe kKotOR2EngineProbe;

class KotOR2Engine : public Engines::Engine {
public:
	KotOR2Engine();
	~KotOR2Engine();

	void run(const std::string &target);

private:
	std::string _baseDirectory;

	void init();
};

} // End of namespace KotOR2

#endif // ENGINES_KOTOR2_KOTOR2_H
