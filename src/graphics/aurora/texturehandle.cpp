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
 *  A handle to an Aurora texture.
 */

#include <cassert>

#include "src/graphics/aurora/texturehandle.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

ManagedTexture::ManagedTexture(Texture *t) : texture(t), referenceCount(0) {
}

ManagedTexture::~ManagedTexture() {
	delete texture;
}


TextureHandle::TextureHandle() : _empty(true) {
}

TextureHandle::TextureHandle(TextureMap::iterator &i) : _empty(false), _it(i) {
	_it->second->referenceCount++;
}

TextureHandle::TextureHandle(const TextureHandle &right) : _empty(true) {
	*this = right;
}

TextureHandle::~TextureHandle() {
	clear();
}

TextureHandle &TextureHandle::operator=(const TextureHandle &right) {
	if (this == &right)
		return *this;

	clear();

	TextureMan.assign(*this, right);

	return *this;
}

bool TextureHandle::empty() const {
	return _empty;
}

const Common::UString kEmptyString;
const Common::UString &TextureHandle::getName() const {
	if (_empty)
		return kEmptyString;

	return _it->first;
}

void TextureHandle::clear() {
	TextureMan.release(*this);
}

Texture &TextureHandle::getTexture() const {
	assert(!_empty);

	return *_it->second->texture;
}

} // End of namespace Aurora

} // End of namespace Graphics
