/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/font.h
 *  A font.
 */

#ifndef GRAPHICS_AURORA_FONT_H
#define GRAPHICS_AURORA_FONT_H

namespace Common {
	class UString;
}

namespace Graphics {

namespace Aurora {

class Font {
public:
	Font() {}
	virtual ~Font() {}

	virtual float getWidth(const Common::UString &text) const = 0;
	virtual float getHeight(const Common::UString &text) const = 0;

	virtual void draw(const Common::UString &text) const = 0;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_FONT_H
