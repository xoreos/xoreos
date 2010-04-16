/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/object.cpp
 *  A "normal" renderable game object.
 */

#include "graphics/object.h"
#include "graphics/graphics.h"

namespace Graphics {

Object::Object() : Renderable(GfxMan.getObjectQueue()) {
}

Object::~Object() {
}

} // End of namespace Graphics
