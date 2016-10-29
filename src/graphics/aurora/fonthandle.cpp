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

#include <cassert>

#include "src/graphics/font.h"

#include "src/graphics/aurora/fonthandle.h"
#include "src/graphics/aurora/fontman.h"

namespace Graphics {

namespace Aurora {

ManagedFont::ManagedFont(Font *f) : font(f), referenceCount(0) {
}


FontHandle::FontHandle() : _empty(true) {
}

FontHandle::FontHandle(FontMap::iterator &i) : _empty(false), _it(i) {
	_it->second->referenceCount++;
}

FontHandle::FontHandle(const FontHandle &right) : _empty(true) {
	*this = right;
}

FontHandle::~FontHandle() {
	clear();
}

FontHandle &FontHandle::operator=(const FontHandle &right) {
	if (this == &right)
		return *this;

	clear();

	FontMan.assign(*this, right);

	return *this;
}

bool FontHandle::empty() const {
	return _empty;
}

const Common::UString kEmptyString;
const Common::UString &FontHandle::getName() const {
	if (_empty)
		return kEmptyString;

	return _it->first;
}

void FontHandle::clear() {
	FontMan.release(*this);
}

Font &FontHandle::getFont() const {
	assert(!_empty);

	return *_it->second->font;
}

} // End of namespace Aurora

} // End of namespace Graphics
