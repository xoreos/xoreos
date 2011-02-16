/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/thewitcher/modelloader.cpp
 *  The Witcher model loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_witcher.h"

#include "engines/thewitcher/modelloader.h"

namespace Engines {

namespace TheWitcher {

Graphics::Aurora::Model *TheWitcherModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	Common::SeekableReadStream *mdb = ResMan.getResource(resref, Aurora::kFileTypeMDB);
	if (!mdb)
		throw Common::Exception("No such model");

	Graphics::Aurora::Model *model = 0;
	try {
		model = new Graphics::Aurora::Model_Witcher(*mdb);
	} catch (...) {
		delete mdb;
		delete model;
		throw;
	}

	delete mdb;
	return model;
}

} // End of namespace TheWitcher

} // End of namespace Engines
