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
 *  A TrueType font.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/resman.h"

#include "src/graphics/texture.h"
#include "src/graphics/ttf.h"

#include "src/graphics/images/surface.h"

#include "src/graphics/aurora/ttffont.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

static const uint32 kPageWidth  = 256;
static const uint32 kPageHeight = 256;

namespace Graphics {

namespace Aurora {

TTFFont::Page::Page() : needRebuild(false),
		curX(0), curY(0), heightLeft(kPageHeight), widthLeft(kPageWidth) {

	surface = new Surface(kPageWidth, kPageHeight);
	surface->fill(0x00, 0x00, 0x00, 0x00);

	texture = TextureMan.add(Texture::create(surface));
}

void TTFFont::Page::rebuild() {
	if (!needRebuild)
		return;

	texture.getTexture().rebuild();
	needRebuild = false;
}


TTFFont::TTFFont(Common::SeekableReadStream *ttf, int height) {
	load(ttf, height);
}

TTFFont::TTFFont(const Common::UString &name, int height) {
	Common::SeekableReadStream *ttf = ResMan.getResource(name, ::Aurora::kFileTypeTTF);
	if (!ttf)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	load(ttf, height);
}

TTFFont::~TTFFont() {
}

void TTFFont::load(Common::SeekableReadStream *ttf, int height) {
	Common::ScopedPtr<Common::SeekableReadStream> ttfStream(ttf);

	_ttf.reset(new TTFRenderer(*ttfStream, height));

	_height = _ttf->getHeight();
	if (_height > kPageHeight)
		throw Common::Exception("Font height too big (%d)", _height);

	// Add all ASCII characters
	for (uint32 i = 0; i < 128; i++)
		addChar(i);

	// Add the Unicode "replacement character" character
	addChar(0xFFFD);
	_missingChar = _chars.find(0xFFFD);

	// Find an appropriate width for a "missing character" character
	if (_missingChar == _chars.end()) {
		// This font doesn't have the Unicode "replacement character"

		// Try to find the width of an m. Alternatively, take half of a line's height.
		std::map<uint32, Char>::const_iterator m = _chars.find('m');
		if (m != _chars.end())
			_missingWidth = m->second.width;
		else
			_missingWidth = MAX<float>(2.0f, _height / 2);

	} else
		_missingWidth = _missingChar->second.width;

	rebuildPages();
}

float TTFFont::getWidth(uint32 c) const {
	std::map<uint32, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end())
		return _missingWidth;

	return cC->second.width;
}

float TTFFont::getHeight() const {
	return _height;
}

void TTFFont::drawMissing() const {
	TextureMan.set();

	const float width = _missingWidth - 1.0f;

	glBegin(GL_QUADS);
		glVertex2f(0.0f ,    0.0f);
		glVertex2f(width,    0.0f);
		glVertex2f(width, _height);
		glVertex2f(0.0f , _height);
	glEnd();

	glTranslatef(width + 1.0f, 0.0f, 0.0f);
}

void TTFFont::draw(uint32 c) const {
	std::map<uint32, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end()) {
		cC = _missingChar;

		if (cC == _chars.end()) {
			drawMissing();
			return;
		}
	}

	size_t page = cC->second.page;
	assert(page < _pages.size());

	TextureMan.set(_pages[page]->texture);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC->second.tX[i], cC->second.tY[i]);
		glVertex2f  (cC->second.vX[i], cC->second.vY[i]);
	}
	glEnd();

	glTranslatef(cC->second.width, 0.0f, 0.0f);
}

void TTFFont::buildChars(const Common::UString &str) {
	for (Common::UString::iterator c = str.begin(); c != str.end(); ++c)
		addChar(*c);

	rebuildPages();
}

void TTFFont::rebuildPages() {
	for (std::vector<Page *>::iterator p = _pages.begin(); p != _pages.end(); ++p)
		(*p)->rebuild();
}

void TTFFont::addChar(uint32 c) {
	std::map<uint32, Char>::iterator cC = _chars.find(c);
	if (cC != _chars.end())
		return;

	if (!_ttf->hasChar(c))
		return;

	try {

		uint32 cWidth = _ttf->getCharWidth(c);
		if (cWidth > kPageWidth)
			return;

		if (_pages.empty()) {
			_pages.push_back(new Page);
			_pages.back()->heightLeft -= _height;
		}

		if (_pages.back()->widthLeft < cWidth) {
			// The current character doesn't fit into the current line

			if (_pages.back()->heightLeft >= _height) {
				// Create a new line

				_pages.back()->curX  = 0;
				_pages.back()->curY += _height;

				_pages.back()->heightLeft -= _height;
				_pages.back()->widthLeft   = kPageWidth;

			} else {
				// Create a new page

				_pages.push_back(new Page);
				_pages.back()->heightLeft -= _height;
			}

		}

		_ttf->drawCharacter(c, *_pages.back()->surface, _pages.back()->curX, _pages.back()->curY);

		std::pair<std::map<uint32, Char>::iterator, bool> result;

		result = _chars.insert(std::make_pair(c, Char()));

		cC = result.first;

		Char &ch   = cC->second;
		Page &page = *_pages.back();

		ch.width = cWidth;
		ch.page  = _pages.size() - 1;

		ch.vX[0] = 0.00f;  ch.vY[0] = 0.00f;
		ch.vX[1] = cWidth; ch.vY[1] = 0.00f;
		ch.vX[2] = cWidth; ch.vY[2] = _height;
		ch.vX[3] = 0.00f;  ch.vY[3] = _height;

		const float tX = (float) page.curX / (float) kPageWidth;
		const float tY = (float) page.curY / (float) kPageHeight;
		const float tW = (float) cWidth    / (float) kPageWidth;
		const float tH = (float) _height   / (float) kPageHeight;

		ch.tX[0] = tX;      ch.tY[0] = tY + tH;
		ch.tX[1] = tX + tW; ch.tY[1] = tY + tH;
		ch.tX[2] = tX + tW; ch.tY[2] = tY;
		ch.tX[3] = tX;      ch.tY[3] = tY;

		_pages.back()->widthLeft  -= cWidth;
		_pages.back()->curX       += cWidth;
		_pages.back()->needRebuild = true;

	} catch (...) {
		if (cC != _chars.end())
			_chars.erase(cC);

		Common::exceptionDispatcherWarning();
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
