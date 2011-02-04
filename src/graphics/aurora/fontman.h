/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/fontman.h
 *  The Aurora font manager.
 */

#ifndef GRAPHICS_AURORA_FONTMAN_H
#define GRAPHICS_AURORA_FONTMAN_H

#include <map>

#include "graphics/types.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"
#include "common/ustring.h"

namespace Graphics {

namespace Aurora {

class Font;

struct ManagedFont {
	Font *font;
	uint32 referenceCount;

	ManagedFont(const Common::UString &name);
	~ManagedFont();
};

typedef std::map<Common::UString, ManagedFont *> FontMap;

struct FontHandle {
	bool empty;
	FontMap::iterator it;

	FontHandle();
	FontHandle(FontMap::iterator &i);
	FontHandle(const FontHandle &right);
	~FontHandle();

	FontHandle &operator=(const FontHandle &right);

	void clear();

	const Font &getFont();
};

class FontManager : public Common::Singleton<FontManager> {
public:
	FontManager();
	~FontManager();

	void clear();

	FontHandle get(const Common::UString &name);
	void release(FontHandle &handle);

private:
	FontMap _fonts;

	Common::Mutex _mutex;
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the font manager. */
#define FontMan Graphics::Aurora::FontManager::instance()

#endif // GRAPHICS_AURORA_FONTMAN_H
