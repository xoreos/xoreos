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

#ifndef GRAPHICS_AURORA_TEXTUREMAN_H
#define GRAPHICS_AURORA_TEXTUREMAN_H

#include <map>
#include <list>

#include "src/graphics/types.h"

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"
#include "src/common/ustring.h"

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
	ManagedTexture(Texture *t);
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
typedef std::list<ManagedPLT *> PLTList;

/** A handle to a texture. */
class TextureHandle {
public:
	TextureHandle();
	TextureHandle(const TextureHandle &right);
	~TextureHandle();

	TextureHandle &operator=(const TextureHandle &right);

	bool empty() const;
	const Common::UString &getName() const;

	void clear();

	Texture &getTexture() const;

private:
	bool _empty;
	TextureMap::iterator _it;

	TextureHandle(TextureMap::iterator &i);

	friend class TextureManager;
};

class PLTHandle {
public:
	PLTHandle();
	PLTHandle(const PLTHandle &right);
	~PLTHandle();

	PLTHandle &operator=(const PLTHandle &right);

	bool empty() const;

	void clear();

	PLTFile &getPLT() const;

private:
	bool _empty;
	PLTList::iterator _it;

	PLTHandle(PLTList::iterator &i);

	friend class TextureManager;
};

/** The global Aurora texture manager. */
class TextureManager : public Common::Singleton<TextureManager> {
public:
	TextureManager();
	~TextureManager();

	// .--- Texture management
	/** Remove and delete all managed textures. */
	void clear();

	/** Does this named managed texture exist? */
	bool hasTexture(const Common::UString &name);

	/** Add this texture to the TextureManager. If name is empty, generate a random one. */
	TextureHandle add(Texture *texture, Common::UString name = "");
	/** Retrieve this named texture, loading it if it's not yet managed. */
	TextureHandle get(const Common::UString &name);
	/** Retrieve this named texture, returning an empty handle if it's not managed. */
	TextureHandle getIfExist(const Common::UString &name);

	/** Reload and rebuild all managed textures, if possible. */
	void reloadAll();

	/** Return all newly created PLT textures. */
	void getNewPLTs(std::list<PLTHandle> &plts);
	/** Return the list of newly created PLT textures. */
	void clearNewPLTs();
	// '---

	// .--- Texture rendering
	/** Bind this texture to the current texture unit. */
	void set(const TextureHandle &handle);
	/** Reset the current texture unit to an empty texture. */
	void set();
	/** Completely reset the texture rendering. */
	void reset();

	/** Set this texture unit as the current one. */
	void activeTexture(uint32 n);
	// '---

private:
	TextureMap _textures;
	PLTList    _plts;

	std::list<PLTHandle> _newPLTs;

	Common::Mutex _mutex;

	void assign(TextureHandle &texture, const TextureHandle &from);
	void assign(PLTHandle &plt, const PLTHandle &from);
	void release(TextureHandle &texture);
	void release(PLTHandle &plt);

	friend class PLTHandle;
	friend class TextureHandle;
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::Aurora::TextureManager::instance()

#endif // GRAPHICS_AURORA_TEXTUREMAN_H
