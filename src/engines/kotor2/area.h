/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/area.h
 *  An area.
 */

#ifndef ENGINES_KOTOR2_AREA_H
#define ENGINES_KOTOR2_AREA_H

#include "engines/kotor/area.h"

namespace Engines {

namespace KotOR2 {

class Area : public Engines::KotOR::Area {
public:
	Area();
	~Area();

protected:
	virtual Graphics::Aurora::Model *loadModel(const Common::UString &resref);
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_AREA_H
