/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn2/modelloader.h
 *  NWN2 model loader.
 */

#ifndef ENGINES_NWN2_MODELLOADER_H
#define ENGINES_NWN2_MODELLOADER_H

#include "engines/aurora/modelloader.h"

namespace Engines {

namespace NWN2 {

class NWN2ModelLoader : public ModelLoader {
public:
	Graphics::Aurora::Model *load(const Common::UString &resref,
			Graphics::Aurora::ModelType type, const Common::UString &texture);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_MODELLOADER_H
