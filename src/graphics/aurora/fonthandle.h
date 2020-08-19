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
 *  A handle to an Aurora font.
 */

#ifndef GRAPHICS_AURORA_FONTHANDLE_H
#define GRAPHICS_AURORA_FONTHANDLE_H

#include <map>
#include <memory>

#include "src/common/types.h"
#include "src/common/ptrmap.h"
#include "src/common/ustring.h"

namespace Graphics {

class Font;

namespace Aurora {

/** A managed font, storing how often it's referenced. */
struct ManagedFont {
	std::unique_ptr<Font> font;
	uint32_t referenceCount;

	ManagedFont(Font *f);
};

typedef Common::PtrMap<Common::UString, ManagedFont> FontMap;

/** A handle to a font. */
class FontHandle {
public:
	FontHandle();
	FontHandle(const FontHandle &right);
	~FontHandle();

	FontHandle &operator=(const FontHandle &right);

	bool empty() const;
	const Common::UString &getName() const;

	void clear();

	Font &getFont() const;

private:
	bool _empty;
	FontMap::iterator _it;

	FontHandle(FontMap::iterator &i);

	friend class FontManager;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_FONTHANDLE_H
