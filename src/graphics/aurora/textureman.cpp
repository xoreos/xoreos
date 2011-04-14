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
#include "common/uuid.h"

#include "aurora/resman.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/texture.h"
#include "graphics/aurora/pltfile.h"

#include "graphics/graphics.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::Aurora::TextureManager)

namespace Graphics {

namespace Aurora {

ManagedTexture::ManagedTexture(const Common::UString &name) : reloadable(false) {
	referenceCount = 0;
	texture = new Texture(name);
}

ManagedTexture::ManagedTexture(const Common::UString &name, Texture *t) : reloadable(false) {
	referenceCount = 0;
	texture = t;
}

ManagedTexture::~ManagedTexture() {
	delete texture;
}


ManagedPLT::ManagedPLT(const Common::UString &name) {
	referenceCount = 0;
	plt = new PLTFile(name);
}

ManagedPLT::~ManagedPLT() {
	delete plt;
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

	_empty = right._empty;
	_it    = right._it;

	if (!_empty)
		_it->second->referenceCount++;

	return *this;
}

bool TextureHandle::empty() const {
	return _empty;
}

void TextureHandle::clear() {
	if (_empty)
		return;

	if (--_it->second->referenceCount == 0) {
		TextureMan.release(_it);
		_empty = true;
	}
}

Texture &TextureHandle::getTexture() const {
	assert(!_empty);

	return *_it->second->texture;
}


PLTHandle::PLTHandle() : _empty(true) {
}

PLTHandle::PLTHandle(PLTList::iterator &i) : _empty(false), _it(i) {
	(*_it)->referenceCount++;
}

PLTHandle::PLTHandle(const PLTHandle &right) : _empty(true) {
	*this = right;
}

PLTHandle::~PLTHandle() {
	clear();
}

PLTHandle &PLTHandle::operator=(const PLTHandle &right) {
	if (this == &right)
		return *this;

	clear();

	_empty = right._empty;
	_it    = right._it;

	if (!_empty)
		(*_it)->referenceCount++;

	return *this;
}

bool PLTHandle::empty() const {
	return _empty;
}

void PLTHandle::clear() {
	if (_empty)
		return;

	if (--(*_it)->referenceCount == 0) {
		TextureMan.release(_it);
		_empty = true;
	}
}

PLTFile &PLTHandle::getPLT() const {
	assert(!_empty);

	return *(*_it)->plt;
}


TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	clear();
}

void TextureManager::clear() {
	Common::StackLock lock(_mutex);

	_newPLTs.clear();

	for (PLTList::iterator p = _plts.begin(); p != _plts.end(); ++p)
		delete *p;
	_plts.clear();

	for (TextureMap::iterator t = _textures.begin(); t != _textures.end(); ++t)
		delete t->second;
	_textures.clear();
}

TextureHandle TextureManager::add(Texture *texture, Common::UString name) {
	Common::StackLock lock(_mutex);

	bool reloadable = true;
	if (name.empty()) {
		reloadable = false;

		name = Common::generateIDRandomString();
	}

	TextureMap::iterator text = _textures.find(name);
	if (text != _textures.end())
		throw Common::Exception("Texture \"%s\" already exists", name.c_str());

	std::pair<TextureMap::iterator, bool> result;

	ManagedTexture *t = new ManagedTexture(name, texture);

	result = _textures.insert(std::make_pair(name, t));

	text = result.first;

	text->second->reloadable = reloadable;

	return TextureHandle(text);
}

TextureHandle TextureManager::get(const Common::UString &name) {
	Common::StackLock lock(_mutex);

	if (ResMan.hasResource(name, ::Aurora::kFileTypePLT)) {
		_plts.push_back(new ManagedPLT(name));

		_newPLTs.push_back(PLTHandle(--_plts.end()));

		return _newPLTs.back().getPLT().getTexture();
	}

	TextureMap::iterator texture = _textures.find(name);
	if (texture == _textures.end()) {
		std::pair<TextureMap::iterator, bool> result;

		ManagedTexture *t = new ManagedTexture(name);

		result = _textures.insert(std::make_pair(name, t));

		texture = result.first;

		texture->second->reloadable = true;
	}

	return TextureHandle(texture);
}

void TextureManager::release(TextureMap::iterator &i) {
	Common::StackLock lock(_mutex);

	if (i == _textures.end())
		return;

	RequestMan.sync();

	delete i->second;
	_textures.erase(i);
	i = _textures.end();
}

void TextureManager::release(PLTList::iterator &i) {
	Common::StackLock lock(_mutex);

	if (i == _plts.end())
		return;

	RequestMan.sync();

	delete *i;
	_plts.erase(i);
	i = _plts.end();
}

void TextureManager::reloadAll() {
	Common::StackLock lock(_mutex);

	GfxMan.lockFrame();

	TextureMap::iterator texture;
	try {

		for (texture = _textures.begin(); texture != _textures.end(); ++texture)
			if (texture->second->reloadable)
				texture->second->texture->reload(texture->first);

	} catch (Common::Exception &e) {
		e.add("Failed reloading texture \"%s\"", texture->first.c_str());
		throw;
	}

	RequestMan.sync();
	GfxMan.unlockFrame();
}

void TextureManager::getNewPLTs(std::list<PLTHandle> &plts) {
	for (std::list<PLTHandle>::const_iterator p = _newPLTs.begin(); p != _newPLTs.end(); ++p)
		plts.push_back(*p);
	_newPLTs.clear();
}

void TextureManager::clearNewPLTs() {
	_newPLTs.clear();
}

void TextureManager::reset() {
	activeTexture(0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::set() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::set(const TextureHandle &handle) {
	if (handle.empty()) {
		set();
		return;
	}

	TextureID id = handle._it->second->texture->getID();
	if (id == 0)
		warning("Empty texture ID for texture \"%s\"", handle._it->first.c_str());

	glBindTexture(GL_TEXTURE_2D, id);
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
