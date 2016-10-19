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
 *  The Aurora texture manager.
 */

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/uuid.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/images/decoder.h"

#include "src/graphics/graphics.h"

#include "src/events/requests.h"

DECLARE_SINGLETON(Graphics::Aurora::TextureManager)

namespace Graphics {

namespace Aurora {

static GLenum kTextureUnit[] = {
	GL_TEXTURE0_ARB,
	GL_TEXTURE1_ARB,
	GL_TEXTURE2_ARB,
	GL_TEXTURE3_ARB,
	GL_TEXTURE4_ARB,
	GL_TEXTURE5_ARB,
	GL_TEXTURE6_ARB,
	GL_TEXTURE7_ARB,
	GL_TEXTURE8_ARB,
	GL_TEXTURE9_ARB,
	GL_TEXTURE10_ARB,
	GL_TEXTURE11_ARB,
	GL_TEXTURE12_ARB,
	GL_TEXTURE13_ARB,
	GL_TEXTURE14_ARB,
	GL_TEXTURE15_ARB,
	GL_TEXTURE16_ARB,
	GL_TEXTURE17_ARB,
	GL_TEXTURE18_ARB,
	GL_TEXTURE19_ARB,
	GL_TEXTURE20_ARB,
	GL_TEXTURE21_ARB,
	GL_TEXTURE22_ARB,
	GL_TEXTURE23_ARB,
	GL_TEXTURE24_ARB,
	GL_TEXTURE25_ARB,
	GL_TEXTURE26_ARB,
	GL_TEXTURE27_ARB,
	GL_TEXTURE28_ARB,
	GL_TEXTURE29_ARB,
	GL_TEXTURE30_ARB,
	GL_TEXTURE31_ARB
};

static const size_t kTextureUnitCount = ARRAYSIZE(kTextureUnit);


TextureManager::TextureManager() : _recordNewTextures(false) {
}

TextureManager::~TextureManager() {
	clear();
}

void TextureManager::clear() {
	Common::StackLock lock(_mutex);

	_bogusTextures.clear();

	for (TextureMap::iterator t = _textures.begin(); t != _textures.end(); ++t)
		delete t->second;
	_textures.clear();

	_recordNewTextures = false;
	_newTextureNames.clear();
}

void TextureManager::addBogusTexture(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	_bogusTextures.insert(name);
}

bool TextureManager::hasTexture(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	if (_bogusTextures.find(name) != _bogusTextures.end())
		return true;
	if (_textures.find(name) != _textures.end())
		return true;

	return false;
}

TextureHandle TextureManager::add(Texture *texture, Common::UString name) {
	Common::StackLock lock(_mutex);

	if (_bogusTextures.find(name) != _bogusTextures.end()) {
		delete texture;
		return TextureHandle();
	}

	if (name.empty())
		name = Common::generateIDRandomString();

	Common::ScopedPtr<ManagedTexture> managedTexture(new ManagedTexture(texture));

	std::pair<TextureMap::iterator, bool> result = _textures.insert(std::make_pair(name, managedTexture.get()));
	if (!result.second)
		throw Common::Exception("Texture \"%s\" already exists", name.c_str());

	managedTexture.release();
	TextureMap::iterator textureIterator = result.first;

	if (_recordNewTextures)
		_newTextureNames.push_back(name);

	return TextureHandle(textureIterator);
}

TextureHandle TextureManager::get(Common::UString name) {
	Common::StackLock lock(_mutex);

	if (_bogusTextures.find(name) != _bogusTextures.end())
		return TextureHandle();

	TextureMap::iterator texture = _textures.find(name);
	if (texture == _textures.end()) {
		std::pair<TextureMap::iterator, bool> result;

		ManagedTexture *managedTexture = new ManagedTexture(Texture::create(name));

		if (managedTexture->texture->isDynamic())
			name = name + "#" + Common::generateIDRandomString();

		result = _textures.insert(std::make_pair(name, managedTexture));

		texture = result.first;
	}

	if (_recordNewTextures)
		_newTextureNames.push_back(name);

	return TextureHandle(texture);
}

TextureHandle TextureManager::getIfExist(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	if (_bogusTextures.find(name) != _bogusTextures.end())
		return TextureHandle();

	TextureMap::iterator texture = _textures.find(name);
	if (texture != _textures.end())
		return TextureHandle(texture);

	return TextureHandle();
}

void TextureManager::startRecordNewTextures() {
	Common::StackLock lock(_mutex);

	_newTextureNames.clear();
	_recordNewTextures = true;
}

void TextureManager::stopRecordNewTextures(std::list<Common::UString> &newTextures) {
	Common::StackLock lock(_mutex);

	_newTextureNames.swap(newTextures);

	_newTextureNames.clear();
	_recordNewTextures = false;
}

void TextureManager::assign(TextureHandle &texture, const TextureHandle &from) {
	Common::StackLock lock(_mutex);

	texture._empty = from._empty;
	texture._it    = from._it;

	if (!texture._empty)
		texture._it->second->referenceCount++;
}

void TextureManager::release(TextureHandle &texture) {
	Common::StackLock lock(_mutex);

	if (!texture._empty && (texture._it != _textures.end())) {
		if (--texture._it->second->referenceCount == 0) {
			delete texture._it->second;
			_textures.erase(texture._it);
		}
	}

	texture._empty = true;
	texture._it    = _textures.end();
}

void TextureManager::reloadAll() {
	Common::StackLock lock(_mutex);

	GfxMan.lockFrame();

	for (TextureMap::iterator texture = _textures.begin(); texture != _textures.end(); ++texture) {
		try {
			texture->second->texture->reload();
		} catch (...) {
			Common::exceptionDispatcherWarning("Failed reloading texture \"%s\"", texture->first.c_str());
		}
	}

	RequestMan.sync();
	GfxMan.unlockFrame();
}

void TextureManager::reset() {
	for (size_t i = 0; i < kTextureUnitCount; i++) {
		activeTexture(i);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP);

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
	}

	activeTexture(0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::set() {
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}

void TextureManager::set(const TextureHandle &handle, TextureMode mode) {
	if (handle.empty()) {
		set();
		return;
	}

	TextureID id = handle._it->second->texture->getID();
	if (id == 0)
		warning("Empty texture ID for texture \"%s\"", handle._it->first.c_str());

	if (handle._it->second->texture->getImage().isCubeMap()) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_CUBE_MAP);
	} else {
		glBindTexture(GL_TEXTURE_2D, id);

		glDisable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_2D);
	}

	switch (mode) {
		case kModeEnvironmentMapReflective:
			if (handle._it->second->texture->getImage().isCubeMap()) {
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
			} else {
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
			}

			break;

		case kModeDiffuse:
		default:
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			break;
	}
}

void TextureManager::activeTexture(size_t n) {
	if ((n >= GfxMan.getMultipleTextureCount()) || (n >= ARRAYSIZE(kTextureUnit)))
		return;

	glActiveTextureARB(kTextureUnit[n]);
}

} // End of namespace Aurora

} // End of namespace Graphics
