/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
	Cursor(const Common::UString &name, int hotspotX = -1, int hotspotY = -1);
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
