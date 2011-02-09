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

#include "graphics/queueable.h"

namespace Graphics {

class ListContainer : public Queueable<ListContainer> {
public:
	ListContainer();
	~ListContainer();

protected:
	void enforceMainThread();


// To be called from the main/events/graphics thread
public:
	/** Rebuild the OpenGL lists. */
	virtual void rebuild() = 0;
	/** Destroy the OpenGL lists. */
	virtual void destroy() = 0;
};

} // End of namespace Graphics

#endif // GRAPHICS_LISTCONTAINER_H
