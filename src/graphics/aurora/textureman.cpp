/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/texture.h"

DECLARE_SINGLETON(Graphics::Aurora::TextureManager)

namespace Graphics {

namespace Aurora {

ManagedTexture::ManagedTexture(const Common::UString &name) {
	referenceCount = 0;
	texture = new Texture(name);
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

void TextureHandle::clear() {
	empty = true;
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

} // End of namespace Aurora

} // End of namespace Graphics
