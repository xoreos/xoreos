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

typedef std::pair<Common::UString::iterator, Common::UString::iterator> LineDefinition;

/** An abstract font. */
class Font {
public:
	Font();
	virtual ~Font();

	/** Return the base height of a character. */
	virtual float getHeight() const = 0;

	/** Return the width this string would take. */
	virtual float getWidth(const Common::UString &text) const = 0;
	/** Return the height this string would take. */
	virtual float getHeight(const Common::UString &text) const = 0;

	/** Draw this string. */
	virtual void draw(const Common::UString &text, float align = 0.0) const = 0;
	/** Draw this string in this color. */
	virtual void draw(const Common::UString &text, float r, float g, float b, float a,
	                  float align = 0.0) const;

	float getLines(const Common::UString &line, std::vector<LineDefinition> &lines,
	               std::vector<float> &lengths) const;

	/** Split a long line, on word boundaries if possible. */
	float split(const Common::UString &line, float maxWidth,
	            std::vector<LineDefinition> &lines) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H
