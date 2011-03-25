/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

uint32 Font::getLineCount(const Common::UString &text) const {
	std::vector<Common::UString> lines;

	split(text, lines);

	return lines.size();
}

float Font::getWidth(const Common::UString &text) const {
	std::vector<Common::UString> lines;

	return split(text, lines);
}

float Font::getHeight(const Common::UString &text) const {
	uint32 lines = getLineCount(text);

	if (lines == 0)
		return 0.0;

	return (lines * getHeight()) + ((lines - 1) * getLineSpacing());
}

void Font::draw(Common::UString text, float r, float g, float b, float a,
                float align) const {

	glColor4f(r, g, b, a);
	draw(text, align);
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void Font::draw(Common::UString text, float align) const {
	std::vector<Common::UString> lines;
	float maxLength = split(text, lines);

	// Move position to the top
	glTranslatef(0.0, (lines.size() - 1) * (getHeight() + getLineSpacing()), 0.0);

	// Draw lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l) {
		// Save the current position
		glPushMatrix();

		// Align
		glTranslatef(roundf((maxLength - getLineWidth(*l)) * align), 0.0, 0.0);

		// Draw line
		for (Common::UString::iterator s = l->begin(); s != l->end(); ++s)
			draw(*s);

		// Restore position to the start of the line
		glPopMatrix();

		// Move to the next line
		glTranslatef(0.0, -(getHeight() + getLineSpacing()), 0.0);
	}
}

float Font::split(const Common::UString &line, std::vector<Common::UString> &lines,
                  float maxWidth) const {

	if (line.empty())
		// Nothing to do
		return 0.0;

	if (maxWidth <= 0.0)
		maxWidth = FLT_MAX;

	// Wrap the line into several lines of at max maxWidth pixel length, breaking
	// the line at font-specific word boundaries.

	Common::UString::iterator lineStart = line.begin();
	Common::UString::iterator lineEnd   = line.begin();

	float length     = 0.0;
	float wordLength = 0.0;
	float lineLength = 0.0;

	Common::UString::iterator p = line.begin();
	while (p != line.end()) {
		uint32 c = *p;

		if (((c == '\n') || Common::UString::isSpace(c)) && (lineEnd != p)) {
			// We can break and there's already something in the word buffer

			if ((lineLength + wordLength) > maxWidth) {
				// Adding the word to the line would overflow

				// Commit the line first
				lines.push_back(Common::UString(lineStart, lineEnd));

				length = MAX(length, lineLength);

				if (c == '\n')
					lineEnd++;

				lineStart = lineEnd;
				lineLength = 0.0;
			}

			// Add the word to the line

			lineEnd = p;

			lineLength += wordLength;
			wordLength = 0.0;
		}

		float charWidth = getWidth(c);

		if ((wordLength + charWidth) > maxWidth) {
			// The word itself overflows the max width

			if (lineEnd != lineStart)
				// Commit the line
				lines.push_back(Common::UString(lineStart, lineEnd));
			// Commit the word fragment in a new line
			lines.push_back(Common::UString(lineEnd, p));

			length = MAX(length, MAX(lineLength, wordLength));

			lineStart = p;
			lineEnd   = p;

			wordLength = 0.0;
			lineLength = 0.0;
		}

		if (c == '\n') {
			// Mandatory line break

			// Commit the line
			lines.push_back(Common::UString(lineStart, lineEnd));

			length = MAX(length, lineLength);

			lineEnd++;

			lineStart = lineEnd;
			lineLength = 0.0;
			wordLength = 0.0;
		}

		// Add the character to the word

		wordLength += charWidth;

		++p;
	}

	if (lineEnd != p) {
		// We've got a dangling word fragment

		if ((lineLength + wordLength) > maxWidth) {
			// The dangling word would overflow the line, commit that first
			lines.push_back(Common::UString(lineStart, lineEnd));

			length = MAX(length, lineLength);

			lineStart = lineEnd;
			lineLength = 0.0;
		}

		// Add the dangling word to the line

		lineEnd = p;

		lineLength += wordLength;
	}

	if (lineEnd != lineStart) {
		// We've got a dangling line, commit it
		lines.push_back(Common::UString(lineStart, lineEnd));

		length = MAX(length, lineLength);
	}

	// Trim the lines
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l)
		l->trim();

	return length;
}

float Font::split(Common::UString &line, float maxWidth) const {
	return split(line, line, maxWidth);
}

float Font::split(const Common::UString &line, Common::UString &lines, float maxWidth) const {
	std::vector<Common::UString> sLines;

	float width = split(line, sLines, maxWidth);

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

} // End of namespace Graphics
