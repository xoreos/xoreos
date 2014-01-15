/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/font.h
 *  A font.
 */

#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <vector>

#include "common/ustring.h"

#include "graphics/types.h"

namespace Ogre {
	class Entity;
}

namespace Graphics {

/** An abstract font. */
class Font {
public:
	Font();
	virtual ~Font();

	/** Return the width of a character. */
	virtual float getWidth (uint32 c) const = 0;
	/** Return the height of a character. */
	virtual float getHeight()         const = 0;

	/** Return the size of space between lines. */
	virtual float getLineSpacing() const;

	/** Return the number of lines this text spans. */
	uint32 getLineCount(const Common::UString &text) const;

	/** Return the width this string would take. */
	float getWidth (const Common::UString &text) const;
	/** Return the height this string would take. */
	float getHeight(const Common::UString &text) const;

	/** Create an overlay element containing the character. */
	virtual Ogre::Entity *createCharacter(uint32 c, float &width, float &height, const Common::UString &scene = "gui") = 0;


	float split(const Common::UString &line, std::vector<Common::UString> &lines,
	            float maxWidth = 0.0) const;
	float split(Common::UString &line, float maxWidth) const;
	float split(const Common::UString &line, Common::UString &lines, float maxWidth) const;


private:
	float getLineWidth(const Common::UString &text) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H
