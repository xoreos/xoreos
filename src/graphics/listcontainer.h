/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/listcontainer.h
 *  A container of OpenGL lists.
 */

#ifndef GRAPHICS_LISTCONTAINER_H
#define GRAPHICS_LISTCONTAINER_H

#include "graphics/glcontainer.h"
#include "graphics/queueable.h"

namespace Graphics {

/** A container of OpenGL lists. */
class ListContainer : public GLContainer, public Queueable<ListContainer> {
public:
	ListContainer();
	~ListContainer();
};

} // End of namespace Graphics

#endif // GRAPHICS_LISTCONTAINER_H
