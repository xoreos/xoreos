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

/**
 * @file
 * A loader class for texture atlas XML files for Dragon Age: Origins and Dragon Age 2.
 */

#ifndef AURORA_TEXTUREATLASFILE_H
#define AURORA_TEXTUREATLASFILE_H

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef ENABLE_XML

#include <map>

namespace Aurora {

class TextureAtlasFile {
public:
	TextureAtlasFile(Common::SeekableReadStream &stream);
	TextureAtlasFile(Common::UString name);

	bool isAtlasTexture(const Common::UString &texture) const;
	void getAtlasTexture(const Common::UString &texture,
	                     Common::UString &textureFile, float &x, float &y, float &w, float &h) const;

private:
	void load(Common::SeekableReadStream &stream);

	struct AtlasTexture {
		Common::UString textureFile;
		float x, y, w, h;
	};

	std::map<Common::UString, AtlasTexture> _atlasTextures;
};

} // End of namespace Aurora

#endif // ENABLE_XML

#endif // AURORA_TEXTUREATLASFILE_H
