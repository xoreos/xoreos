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

/** @file
 *  A text object.
 */

#ifndef GRAPHICS_AURORA_TEXT_H
#define GRAPHICS_AURORA_TEXT_H

#include "src/common/ustring.h"
#include "src/common/maths.h"

#include "src/graphics/types.h"
#include <src/graphics/guielement.h>

#include "src/graphics/aurora/fonthandle.h"
#include "src/graphics/aurora/types.h"

namespace Graphics {

namespace Aurora {

/** A text object. */
class Text : public GUIElement {
public:
	Text(const FontHandle &font, const Common::UString &str,
	     float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	     float halign = kHAlignLeft, float valign = kVAlignTop);
	Text(const FontHandle &font, float w, float h, const Common::UString &str,
	     float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	     float halign = kHAlignLeft, float valign = kVAlignTop);
	Text(Graphics::GUIElement::GUIElementType type, const FontHandle &font, float w, float h, const Common::UString &str,
	     float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	     float halign = kHAlignLeft, float valign = kVAlignTop);
	~Text();

	const Common::UString &get() const;
	const Common::UString &getFont() const;
	void getPosition(float &x, float &y, float &z) const;
	void getColor(float &r, float &g, float &b, float &a) const;

	float getHorizontalAlign() const;
	float getVerticalAlign  () const;

	void set(const Common::UString &str, float maxWidth = 0.0f, float maxHeight = 0.0f);
	void setText(const Common::UString &str);
	void setPosition(float x, float y, float z = -FLT_MAX);
	void setSize(float width, float height);
	void setColor(float r, float g, float b, float a);
	void unsetColor();
	void setHorizontalAlign(float halign);
	void setVerticalAlign(float valign);

	/** Change the font of the text. */
	void setFont(const Common::UString &fnt);

	/** Disable parsing <c color tokens into actual coloring. */
	void disableColorTokens(bool disabled);

	bool empty();

	size_t getLineCount() const;

	float getWidth()  const;
	float getHeight() const;

	// Renderable
	void calculateDistance();
	void render(RenderPass pass);
	bool isIn(float x, float y) const;

private:
	float _r, _g, _b, _a;
	FontHandle _font;

	float _x;
	float _y;

	size_t _lineCount;

	float _width;
	float _height;

	float _halign;
	float _valign;

	Common::UString _str;
	ColorPositions  _colors;

	bool _disableColorTokens;

	void parseColors(const Common::UString &str, Common::UString &parsed,
	                 ColorPositions &colors);

	void drawLine(const Common::UString &line, ColorPositions::const_iterator color, size_t position);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXT_H
