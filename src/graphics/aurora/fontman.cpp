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

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/texturefont.h"

DECLARE_SINGLETON(Graphics::Aurora::FontManager)

namespace Graphics {

namespace Aurora {

ManagedFont::ManagedFont(const Common::UString &name) {
	referenceCount = 0;
	font = new TextureFont(name);
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

const Font &FontHandle::getFont() const {
	assert(!empty);

	return *it->second->font;
}


FontManager::FontManager() {
}

FontManager::~FontManager() {
	clear();
}

void FontManager::clear() {
	Common::StackLock lock(_mutex);

	for (FontMap::iterator font = _fonts.begin(); font != _fonts.end(); ++font)
		delete font->second;

	_fonts.clear();
}

FontHandle FontManager::get(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	FontMap::iterator font = _fonts.find(name);
	if (font == _fonts.end()) {
		std::pair<FontMap::iterator, bool> result;

		ManagedFont *t = new ManagedFont(name);

		result = _fonts.insert(std::make_pair(name, t));

		font = result.first;
	}

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

} // End of namespace Aurora

} // End of namespace Graphics
