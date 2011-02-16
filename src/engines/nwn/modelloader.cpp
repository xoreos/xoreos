/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/modelloader.cpp
 *  NWN model loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_nwn_ascii.h"
#include "graphics/aurora/model_nwn_binary.h"

#include "engines/nwn/modelloader.h"

namespace Engines {

namespace NWN {

Graphics::Aurora::Model *NWNModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	Common::SeekableReadStream *mdl = 0;
	Graphics::Aurora::Model *model = 0;
	try {
		mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL);

		if (!mdl)
			throw Common::Exception("No such model \"%s\"", resref.c_str());

		if      (Graphics::Aurora::Model_NWN_Binary::isBinary(*mdl))
			model = new Graphics::Aurora::Model_NWN_Binary(*mdl, type);
		else if (Graphics::Aurora::Model_NWN_ASCII::isASCII(*mdl))
			model = new Graphics::Aurora::Model_NWN_ASCII(*mdl, type);

		if (!model)
			throw Common::Exception("Model not binary and not ASCII?!?");

	} catch (...) {
		delete mdl;
		delete model;
		throw;
	}

	return model;
}

} // End of namespace NWN

} // End of namespace Engines
