/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/types.h
 *  Basic Aurora graphics types.
 */

#ifndef GRAPHICS_AURORA_TYPES_H
#define GRAPHICS_AURORA_TYPES_H

#include "graphics/types.h"

namespace Graphics {

namespace Aurora {

enum ModelType {
	kModelTypeObject   = kRenderableQueueObject,
	kModelTypeGUIFront = kRenderableQueueGUIFront
};

class Model;
class Text;

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TYPES_H
