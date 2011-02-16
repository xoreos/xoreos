/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/modelloader.cpp
 *  An abstract Aurora model loader.
 */

#include "graphics/aurora/model.h"

#include "engines/aurora/modelloader.h"

namespace Engines {

void ModelLoader::free(Graphics::Aurora::Model *&model) {
	delete model;
	model = 0;
}

} // End of namespace Engines
