/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/textureman.h
 *  The Aurora texture manager.
 */

#ifndef GRAPHICS_AURORA_TEXTUREMAN_H
#define GRAPHICS_AURORA_TEXTUREMAN_H

#include <map>

#include "graphics/types.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"
#include "common/ustring.h"

namespace Graphics {

namespace Aurora {

class Texture;

/** A managed texture, storing how often it's referenced. */
struct ManagedTexture {
	Texture *texture;
	uint32 referenceCount;

	ManagedTexture(const Common::UString &name);
	ManagedTexture(const Common::UString &name, Texture *t);
	~ManagedTexture();
};

typedef std::map<Common::UString, ManagedTexture *> TextureMap;

/** A handle to a texture. */
struct TextureHandle {
	bool empty;
	TextureMap::iterator it;

	TextureHandle();
	TextureHandle(TextureMap::iterator &i);
	TextureHandle(const TextureHandle &right);
	~TextureHandle();

	TextureHandle &operator=(const TextureHandle &right);

	void clear();

	const Texture &getTexture() const;
};

/** The global Aurora texture manager. */
class TextureManager : public Common::Singleton<TextureManager> {
public:
	TextureManager();
	~TextureManager();

	void clear();

	TextureHandle add(const Common::UString &name, Texture *texture);

	TextureHandle get(const Common::UString &name);
	void release(TextureHandle &handle);

	void reloadAll();

	void set();
	void set(const TextureHandle &handle);

	void activeTexture(uint32 n);

	void textureCoord2f(uint32 n, float u, float v);

private:
	TextureMap _textures;

	Common::Mutex _mutex;
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::Aurora::TextureManager::instance()

#endif // GRAPHICS_AURORA_TEXTUREMAN_H
