/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/util.cpp
 *  Utility functions for Neverwinter Nights.
 */

#include "engines/nwn/util.h"

#include "common/stream.h"
#include "common/error.h"

#include "aurora/resman.h"

#include "graphics/aurora/model_nwn_ascii.h"
#include "graphics/aurora/model_nwn_binary.h"

namespace Engines {

namespace NWN {

Graphics::Aurora::Model *loadModel(const Common::UString &resref,
		Graphics::Aurora::ModelType type) {

	Common::SeekableReadStream *mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL);
	if (!mdl)
		throw Common::Exception("No such model \"%s\"", resref.c_str());

	if      (Graphics::Aurora::Model_NWN_Binary::isBinary(*mdl))
		return new Graphics::Aurora::Model_NWN_Binary(*mdl, type);
	else if (Graphics::Aurora::Model_NWN_ASCII::isASCII(*mdl))
		return new Graphics::Aurora::Model_NWN_ASCII(*mdl, type);

	throw Common::Exception("Model not binary and not ASCII?!?");
}

void freeModel(Graphics::Aurora::Model *&model) {
	if (!model)
		return;

	model->hide();
	delete model;

	model = 0;
}

} // End of namespace NWN

} // End of namespace Engines
