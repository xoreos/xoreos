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

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

#include "graphics/aurora/types.h"

namespace Common {
	class FileList;
}

namespace NWN {

class NWNEngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

extern const NWNEngineProbe kNWNEngineProbe;

class NWNEngine : public Engines::Engine {
public:
	NWNEngine();
	~NWNEngine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;

	bool _hasXP1; // Shadows of Undrentide (SoU)
	bool _hasXP2; // Hordes of the Underdark (HotU)
	bool _hasXP3; // Kingmaker (resources also included in the final 1.69 patch)

	void init();

	Graphics::Aurora::Model *showLegal();

	Graphics::Aurora::Model *loadModel(const Common::UString &resref,
			Graphics::Aurora::ModelType type = Graphics::Aurora::kModelTypeObject);
	void freeModel(Graphics::Aurora::Model *&model);
};

} // End of namespace NWN

#endif // ENGINES_NWN_NWN_H
