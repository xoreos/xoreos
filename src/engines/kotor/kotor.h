/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/kotor.h
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#ifndef ENGINES_KOTOR_KOTOR_H
#define ENGINES_KOTOR_KOTOR_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

#include "graphics/aurora/types.h"

namespace Common {
	class FileList;
}

namespace Engines {

class ModelLoader;

namespace KotOR {

class KotOREngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

extern const KotOREngineProbe kKotOREngineProbe;

class KotOREngine : public Engines::Engine {
public:
	KotOREngine();
	~KotOREngine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;

	ModelLoader *_modelLoader;

	void init();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_KOTOR_H
