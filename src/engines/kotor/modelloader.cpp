/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/modelloader.cpp
 *  KotOR model loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_kotor.h"

#include "engines/kotor/modelloader.h"

namespace Engines {

namespace KotOR {

Graphics::Aurora::Model *KotORModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

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

	delete mdl;
	delete mdx;
	return model;
}

} // End of namespace KotOR

} // End of namespace Engines
