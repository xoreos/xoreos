/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/modelloader.h
 *  An abstract Aurora model loader.
 */

#ifndef ENGINES_AURORA_MODELLOADER_H
#define ENGINES_AURORA_MODELLOADER_H

#include "graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader {
public:
	virtual ~ModelLoader();

	virtual Graphics::Aurora::Model *load(const Common::UString &resref,
			Graphics::Aurora::ModelType type, const Common::UString &texture) = 0;
	virtual void free(Graphics::Aurora::Model *&model);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_MODELLOADER_H
