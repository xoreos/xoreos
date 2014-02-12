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
#include <OgreTexture.h>

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
	/** Coordinates. */
	struct Coords {
		float x, y, z;
	};

	TextureProperties();
	~TextureProperties();

	Common::UString getString(const Common::UString &key, const Common::UString &def = "") const;
	void setString(const Common::UString &key, const Common::UString &value);

	bool loadFromTXI(Common::SeekableReadStream &stream);
	bool loadFromTXI(const ImageDecoder &img);
	bool loadFromTXI(const Common::UString &file);

	bool   getBool  (const Common::UString &key, bool   def = false) const;
	int    getInt   (const Common::UString &key, int    def = 0    ) const;
	uint   getUInt  (const Common::UString &key, uint   def = 0    ) const;
	double getDouble(const Common::UString &key, double def = 0.0  ) const;

	void setBool  (const Common::UString &key, bool   value);
	void setInt   (const Common::UString &key, int    value);
	void setUInt  (const Common::UString &key, uint   value);
	void setDouble(const Common::UString &key, double value);

	const std::vector<Coords> &getUpperLeftCoords() const;
	const std::vector<Coords> &getLowerRightCoords() const;

private:
	Common::StringIMap _properties;

	std::vector<Coords> _coordsUpperLeft;
	std::vector<Coords> _coordsLowerRight;
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

	/** Create a dynamic texture. */
	Ogre::TexturePtr createDynamic(Ogre::TextureType type, uint width, uint height,
			int mipMaps, Ogre::PixelFormat format, int usage);

	/** Get the properties of a texture. */
	const TextureProperties &getProperties(const Common::UString &name);
	/** Get the properties of a texture. */
	const TextureProperties &getProperties(const Ogre::TexturePtr &texture);

	/** Dump a texture into a TGA file. */
	bool dumpTGA(const Common::UString &name, const Common::UString &fileName);

	/** Reload all textures from their image resources. */
	void reloadAll();

private:
	typedef std::map<Common::UString, TextureProperties *> Properties;

	Properties _properties;


	Ogre::TexturePtr create(const Common::UString &imageName, const Common::UString &textureName);
	Ogre::TexturePtr create(const Common::UString &imageName, const Common::UString &textureName, const ImageDecoder &image);

	ImageDecoder *createImage(const Common::UString &imageName, const Common::UString &textureName);

	void convert(Ogre::TexturePtr &texture, const ImageDecoder &image, int mipMaps);

	void reload(Ogre::TexturePtr &texture);
	void reload(Ogre::TexturePtr &texture, const ImageDecoder &image);

	static bool isDynamic(const Ogre::TexturePtr &texture);

	static Common::UString canonicalName(const Common::UString &texture);
	static Common::UString dynamicName();

	Common::UString getImageName(const Ogre::TexturePtr &texture);
};

} // End of namespace Graphics

/** Shortcut for accessing the texture manager. */
#define TextureMan Graphics::TextureManager::instance()

#endif // GRAPHICS_TEXTUREMAN_H
