/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/area.cpp
 *  An area.
 */

#include "engines/kotor2/area.h"

#include "common/ustring.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/aurora/model_kotor.h"

namespace Engines {

namespace KotOR2 {

Area::Area() {
}

Area::~Area() {
}

Graphics::Aurora::Model *Area::loadModel(const Common::UString &resref) {
	Common::SeekableReadStream *mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL);
	Common::SeekableReadStream *mdx = ResMan.getResource(resref, Aurora::kFileTypeMDX);

	Graphics::Aurora::Model *model = 0;
	if (mdl && mdx) {
		try {
			model = new Graphics::Aurora::Model_KotOR(*mdl, *mdx, true);
		} catch(...) {
			delete model;
			model = 0;
		}
	}

	delete mdl;
	delete mdx;
	return model;
}

} // End of namespace KotOR2

} // End of namespace Engines
