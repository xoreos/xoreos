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

/** The format of a font. */
enum FontFormat {
	kFontFormatUnknown = 0, ///< Unknown font format.
	kFontFormatTexture    , ///< Textured font, used by NWN and KotOR/KotOR2
	kFontFormatABC        , ///< ABC/SBM font, used by Jade Empire.
	kFontFormatTTF          ///< TTF font, used by NWN2.
};

struct ManagedFont {
	Font *font;
	uint32 referenceCount;

	ManagedFont(Font *f);
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

	const Font &getFont() const;
};

class FontManager : public Common::Singleton<FontManager> {
public:
	FontManager();
	~FontManager();

	void clear();

	void setFormat(FontFormat format);

	FontHandle get(const Common::UString &name);
	void release(FontHandle &handle);

private:
	FontFormat _format;

	FontMap _fonts;

	Common::Mutex _mutex;

	ManagedFont *createFont(const Common::UString &name);
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the font manager. */
#define FontMan Graphics::Aurora::FontManager::instance()

#endif // GRAPHICS_AURORA_FONTMAN_H
