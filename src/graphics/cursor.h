/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/cursor.cpp
 *  A cursor.
 */

#ifndef GRAPHICS_CURSOR_H
#define GRAPHICS_CURSOR_H

namespace Graphics {

class Cursor {
public:
	Cursor();
	virtual ~Cursor();

	/** Render the cursor. */
	virtual void render() = 0;
};

} // End of namespace Graphics

#endif // GRAPHICS_CURSOR_H
