/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** @file graphics/aurora/fontman.cpp
 *  The Aurora font manager.
 */

#include "common/error.h"
#include "common/systemfonts.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/texturefont.h"
#include "graphics/aurora/abcfont.h"
#include "graphics/aurora/ttffont.h"

DECLARE_SINGLETON(Graphics::Aurora::FontManager)

namespace Graphics {

namespace Aurora {

const char *kSystemFontMono = "_xoreosSystemFontMono";

ManagedFont::ManagedFont(Font *f) {
	referenceCount = 0;
	font = f;
}

ManagedFont::~ManagedFont() {
	delete font;
}


FontHandle::FontHandle() : empty(true) {
}

FontHandle::FontHandle(FontMap::iterator &i) : empty(false), it(i) {
}

FontHandle::FontHandle(const FontHandle &right) : empty(true) {
	*this = right;
}

FontHandle::~FontHandle() {
	FontMan.release(*this);
}

FontHandle &FontHandle::operator=(const FontHandle &right) {
	if (this == &right)
		return *this;

	FontMan.release(*this);

	empty = right.empty;
	it    = right.it;

	if (!empty)
		it->second->referenceCount++;

	return *this;
}

void FontHandle::clear() {
	empty = true;
}

const Common::UString &FontHandle::getFontName() const {
	assert(!empty);

	return it->first;
}

Font &FontHandle::getFont() const {
	assert(!empty);

	return *it->second->font;
}


FontManager::FontManager() : _format(kFontFormatUnknown) {
}

FontManager::~FontManager() {
	clear();
}

void FontManager::clear() {
	Common::StackLock lock(_mutex);

	for (FontMap::iterator font = _fonts.begin(); font != _fonts.end(); ++font)
		delete font->second;

	_fonts.clear();

	_format = kFontFormatUnknown;
	_aliases.clear();
}

void FontManager::setFormat(FontFormat format) {
	_format = format;
}

void FontManager::addAlias(const Common::UString &alias, const Common::UString &realName) {
	_aliases[alias] = realName;
}

FontHandle FontManager::get(Common::UString name, int height) {
	return get(_format, name, height);
}

FontHandle FontManager::get(FontFormat format, Common::UString name, int height) {
	Common::StackLock lock(_mutex);

	// Lock up the name in our alias map first
	std::map<Common::UString, Common::UString>::iterator realName = _aliases.find(name);
	if (realName != _aliases.end())
		name = realName->second;

	Common::UString indexName = name;

	// If we have been given a size, index the font under that size
	if (height > 0)
		indexName = Common::UString::sprintf("%s-%d", name.c_str(), height);

	// Look up the name in our font map
	FontMap::iterator font = _fonts.find(indexName);
	if (font == _fonts.end()) {
		// If not found, load and add that font

		std::pair<FontMap::iterator, bool> result;

		ManagedFont *t = createFont(format, name, height);

		result = _fonts.insert(std::make_pair(indexName, t));

		font = result.first;
	}

	// Increase the reference count and return the font
	font->second->referenceCount++;

	return FontHandle(font);
}

void FontManager::release(FontHandle &handle) {
	Common::StackLock lock(_mutex);

	if (handle.empty)
		return;

	if (--handle.it->second->referenceCount == 0) {
		delete handle.it->second;
		_fonts.erase(handle.it);
	}

	handle.clear();
}

ManagedFont *FontManager::createFont(FontFormat format,
		const Common::UString &name, int height) {

	if (name == kSystemFontMono)
		return new ManagedFont(new TTFFont(Common::getSystemFontMono(), height));

	if (format == kFontFormatUnknown)
		throw Common::Exception("Font format unknown (%s)", name.c_str());

	if (format == kFontFormatTexture)
		return new ManagedFont(new TextureFont(name));
	if (format == kFontFormatABC)
		return new ManagedFont(new ABCFont(name));
	if (format == kFontFormatTTF)
		return new ManagedFont(new TTFFont(name, height));

	throw Common::Exception("Invalid font format %d (%s)", format, name.c_str());
	return 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
