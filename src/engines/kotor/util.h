/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/util.h
 *  Utility functions.
 */

#ifndef ENGINES_KOTOR_UTIL_H
#define ENGINES_KOTOR_UTIL_H

#include "engines/util.h"

namespace Engines {

namespace KotOR {

class KotORModelLoader : public ModelLoader {
public:
	Graphics::Aurora::Model *operator()(const Common::UString &resref,
	                                    const Common::UString &texture = "") const;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_UTIL_H
