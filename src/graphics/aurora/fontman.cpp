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
 *  The Aurora font manager.
 */

#include "src/common/scopedptr.h"
#include "src/common/error.h"
#include "src/common/systemfonts.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/texturefont.h"
#include "src/graphics/aurora/abcfont.h"
#include "src/graphics/aurora/ttffont.h"
#include "src/graphics/aurora/nftrfont.h"

DECLARE_SINGLETON(Graphics::Aurora::FontManager)

namespace Graphics {

namespace Aurora {

const char *kSystemFontMono = "_xoreosSystemFontMono";

FontManager::FontManager() : _format(kFontFormatUnknown) {
}

FontManager::~FontManager() {
	clear();
}

void FontManager::clear() {
	Common::StackLock lock(_mutex);

	_fonts.clear();

	_format = kFontFormatUnknown;
	_aliases.clear();
}

void FontManager::setFormat(FontFormat format) {
	Common::StackLock lock(_mutex);

	_format = format;
}

void FontManager::addAlias(const Common::UString &alias, const Common::UString &realName) {
	Common::StackLock lock(_mutex);

	_aliases[alias] = realName;
}

bool FontManager::hasFont(const Common::UString &name, int height) {
	Common::StackLock lock(_mutex);

	FontMap::const_iterator font = _fonts.find(getIndexName(name, height));

	return font != _fonts.end();
}

FontHandle FontManager::add(Font *font, const Common::UString &name) {
	Common::StackLock lock(_mutex);

	Common::ScopedPtr<ManagedFont> managedFont(new ManagedFont(font));

	std::pair<FontMap::iterator, bool> result = _fonts.insert(std::make_pair(name, managedFont.get()));
	if (!result.second)
		throw Common::Exception("Font \"%s\" already exists", name.c_str());

	managedFont.release();
	FontMap::iterator fontIterator = result.first;

	return FontHandle(fontIterator);
}

FontHandle FontManager::get(const Common::UString &name, int height) {
	return get(_format, name, height);
}

FontHandle FontManager::get(FontFormat format, const Common::UString &name, int height) {
	Common::StackLock lock(_mutex);

	Common::UString aliasName = getAliasName(name);
	Common::UString indexName = getIndexName(name, height);

	FontMap::iterator font = _fonts.find(indexName);
	if (font == _fonts.end()) {
		std::pair<FontMap::iterator, bool> result;

		ManagedFont *f = createFont(format, aliasName, height);

		result = _fonts.insert(std::make_pair(indexName, f));

		font = result.first;
	}

	return FontHandle(font);
}

FontHandle FontManager::getIfExist(const Common::UString &name, int height) {
	Common::StackLock lock(_mutex);

	FontMap::iterator font = _fonts.find(getIndexName(name, height));
	if (font != _fonts.end())
		return FontHandle(font);

	return FontHandle();
}

Common::UString FontManager::getAliasName(const Common::UString &name) {
	std::map<Common::UString, Common::UString>::iterator realName = _aliases.find(name);
	if (realName != _aliases.end())
		return realName->second;

	return name;
}

Common::UString FontManager::getIndexName(Common::UString name, int height) {
	// Lock up the name in our alias map first
	name = getAliasName(name);

	if (height <= 0)
		return name;

	// If we have been given a height, the font is indexed with the height
	return Common::UString::format("%s-%d", name.c_str(), height);
}

void FontManager::assign(FontHandle &font, const FontHandle &from) {
	Common::StackLock lock(_mutex);

	font._empty = from._empty;
	font._it    = from._it;

	if (!font._empty)
		font._it->second->referenceCount++;
}

void FontManager::release(FontHandle &font) {
	Common::StackLock lock(_mutex);

	if (!font._empty && (font._it != _fonts.end()))
		if (--font._it->second->referenceCount == 0)
			_fonts.erase(font._it);

	font._empty = true;
	font._it    = _fonts.end();
}

ManagedFont *FontManager::createFont(FontFormat format,
		const Common::UString &name, int height) {

	try {
		if (name == kSystemFontMono)
			return new ManagedFont(new TTFFont(Common::getSystemFontMono(), height));

		if (format == kFontFormatUnknown)
			throw Common::Exception("Font format unknown");

		if (format == kFontFormatTexture)
			return new ManagedFont(new TextureFont(name));
		if (format == kFontFormatABC)
			return new ManagedFont(new ABCFont(name));
		if (format == kFontFormatTTF)
			return new ManagedFont(new TTFFont(name, height));
		if (format == kFontFormatNFTR)
			return new ManagedFont(new NFTRFont(name));

		throw Common::Exception("Invalid font format %d", format);

	} catch (Common::Exception &e) {
		e.add("Failed to create font \"%s\" (%d)", name.c_str(), format);
		throw;
	}

	return 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
