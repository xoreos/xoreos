/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/font.h
 *  A font.
 */

#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <vector>

#include "common/ustring.h"

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

	/** Return the width this string would take. */
	float getWidth (const Common::UString &text) const;
	/** Return the height this string would take. */
	float getHeight(const Common::UString &text) const;

	/** Draw this character. */
	virtual void draw(uint32 c) const = 0;

	void draw(Common::UString text, float align = 0.0) const;
	void draw(Common::UString text, float r, float g, float b, float a,
	          float align = 0.0) const;

	float split(const Common::UString &line, std::vector<Common::UString> &lines,
	            float maxWidth = 0.0) const;
	float split(Common::UString &line, float maxWidth) const;
	float split(const Common::UString &line, Common::UString &lines, float maxWidth) const;

private:
	float getLineWidth(const Common::UString &text) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H
