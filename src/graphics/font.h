/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file graphics/font.h
 *  A font.
 */

#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <vector>

#include "common/ustring.h"

#include "graphics/types.h"

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
	uint32 getLineCount(const Common::UString &text, float maxWidth = 0.0, float maxHeight = 0.0) const;

	/** Return the width this string would take. */
	float getWidth (const Common::UString &text, float maxWidth = 0.0) const;
	/** Return the height this string would take. */
	float getHeight(const Common::UString &text, float maxWidth = 0.0, float maxHeight = 0.0) const;

	/** Build all necessary characters to display this string. */
	virtual void buildChars(const Common::UString &str);

	/** Draw this character. */
	virtual void draw(uint32 c) const = 0;

	void draw(Common::UString text, const ColorPositions &colors,
		  float r, float g, float b, float a, float align = 0.0, float maxWidth = 0.0, float maxHeight = 0.0) const;

	float split(const Common::UString &line, std::vector<Common::UString> &lines,
		    float maxWidth = 0.0, float maxHeight = 0.0) const;
	float split(Common::UString &line, float maxWidth, float maxHeight = 0.0) const;
	float split(const Common::UString &line, Common::UString &lines, float maxWidth, float maxHeight = 0.0) const;

private:
	float getLineWidth(const Common::UString &text) const;
	bool addLine(std::vector<Common::UString> &lines, const Common::UString &newLine, float maxHeight) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H
