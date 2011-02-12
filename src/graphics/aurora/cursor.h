/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/cursor.h
 *  A cursor as used in the Aurora engines.
 */

#ifndef GRAPHICS_AURORA_CURSOR_H
#define GRAPHICS_AURORA_CURSOR_H

#include "common/ustring.h"

#include "graphics/cursor.h"

#include "graphics/aurora/textureman.h"

namespace Graphics {

namespace Aurora {

/** A cursor as used in the Aurora engines. */
class Cursor : public Graphics::Cursor {
public:
	Cursor(const Common::UString &name);
	~Cursor();

	/** Render the cursor. */
	void render();

private:
	Common::UString _name;

	TextureHandle _texture;

	int _width;
	int _height;

	int _hotspotX;
	int _hotspotY;

	void load();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_CURSOR_H
