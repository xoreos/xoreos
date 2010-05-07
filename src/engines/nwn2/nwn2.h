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

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

#include "graphics/aurora/types.h"

namespace Common {
	class FileList;
}

namespace NWN2 {

class NWN2EngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

extern const NWN2EngineProbe kNWN2EngineProbe;

class NWN2Engine : public Engines::Engine {
public:
	NWN2Engine();
	~NWN2Engine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;

	void init();

	Graphics::Aurora::Model *loadModel(const Common::UString &resref);
};

} // End of namespace NWN2

#endif // ENGINES_NWN2_NWN2_H
