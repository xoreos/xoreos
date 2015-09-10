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

#include <set>
#include <list>

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/texturehandle.h"

namespace Graphics {

namespace Aurora {

/** The global Aurora texture manager. */
class TextureManager : public Common::Singleton<TextureManager> {
public:
	/** The mode/usage of a specific texture. */
	enum TextureMode {
		kModeDiffuse,                 ///< A standard diffuse texture.
		kModeEnvironmentMapReflective ///< A reflective environment map.
	};

	TextureManager();
	~TextureManager();

	// .--- Texture management
	/** Remove and delete all managed textures. */
	void clear();

	/** Add the name of a texture that doesn't really exist.
	 *
	 *  An empty TextureHandle is returned when this texture is requested.
	 */
	void addBogusTexture(const Common::UString &name);

	/** Does this named managed texture exist? */
	bool hasTexture(const Common::UString &name);

	/** Add this texture to the TextureManager. If name is empty, generate a random one. */
	TextureHandle add(Texture *texture, Common::UString name = "");
	/** Retrieve this named texture, loading it if it's not yet managed. */
	TextureHandle get(Common::UString name);
	/** Retrieve this named texture, returning an empty handle if it's not managed. */
	TextureHandle getIfExist(const Common::UString &name);

	/** Start recording all names of newly created textures. */
	void startRecordNewTextures();
	/** Stop the recording of texture names, and return a list of previously recorded names. */
	void stopRecordNewTextures(std::list<Common::UString> &newTextures);

	/** Reload and rebuild all managed textures, if possible. */
	void reloadAll();
	// '---

	// .--- Texture rendering
	/** Bind this texture to the current texture unit. */
	void set(const TextureHandle &handle, TextureMode mode = kModeDiffuse);
	/** Reset the current texture unit to an empty texture. */
	void set();
	/** Completely reset the texture rendering. */
	void reset();

	/** Set this texture unit as the current one. */
	void activeTexture(size_t n);
	// '---

private:
	TextureMap _textures;

	std::set<Common::UString> _bogusTextures;

	Common::Mutex _mutex;

	bool _recordNewTextures;
	std::list<Common::UString> _newTextureNames;

	void assign(TextureHandle &texture, const TextureHandle &from);
	void release(TextureHandle &texture);

	friend class TextureHandle;
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::Aurora::TextureManager::instance()

#endif // GRAPHICS_AURORA_TEXTUREMAN_H
