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

/** @file graphics/font.cpp
 *  A font.
 */

#include "common/util.h"
#include "common/maths.h"

#include "graphics/types.h"
#include "graphics/font.h"

namespace Graphics {

Font::Font() {
}

Font::~Font() {
}

float Font::getLineSpacing() const {
	return 0.0;
}

uint32 Font::getLineCount(const Common::UString &text, float maxWidth, float maxHeight) const {
	std::vector<Common::UString> lines;

	split(text, lines, maxWidth, maxHeight);

	return lines.size();
}

float Font::getWidth(const Common::UString &text, float maxWidth) const {
	std::vector<Common::UString> lines;

	return split(text, lines, maxWidth);
}

float Font::getHeight(const Common::UString &text, float maxWidth, float maxHeight) const {
	uint32 lines = getLineCount(text, maxWidth, maxHeight);

	if (lines == 0)
		return 0.0;

	return (lines * getHeight()) + ((lines - 1) * getLineSpacing());
}

void Font::buildChars(const Common::UString &str) {
}

void Font::draw(Common::UString text, const ColorPositions &colors,
		float r, float g, float b, float a, float align,  float maxWidth, float maxHeight) const {

	glColor4f(r, g, b, a);

	std::vector<Common::UString> lines;
	float maxLength = split(text, lines, maxWidth, maxHeight);

	// Move position to the top
	glTranslatef(0.0, (lines.size() - 1) * (getHeight() + getLineSpacing()), 0.0);

	uint32 position = 0;

	ColorPositions::const_iterator color = colors.begin();

	// Draw lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l) {
		// Save the current position
		glPushMatrix();

		// Align
		glTranslatef(roundf((maxLength - getLineWidth(*l)) * align), 0.0, 0.0);

		// Draw line
		for (Common::UString::iterator s = l->begin(); s != l->end(); ++s, position++) {
			// If we have color changes, apply them
			while ((color != colors.end()) && (color->position <= position)) {
				if (color->defaultColor)
					glColor4f(r, g, b, a);
				else
					glColor4f(color->r, color->g, color->b, color->a);

				++color;
			}

			draw(*s);
		}

		// Restore position to the start of the line
		glPopMatrix();

		// Move to the next line
		glTranslatef(0.0, -(getHeight() + getLineSpacing()), 0.0);

		// \n character
		position++;
	}

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

float Font::split(const Common::UString &line, std::vector<Common::UString> &lines,
                  float maxWidth, float maxHeight) const {

	if (line.empty())
		// Nothing to do
		return 0.0;

	if (maxWidth <= 0.0)
		maxWidth = FLT_MAX;

	if (maxHeight <= 0.0)
		maxHeight = FLT_MAX;

	// Wrap the line into several lines of at max maxWidth pixel length, breaking
	// the line at font-specific word boundaries.

	float length     = 0.0;
	float wordLength = 0.0;
	float lineLength = 0.0;

	Common::UString currentWord;
	Common::UString currentLine;

	Common::UString wordColor;
	Common::UString lineColor;

	std::vector<Common::UString> tokens;
	Common::UString::splitTextTokens(line, tokens);

	bool plain = false;
	for (std::vector<Common::UString>::iterator t = tokens.begin(); t != tokens.end(); ++t) {
		plain = !plain;

		// Color tokens always have length 0
		if (!plain) {
			if (((t->size() == 11) && t->beginsWith("<c") && t->endsWith(">")) ||
			    (*t == "</c>")) {

				currentWord += *t;
				wordColor = *t;
				continue;
			}
		}

		for (Common::UString::iterator p = t->begin(); p != t->end(); ++p) {
			uint32 c = *p;

			if (((c == '\n') || Common::UString::isSpace(c)) && !currentWord.empty()) {
				// We can break and there's already something in the word buffer

				if ((lineLength + wordLength) > maxWidth) {
					// Adding the word to the line would overflow

					// Commit the line first
					if (!addLine(lines, currentLine, maxHeight))
						break;

					length = MAX(length, lineLength);

					currentLine.clear();
					lineLength = 0.0;

					currentLine += lineColor;
				}

				// Add the word to the line

				currentLine += currentWord;
				lineLength  += wordLength;

				lineColor = wordColor;

				currentWord.clear();
				wordLength = 0.0;
			}

			float charWidth = getWidth(c);

			if ((wordLength + charWidth) > maxWidth) {
				// The word itself overflows the max width

				// Commit the line
				if (!currentLine.empty()) {
					if (!addLine(lines, currentLine, maxHeight))
						break;
					}

				// Commit the word fragment in a new line
				if (!addLine(lines, lineColor + currentWord, maxHeight))
					break;

				length = MAX(length, MAX(lineLength, wordLength));

				currentLine.clear();
				lineLength = 0.0;

				currentWord.clear();
				wordLength = 0.0;

				lineColor = wordColor;
				currentLine += lineColor;
			}

			if (c == '\n') {
				// Mandatory line break

				// Commit the line
				if (!addLine(lines, currentLine, maxHeight))
					break;

				length = MAX(length, lineLength);

				currentLine.clear();
				lineLength = 0.0;

				currentWord.clear();
				wordLength = 0.0;

				currentLine += lineColor;
			} else {
				// Add the character to the word

				wordLength += charWidth;

				currentWord += c;
			}

		}

	}

	if (!currentWord.empty()) {
		// We've got a dangling word fragment

		if ((lineLength + wordLength) > maxWidth && addLine(lines, currentLine, maxHeight)) {
			// The dangling word would overflow the line, commit that first

			length = MAX(length, lineLength);

			currentLine.clear();
			lineLength = 0.0;

			currentLine += lineColor;
		}

		// Add the dangling word to the line

		currentLine += currentWord;
		lineLength  += wordLength;
	}

	if (!currentLine.empty() && addLine(lines, currentLine, maxHeight)) {
		// We've got a dangling line, commit it
		length = MAX(length, lineLength);
	}

	// Trim the lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l)
		l->trim();

	return length;
}

float Font::split(Common::UString &line, float maxWidth, float maxHeight) const {
	return split(line, line, maxWidth, maxHeight);
}

float Font::split(const Common::UString &line, Common::UString &lines, float maxWidth, float maxHeight) const {
	std::vector<Common::UString> sLines;

	float width = split(line, sLines, maxWidth, maxHeight);

	lines.clear();
	for (std::vector<Common::UString>::const_iterator l = sLines.begin(); l != sLines.end(); ++l) {
		if (!lines.empty())
			lines += '\n';

		lines += *l;
	}

	return width;
}

float Font::getLineWidth(const Common::UString &text) const {
	float width = 0.0;

	for (Common::UString::iterator c = text.begin(); c != text.end(); ++c)
		width += getWidth(*c);

	return width;
}

bool Font::addLine(std::vector<Common::UString> &lines, const Common::UString &newLine, float maxHeight) const {
	if ((lines.size() + 1) * getHeight() <= maxHeight) {
		lines.push_back(newLine);
		return true;
	} else {
		return false;
	}
}

} // End of namespace Graphics
