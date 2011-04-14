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
#include <list>

#include "graphics/types.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"
#include "common/ustring.h"

namespace Graphics {

namespace Aurora {

class Texture;
class PLTFile;

/** A managed texture, storing how often it's referenced. */
struct ManagedTexture {
	Texture *texture;
	uint32 referenceCount;

	bool reloadable;

	ManagedTexture(const Common::UString &name);
	ManagedTexture(const Common::UString &name, Texture *t);
	~ManagedTexture();
};

/** A managed PLT, storing how often it's referenced. */
struct ManagedPLT {
	PLTFile *plt;
	uint32 referenceCount;

	ManagedPLT(const Common::UString &name);
	~ManagedPLT();
};

typedef std::map<Common::UString, ManagedTexture *> TextureMap;
typedef std::list<ManagedPLT *> PLTList;;

/** A handle to a texture. */
class TextureHandle {
public:
	TextureHandle();
	TextureHandle(TextureMap::iterator &i);
	TextureHandle(const TextureHandle &right);
	~TextureHandle();

	TextureHandle &operator=(const TextureHandle &right);

	bool empty() const;

	void clear();

	Texture &getTexture() const;

private:
	bool _empty;
	TextureMap::iterator _it;

	friend class TextureManager;
};

class PLTHandle {
public:
	PLTHandle();
	PLTHandle(PLTList::iterator &i);
	PLTHandle(const PLTHandle &right);
	~PLTHandle();

	PLTHandle &operator=(const PLTHandle &right);

	bool empty() const;

	void clear();

	PLTFile &getPLT() const;

private:
	bool _empty;
	PLTList::iterator _it;

	friend class TextureManager;
};

/** The global Aurora texture manager. */
class TextureManager : public Common::Singleton<TextureManager> {
public:
	TextureManager();
	~TextureManager();

	void clear();


	TextureHandle add(Texture *texture, Common::UString name = "");
	TextureHandle get(const Common::UString &name);


	void reloadAll();


	void getNewPLTs(std::list<PLTHandle> &plts);
	void clearNewPLTs();


	void reset();
	void set();
	void set(const TextureHandle &handle);


	void activeTexture(uint32 n);
	void textureCoord2f(uint32 n, float u, float v);


private:
	TextureMap _textures;
	PLTList    _plts;

	std::list<PLTHandle> _newPLTs;

	Common::Mutex _mutex;

	void release(TextureMap::iterator &i);
	void release(PLTList::iterator &i);

	friend class PLTHandle;
	friend class TextureHandle;
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::Aurora::TextureManager::instance()

#endif // GRAPHICS_AURORA_TEXTUREMAN_H
