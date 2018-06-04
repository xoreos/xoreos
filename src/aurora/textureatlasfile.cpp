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

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef ENABLE_XML
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/xml.h"
#include "src/common/streamtokenizer.h"
#include "src/common/strutil.h"

#include "src/aurora/resman.h"
#include "src/aurora/textureatlasfile.h"

namespace Aurora {

TextureAtlasFile::TextureAtlasFile(Common::UString name) {
	Common::ScopedPtr<Common::SeekableReadStream> stream(ResMan.getResource(name, kFileTypeXML));
	load(*stream);
}

TextureAtlasFile::TextureAtlasFile(Common::SeekableReadStream &stream) {
	load(stream);
}

bool TextureAtlasFile::isAtlasTexture(const Common::UString &texture) const {
	return _atlasTextures.find(texture) != _atlasTextures.end();
}

void TextureAtlasFile::getAtlasTexture(const Common::UString &texture,
                                       Common::UString &textureFile, float &x, float &y, float &w, float &h) const {
	std::map<Common::UString, AtlasTexture>::const_iterator iter = _atlasTextures.find(texture);
	if (iter == _atlasTextures.end()) {
		throw Common::Exception("Atlas Texture %s not found", texture.c_str());
	}

	const AtlasTexture &atlasTexture = iter->second;
	textureFile = atlasTexture.textureFile;
	x = atlasTexture.x;
	y = atlasTexture.y;
	w = atlasTexture.w;
	h = atlasTexture.h;
}

void TextureAtlasFile::load(Common::SeekableReadStream &stream) {
	Common::XMLParser parser(stream);

	const Common::XMLNode &node = parser.getRoot();

	if(node.getName() != "AtlasData")
		throw Common::Exception("Invalid tag, <AtlasData> expected, <%s> found", node.getName().c_str());

	const Common::XMLNode::Children &atlasTextures = node.getChildren();
	for (Common::XMLNode::Children::const_iterator a = atlasTextures.begin(); a != atlasTextures.end(); ++a) {
		if ((*a)->getName() != "AtlasTexture")
			throw Common::Exception("Invalid tag, <AtlasTexture> expected, <%s> found", (*a)->getName().c_str());

		Common::UString atlasTexture = (*a)->getProperty("Name");

		const Common::XMLNode::Children &sourceTextures = (*a)->getChildren();
		for (Common::XMLNode::Children::const_iterator s = sourceTextures.begin();
		     s != sourceTextures.end(); ++s) {
			if ((*s)->getName() != "SourceTexture")
				throw Common::Exception("Invalid tag, <SourceTexture> expected, <%s> found", (*s)->getName().c_str());

			const Common::UString sourceTexture = (*s)->getProperty("Name");
			const Common::UString offsetAndScale = (*s)->getProperty("OffsetAndScale");
			const Common::UString offsetAndScaleV2 = (*s)->getProperty("OffsetAndScale_V2");

			assert(offsetAndScale == offsetAndScaleV2);

			std::vector<Common::UString> values;
			Common::UString::split(offsetAndScale, ' ', values);
			if (values.size() != 4)
				throw Common::Exception("Invalid OffsetAndScale attribute");

			atlasTexture.erase(atlasTexture.findFirst(".dds"), atlasTexture.end());

			AtlasTexture texture;
			texture.textureFile = atlasTexture;
			Common::parseString(values[0], texture.x);
			Common::parseString(values[1], texture.y);
			Common::parseString(values[2], texture.w);
			Common::parseString(values[3], texture.h);

			_atlasTextures[sourceTexture] = texture;
		}
	}
}

} // End of namespace Aurora
#endif
