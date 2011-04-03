/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/fontman.cpp
 *  The Aurora font manager.
 */

#include "common/error.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/texturefont.h"
#include "graphics/aurora/abcfont.h"

DECLARE_SINGLETON(Graphics::Aurora::FontManager)

namespace Graphics {

namespace Aurora {

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

const Font &FontHandle::getFont() const {
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

FontHandle FontManager::get(Common::UString name) {
	Common::StackLock lock(_mutex);

	// Lock up the name in our alias map first
	std::map<Common::UString, Common::UString>::iterator realName = _aliases.find(name);
	if (realName != _aliases.end())
		name = realName->second;

	// Look up the name in our font map
	FontMap::iterator font = _fonts.find(name);
	if (font == _fonts.end()) {
		// If not found, load and add that font

		std::pair<FontMap::iterator, bool> result;

		ManagedFont *t = createFont(name);

		result = _fonts.insert(std::make_pair(name, t));

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

ManagedFont *FontManager::createFont(const Common::UString &name) {
	if (_format == kFontFormatUnknown)
		throw Common::Exception("Font format unknown (%s)", name.c_str());

	if (_format == kFontFormatTexture)
		return new ManagedFont(new TextureFont(name));
	if (_format == kFontFormatABC)
		return new ManagedFont(new ABCFont(name));
	if (_format == kFontFormatTTF)
		throw Common::Exception("TODO: Load TTF font (%s)", name.c_str());

	throw Common::Exception("Invalid font format %d (%s)", _format, name.c_str());
	return 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
