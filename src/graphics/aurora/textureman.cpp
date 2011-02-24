/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/textureman.cpp
 *  The Aurora texture manager.
 */

#include "common/util.h"
#include "common/error.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/texture.h"

#include "graphics/graphics.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::Aurora::TextureManager)

namespace Graphics {

namespace Aurora {

ManagedTexture::ManagedTexture(const Common::UString &name) {
	referenceCount = 0;
	texture = new Texture(name);
}

ManagedTexture::ManagedTexture(const Common::UString &name, Texture *t) {
	referenceCount = 0;
	texture = t;
}

ManagedTexture::~ManagedTexture() {
	delete texture;
}


TextureHandle::TextureHandle() {
	empty = true;
}

TextureHandle::TextureHandle(TextureMap::iterator &i) {
	empty = false;
	it    = i;
}

TextureHandle::TextureHandle(const TextureHandle &right) : empty(true) {
	*this = right;
}

TextureHandle::~TextureHandle() {
	TextureMan.release(*this);
}

TextureHandle &TextureHandle::operator=(const TextureHandle &right) {
	if (this == &right)
		return *this;

	TextureMan.release(*this);

	empty = right.empty;
	it    = right.it;

	if (!empty)
		it->second->referenceCount++;

	return *this;
}

void TextureHandle::clear() {
	empty = true;
}

const Texture &TextureHandle::getTexture() const {
	assert(!empty);

	return *it->second->texture;
}


TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	clear();
}

void TextureManager::clear() {
	Common::StackLock lock(_mutex);

	for (TextureMap::iterator texture = _textures.begin(); texture != _textures.end(); ++texture)
		delete texture->second;

	_textures.clear();
}

TextureHandle TextureManager::add(const Common::UString &name, Texture *texture) {
	Common::StackLock lock(_mutex);

	TextureMap::iterator text = _textures.find(name);
	if (text != _textures.end())
		throw Common::Exception("Texture \"%s\" already exists", name.c_str());

	std::pair<TextureMap::iterator, bool> result;

	ManagedTexture *t = new ManagedTexture(name, texture);

	result = _textures.insert(std::make_pair(name, t));

	text = result.first;

	text->second->referenceCount++;

	return TextureHandle(text);
}

TextureHandle TextureManager::get(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	TextureMap::iterator texture = _textures.find(name);
	if (texture == _textures.end()) {
		std::pair<TextureMap::iterator, bool> result;

		ManagedTexture *t = new ManagedTexture(name);

		result = _textures.insert(std::make_pair(name, t));

		texture = result.first;
	}

	texture->second->referenceCount++;

	return TextureHandle(texture);
}

void TextureManager::release(TextureHandle &handle) {
	Common::StackLock lock(_mutex);

	if (handle.empty)
		return;

	if (--handle.it->second->referenceCount == 0) {
		RequestMan.sync();

		delete handle.it->second;
		_textures.erase(handle.it);
	}

	handle.clear();
}

void TextureManager::set() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::set(const TextureHandle &handle) {
	if (handle.empty) {
		set();
		return;
	}

	glBindTexture(GL_TEXTURE_2D, handle.it->second->texture->getID());
}

static GLenum texture[32] = {
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

void TextureManager::activeTexture(uint32 n) {
	if (n >= ARRAYSIZE(texture))
		return;

	if (GfxMan.supportMultipleTextures())
		glActiveTextureARB(texture[n]);
}

void TextureManager::textureCoord2f(uint32 n, float u, float v) {
	if (n >= ARRAYSIZE(texture))
		return;

	if (n == 0) {
		glTexCoord2f(u, v);
		return;
	}

	if (GfxMan.supportMultipleTextures())
		glMultiTexCoord2fARB(texture[n], u, v);
}

} // End of namespace Aurora

} // End of namespace Graphics
