/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/textureman.h
 *  A texture manager.
 */

#ifndef GRAPHICS_TEXTUREMAN_H
#define GRAPHICS_TEXTUREMAN_H

#include <OgrePrerequisites.h>
#include <OgrePixelFormat.h>

#include "common/singleton.h"
#include "common/stringmap.h"

namespace Graphics {

class ImageDecoder;

/** Material properties of a texture.
 *
 *  Properties, like the blending types or the environment map, are
 *  generally read from a TXI either directly from within the texture resource,
 *  or a separate TXI resource.
 */
class TextureProperties {
public:
	TextureProperties();
	~TextureProperties();

	Common::UString getString(const Common::UString &key, const Common::UString &def = "") const;
	void setString(const Common::UString &key, const Common::UString &value);

	bool   getBool  (const Common::UString &key, bool   def = false) const;
	int    getInt   (const Common::UString &key, int    def = 0    ) const;
	uint   getUInt  (const Common::UString &key, uint   def = 0    ) const;
	double getDouble(const Common::UString &key, double def = 0.0  ) const;

	void setBool  (const Common::UString &key, bool   value);
	void setInt   (const Common::UString &key, int    value);
	void setUInt  (const Common::UString &key, uint   value);
	void setDouble(const Common::UString &key, double value);

private:
	Common::StringIMap _properties;
};

/** The global texture manager.
 *
 *  Texture are read from different image resources and stored here.
 */
class TextureManager : public Common::Singleton<TextureManager> {
public:
	TextureManager();
	~TextureManager();

	void clear();

	/** Get/Load a texture. */
	Ogre::TexturePtr get(const Common::UString &name);

	/** Get the properties of a texture. */
	const TextureProperties &getProperties(const Common::UString &name);
	/** Get the properties of a texture. */
	const TextureProperties &getProperties(const Ogre::TexturePtr &texture);

private:
	typedef std::map<Common::UString, TextureProperties *> Properties;

	Properties _properties;


	Ogre::TexturePtr create(const Common::UString &name);
	Ogre::TexturePtr create(const Common::UString &name, const ImageDecoder &image);

	Ogre::PixelFormat getPixelFormat(const ImageDecoder &image);

	void convert(Ogre::TexturePtr &texture, const ImageDecoder &image, int mipMaps);
};

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::TextureManager::instance()

#endif // GRAPHICS_TEXTUREMAN_H
