/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/guifrontelement.h
 *  A GUI element that is to be drawn in front of the normal objects.
 */

#ifndef GRAPHICS_GUIFRONTELEMENT_H
#define GRAPHICS_GUIFRONTELEMENT_H

#include "graphics/renderable.h"

namespace Graphics {

/** An element of the front GUI. */
class GUIFrontElement : public Renderable {
public:
	GUIFrontElement();
	~GUIFrontElement();
};

} // End of namespace Graphics

#endif // GRAPHICS_GUIFRONTELEMENT_H
