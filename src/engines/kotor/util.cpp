/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/util.cpp
 *  Utility functions.
 */

#include "engines/kotor/util.h"

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/aurora/model_kotor.h"

namespace Engines {

namespace KotOR {

Graphics::Aurora::Model *KotORModelLoader::operator()(const Common::UString &resref,
	Graphics::Aurora::ModelType type, const Common::UString &texture) const {

	Common::SeekableReadStream *mdl = 0, *mdx = 0;
	Graphics::Aurora::Model *model = 0;

	try {
		if (!(mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL)))
			throw Common::Exception("No such MDL");
		if (!(mdx = ResMan.getResource(resref, Aurora::kFileTypeMDX)))
			throw Common::Exception("No such MDX");

		model = new Graphics::Aurora::Model_KotOR(*mdl, *mdx, false, type, texture);

	} catch (...) {
		delete mdl;
		delete mdx;
		delete model;
		throw;
	}

	return model;
}

} // End of namespace KotOR

} // End of namespace Engines
