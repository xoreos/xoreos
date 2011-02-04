/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/util.h
 *  Utility functions for Neverwinter Nights.
 */

#ifndef ENGINES_NWN_UTIL_H
#define ENGINES_NWN_UTIL_H

#include "engines/util.h"

namespace Engines {

namespace NWN {

class NWNModelLoader : public ModelLoader {
public:
	Graphics::Aurora::Model *operator()(const Common::UString &resref,
			Graphics::Aurora::ModelType type = Graphics::Aurora::kModelTypeObject,
			const Common::UString &texture = "") const;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_UTIL_H
