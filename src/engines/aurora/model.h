/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/model.h
 *  Generic Aurora engines model functions.
 */

#ifndef ENGINES_AURORA_MODEL_H
#define ENGINES_AURORA_MODEL_H

#include "graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader;

void registerModelLoader(ModelLoader *loader);
void unregisterModelLoader();

Graphics::Aurora::Model *loadModelObject(const Common::UString &resref,
                                         const Common::UString &texture = "");
Graphics::Aurora::Model *loadModelGUI   (const Common::UString &resref);

void freeModel(Graphics::Aurora::Model *&model);

} // End of namespace Engines

#endif // ENGINES_AURORA_MODEL_H
