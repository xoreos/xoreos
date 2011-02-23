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

#include "graphics/types.h"
#include "graphics/font.h"

namespace Graphics {

Font::Font() {
}

Font::~Font() {
}

void Font::draw(const Common::UString &text, float r, float g, float b, float a,
                float align) const {

	glColor4f(r, g, b, a);
	draw(text, align);
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

float Font::getLines(const Common::UString &line, std::vector<LineDefinition> &lines,
                     std::vector<float> &lengths) const {

	Common::UString::iterator lineStart = line.begin();

	float length = 0.0;

	for (Common::UString::iterator p = line.begin(); p != line.end(); ++p) {
		if (*p == '\n') {
			Common::UString::iterator start = lineStart;
			Common::UString::iterator end   = p;

			while ((end   != lineStart) && Common::UString::isSpace(*--end));
			while ((start != end      ) && Common::UString::isSpace(*start))
				start++;

			lines.push_back(std::make_pair(start, ++end));
			lengths.push_back(getWidth(Common::UString(lines.back().first, lines.back().second)));

			lineStart = p;
			lineStart++;

			length = MAX(length, lengths.back());
		}
	}

	if (lineStart != line.end()) {
		lines.push_back(std::make_pair(lineStart, line.end()));
		lengths.push_back(getWidth(Common::UString(lines.back().first, lines.back().second)));

		length = MAX(length, lengths.back());
	}

	return length;
}

float Font::split(const Common::UString &line, float maxWidth,
                  std::vector<LineDefinition> &lines) const {

	if (line.empty())
		return 0.0;

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
				lines.push_back(std::make_pair(lineStart, lineEnd));

				length = MAX(length, lineLength);

				lineStart = lineEnd;
				lineLength = 0.0;
			}

			// Add the word to the line

			lineEnd = p;

			lineLength += wordLength;
			wordLength = 0;
		}

		float charWidth = getWidth(c);

		if ((wordLength + charWidth) > maxWidth) {
			// The word itself overflows the max width

			if (lineEnd != lineStart)
				// Commit the line
				lines.push_back(std::make_pair(lineStart, lineEnd));
			// Commit the word fragment in a new line
			lines.push_back(std::make_pair(lineEnd, p));

			length = MAX(length, MAX(lineLength, wordLength));

			lineStart = p;
			lineEnd   = p;

			wordLength = 0;
			lineLength = 0;
		}

		if (c == '\n') {
			// Mandatory line break

			if (lineEnd != lineStart) {
				// Commit the line
				lines.push_back(std::make_pair(lineStart, lineEnd));

				length = MAX(length, lineLength);

				lineStart = lineEnd;
				lineLength = 0;
			}

		}

		// Add the character to the word

		wordLength += charWidth;

		++p;
	}

	if (lineEnd != p) {
		// We've got a dangling word fragment

		if ((lineLength + wordLength) > maxWidth) {
			// The dangling word would overflow the line, commit that first
			lines.push_back(std::make_pair(lineStart, lineEnd));

			length = MAX(length, lineLength);

			lineStart = lineEnd;
			lineLength = 0;
		}

		// Add the dangling word to the line

		lineEnd = p;

		lineLength += wordLength;
	}

	if (lineEnd != lineStart) {
		// We've got a dangling line, commit it
		lines.push_back(std::make_pair(lineStart, lineEnd));

		length = MAX(length, lineLength);
	}

	// Trim spaces in front of the lines
	for (std::vector<LineDefinition>::iterator l = lines.begin(); l != lines.end(); ++l)
		while ((l->first != l->second) && Common::UString::isSpace(*l->first))
			++l->first;

	return length;
}

} // End of namespace Graphics
