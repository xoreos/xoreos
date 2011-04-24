/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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

class Font;

namespace Aurora {

/** The format of a font. */
enum FontFormat {
	kFontFormatUnknown = 0, ///< Unknown font format.
	kFontFormatTexture    , ///< Textured font, used by NWN and KotOR/KotOR2
	kFontFormatABC        , ///< ABC/SBM font, used by Jade Empire.
	kFontFormatTTF          ///< TTF font, used by NWN2.
};

/** A managed font, storing how often it's referenced. */
struct ManagedFont {
	Font *font;
	uint32 referenceCount;

	ManagedFont(Font *f);
	~ManagedFont();
};

typedef std::map<Common::UString, ManagedFont *> FontMap;

/** A handle to a font. */
struct FontHandle {
	bool empty;
	FontMap::iterator it;

	FontHandle();
	FontHandle(FontMap::iterator &i);
	FontHandle(const FontHandle &right);
	~FontHandle();

	FontHandle &operator=(const FontHandle &right);

	void clear();

	const Common::UString &getFontName() const;
	const Font &getFont() const;
};

/** The global Aurora font manager. */
class FontManager : public Common::Singleton<FontManager> {
public:
	FontManager();
	~FontManager();

	void clear();

	void setFormat(FontFormat format);

	/** Add an alias for a specific font name. */
	void addAlias(const Common::UString &alias, const Common::UString &realName);

	FontHandle get(Common::UString name);
	void release(FontHandle &handle);

private:
	FontFormat _format;

	std::map<Common::UString, Common::UString> _aliases;

	FontMap _fonts;

	Common::Mutex _mutex;

	ManagedFont *createFont(const Common::UString &name);
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the font manager. */
#define FontMan Graphics::Aurora::FontManager::instance()

#endif // GRAPHICS_AURORA_FONTMAN_H
