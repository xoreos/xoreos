/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/object.h
 *  A "normal" renderable game object.
 */

#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "graphics/renderable.h"

namespace Graphics {

class Object : public Renderable {
public:
	Object();
	virtual ~Object();
};

} // End of namespace Graphics

#endif // GRAPHICS_OBJECT_H
