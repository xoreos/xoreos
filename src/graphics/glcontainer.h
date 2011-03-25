/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/glcontainer.h
 *  A container of OpenGL elements.
 */

#ifndef GRAPHICS_GLCONTAINER_H
#define GRAPHICS_GLCONTAINER_H

#include "graphics/queueable.h"

namespace Graphics {

/** A container of OpenGL elements. */
class GLContainer : public Queueable<GLContainer> {
public:
	GLContainer();
	virtual ~GLContainer();

	void rebuild();
	void destroy();

protected:
	virtual void doRebuild() = 0;
	virtual void doDestroy() = 0;

private:
	bool _built;
};

} // End of namespace Graphics

#endif // GRAPHICS_LISTCONTAINER_H
