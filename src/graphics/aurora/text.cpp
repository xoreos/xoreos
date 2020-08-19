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

#include "src/events/requests.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/text.h"

namespace Graphics {

namespace Aurora {

Text::Text(const FontHandle &font, const Common::UString &str,
		float r, float g, float b, float a, float halign, float valign) :
	Graphics::GUIElement(Graphics::GUIElement::kGUIElementFront),
	_r(r), _g(g), _b(b), _a(a), _font(font), _x(0.0f), _y(0.0f), _halign(halign),_valign(valign),
	_disableColorTokens(false) {

	set(str);

	_distance = -FLT_MAX;
}

Text::Text(const FontHandle &font, float w, float h, const Common::UString &str,
		float r, float g, float b, float a, float halign, float valign) :
	Graphics::GUIElement(Graphics::GUIElement::kGUIElementFront), _r(r), _g(g), _b(b), _a(a),
	_font(font), _x(0.0f), _y(0.0f), _halign(halign),_valign(valign),
	_disableColorTokens(false) {

	_width = roundf(w);
	_height = roundf(h);

	setText(str);

	_distance = -FLT_MAX;
}

Text::Text(Graphics::GUIElement::GUIElementType type, const FontHandle &font, float w, float h, const Common::UString &str,
		float r, float g, float b, float a, float halign, float valign) :
	Graphics::GUIElement(type), _r(r), _g(g), _b(b), _a(a),
	_font(font), _x(0.0f), _y(0.0f), _halign(halign),_valign(valign),
	_disableColorTokens(false) {

	_width = roundf(w);
	_height = roundf(h);

	setText(str);

	_distance = -FLT_MAX;
}

Text::~Text() {
	hide();
}

void Text::disableColorTokens(bool disabled) {
	_disableColorTokens = disabled;
}

void Text::set(const Common::UString &str, float maxWidth, float maxHeight) {
	lockFrameIfVisible();

	if (!_disableColorTokens)
		parseColors(str, _str, _colors);
	else
		_str = str;

	Font &font = _font.getFont();

	font.buildChars(str);

	_lineCount = font.getLineCount(_str, maxWidth, maxHeight);

	_height = font.getHeight(_str, maxWidth, maxHeight);
	_width  = font.getWidth (_str, maxWidth);

	unlockFrameIfVisible();
}

void Text::setText(const Common::UString &str) {
	lockFrameIfVisible();

	if (!_disableColorTokens)
		parseColors(str, _str, _colors);
	else
		_str = str;

	Font &font = _font.getFont();

	font.buildChars(str);

	_lineCount = font.getLineCount(_str, _width, _height);

	unlockFrameIfVisible();
}

void Text::getColor(float& r, float& g, float& b, float& a) const {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

void Text::setColor(float r, float g, float b, float a) {
	lockFrameIfVisible();

	_r = r;
	_g = g;
	_b = b;
	_a = a;

	unlockFrameIfVisible();
}

void Text::unsetColor() {
	setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

float Text::getHorizontalAlign() const {
	return _halign;
}

void Text::setHorizontalAlign(float halign) {
	_halign = halign;
}

float Text::getVerticalAlign() const {
	return _valign;
}

void Text::setVerticalAlign(float valign) {
	_valign = valign;
}

const Common::UString &Text::get() const {
	return _str;
}

const Common::UString &Text::getFont() const {
	return _font.getName();
}

void Text::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _distance;
}

void Text::setPosition(float x, float y, float z) {
	lockFrameIfVisible();

	_x = x;
	_y = y;

	_distance = z;
	resort();

	unlockFrameIfVisible();
}

void Text::setSize(float width, float height) {
	lockFrameIfVisible();

	_width = roundf(width);
	_height = roundf(height);

	_lineCount = _font.getFont().getLineCount(_str, _width, _height);

	unlockFrameIfVisible();
}

bool Text::empty() {
	return _str.empty();
}

float Text::getHeight(const Common::UString &text) const {
	return _font.getFont().getHeight(text, _width);
}

size_t Text::getLineCount() const {
	return _lineCount;
}

float Text::getWidth() const {
	return _width;
}

float Text::getHeight() const {
	return _height;
}

void Text::calculateDistance() {
}

void Text::render(RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	Font &font = _font.getFont();
	float lineHeight = font.getHeight() + font.getLineSpacing();

	glTranslatef(roundf(_x), roundf(_y), 0.0f);

	glColor4f(_r, _g, _b, _a);

	std::vector<Common::UString> lines;
	font.split(_str, lines, _width, _height, false);

	float blockSize = lines.size() * lineHeight;

	// Move position to the top
	glTranslatef(0.0f, roundf(((_height - blockSize) * _valign) + blockSize - lineHeight), 0.0f);

	size_t position = 0;

	ColorPositions::const_iterator color = _colors.begin();

	// Draw lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l) {
		// Save the current position
		glPushMatrix();

		drawLine(*l, color, position);

		// Restore position to the start of the line
		glPopMatrix();

		// Move to the next line
		glTranslatef(0.0f, -lineHeight, 0.0f);

		// \n character
		position++;
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

bool Text::isIn(float x, float y) const {
	if ((x < _x) || (y < _y))
		return false;

	if ((x > (_x + _width)) || (y > (_y + _height)))
		return false;

	return true;
}

void Text::renderImmediate(const glm::mat4 &parentTransform)
{
	Font &font = _font.getFont();
	float lineHeight = font.getHeight() + font.getLineSpacing();

	std::vector<Common::UString> lines;
	font.split(_str, lines, _width, _height, false);

	float blockSize = lines.size() * lineHeight;

	// Move position to the top
	size_t position = 0;

	ColorPositions::const_iterator color = _colors.begin();

	_font.getFont().renderBind(parentTransform);
	float x = roundf(_x);
	float y = roundf(_y) + roundf(((_height - blockSize) * _valign) + blockSize - lineHeight);
	// Draw lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l) {
		float saved_x = x;
		float saved_y = y;
		drawLineImmediate(*l, color, position, x, y);
		x = saved_x;
		y = saved_y - lineHeight;
		// \n character
		position++;
	}
	_font.getFont().renderUnbind();
}

void Text::parseColors(const Common::UString &str, Common::UString &parsed,
                       ColorPositions &colors) {

	parsed.clear();
	colors.clear();

	ColorPosition color;

	// Split by text tokens. They will have a strictly interleaving plain/token order
	std::vector<Common::UString> tokens;
	Common::UString::splitTextTokens(str, tokens);

	bool plain = false;
	for (std::vector<Common::UString>::iterator t = tokens.begin(); t != tokens.end(); ++t) {
		plain = !plain;

		if (plain) {
			// Plain text, add it verbatim

			parsed += *t;
			continue;
		}

		if ((t->size() == 11) && t->beginsWith("<c") && t->endsWith(">")) {
			// Color start token

			uint8_t colorValue[4];

			Common::UString::iterator it = t->begin();

			// Skip "<c"
			++it;
			++it;

			for (int i = 0; i < 8; i++, ++it) {
				uint32_t c = *it;

				// Convert the hex values into true nibble values
				if      ((c >= '0') && (c <= '9'))
					c =  c - '0';
				else if ((c >= 'A') && (c <= 'F'))
					c = (c - 'A') + 10;
				else if ((c >= 'f') && (c <= 'f'))
					c = (c - 'a') + 10;
				else
					c = 15;

				// Merge two nibbles into one color value byte
				uint8_t &value = colorValue[i / 2];
				bool  high   = (i % 2) == 0;

				if (high)
					value  = c << 4;
				else
					value |= c;
			}

			// Add the color change

			color.position     = parsed.size();
			color.defaultColor = false;

			color.r = colorValue[0] / 255.0f;
			color.g = colorValue[1] / 255.0f;
			color.b = colorValue[2] / 255.0f;
			color.a = colorValue[3] / 255.0f;

			colors.push_back(color);

		} else if (*t == "</c>") {
			// Color end token, add a "uncolor" / default color change

			color.position     = parsed.size();
			color.defaultColor = true;

			colors.push_back(color);

		} else
			// Ignore non-color tokens
			parsed += *t;

	}
}

void Text::setFont(const Common::UString &fnt) {
	_font = FontMan.get(fnt);
}

void Text::drawLine(const Common::UString &line,
                    ColorPositions::const_iterator color,
                    size_t position) {

	Font &font = _font.getFont();

	// Horizontal Align
	glTranslatef(roundf((_width - font.getLineWidth(line)) * _halign), 0.0f, 0.0f);

	// Draw line
	for (Common::UString::iterator s = line.begin(); s != line.end(); ++s, position++) {
		// If we have color changes, apply them
		while ((color != _colors.end()) && (color->position <= position)) {
			if (color->defaultColor)
				glColor4f(_r, _g, _b, _a);
			else
				glColor4f(color->r, color->g, color->b, color->a);

			++color;
		}

		font.draw(*s);
	}

}

void Text::drawLineImmediate(const Common::UString &line,
                             ColorPositions::const_iterator color,
                             size_t position,
                             float &x,
                             float &y) {

	Font &font = _font.getFont();

	// Horizontal Align
	x += round((_width - font.getLineWidth(line)) * _halign);

	// Draw line
	float rgba[] = { _r, _g, _b, _a };
	for (Common::UString::iterator s = line.begin(); s != line.end(); ++s, position++) {
		// If we have color changes, apply them
		while ((color != _colors.end()) && (color->position <= position)) {
			if (color->defaultColor) {
				rgba[0] = _r;
				rgba[1] = _g;
				rgba[2] = _b;
				rgba[3] = _a;
			} else {
				rgba[0] = color->r;
				rgba[1] = color->g;
				rgba[2] = color->b;
				rgba[3] = color->a;
			}

			++color;
		}
		font.render(*s, x, y, rgba);
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
