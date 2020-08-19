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
 *  Treat Nintendo NSBTX files, which contain multiple textures as an
 *  archive of intermediate textures.
 */

#ifndef AURORA_NSBTXFILE_H
#define AURORA_NSBTXFILE_H

#include <vector>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/nitrofile.h"

namespace Common {
	class WriteStream;
}

namespace Aurora {

class NSBTXFile : public Archive, public NitroFile {
public:
	/** Take over this stream and read an NSBTX file out of it. */
	NSBTXFile(Common::SeekableReadStream *nsbtx);
	~NSBTXFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	enum Format {
		kFormatNoTexture     = 0, ///< Empty.
		kFormatA3I5          = 1, ///< 3bit alpha + 5bit color index.
		kFormat2bpp          = 2, ///< 2bit color index.
		kFormat4bpp          = 3, ///< 4bit color index.
		kFormat8bpp          = 4, ///< 8bit color index.
		kFormat4x4Compressed = 5, ///< 32bit per 4x4 texel block (Unsupported).
		kFormatA5I3          = 6, ///< 5bit alpha + 3bit color index.
		kFormat16bpp         = 7  ///< R5B5G5A1.
	};

	enum Transform {
		kTransformNone     = 0, ///< No extra texture coordinate transform.
		kTransformTexCoord = 1, ///< Transform texture coordinates using TexCoords.
		kTransformNormal   = 2, ///< Transform texture coordinates using Normals.
		kTransformVertex   = 3  ///< Transform texture coordinates using Vertices.
	};

	struct Texture {
		Common::UString name;
		uint32_t offset;

		Format format;

		uint16_t width;
		uint16_t height;

		bool wrapX; ///< true: wrap, false: clamp.
		bool wrapY; ///< true: wrap, false: clamp.
		bool flipX; ///< true: flip on every 2nd texture wrap.
		bool flipY; ///< true: flip on every 2nd texture wrap.
		bool alpha; ///< true: color index 0 is transparent.

		Transform coordTransform;
	};

	struct Palette {
		Common::UString name;
		uint32_t offset;
	};

	struct ReadContext {
		const Texture *texture;

		std::unique_ptr<const byte[]> palette;

		Common::SeekableSubReadStreamEndian *nsbtx;
		Common::WriteStream *stream;

		ReadContext(Common::SeekableSubReadStreamEndian &n, const Texture &t, Common::WriteStream &s);
		~ReadContext();
	};

	typedef std::vector<Texture> Textures;
	typedef std::vector<Palette> Palettes;


	/** The name of the NSBTX file. */
	std::unique_ptr<Common::SeekableSubReadStreamEndian> _nsbtx;

	/** External list of resource names and types. */
	ResourceList _resources;

	uint32_t _textureOffset;

	uint32_t _textureInfoOffset;
	uint32_t _paletteInfoOffset;

	uint32_t _textureDataOffset;
	uint32_t _paletteDataOffset;

	Textures _textures;
	Palettes _palettes;


	void load(Common::SeekableSubReadStreamEndian &nsbtx);

	void readHeader    (Common::SeekableSubReadStreamEndian &nsbtx);
	void readFileHeader(Common::SeekableSubReadStreamEndian &nsbtx);
	void readInfoHeader(Common::SeekableSubReadStreamEndian &nsbtx);
	void readTextures  (Common::SeekableSubReadStreamEndian &nsbtx);
	void readPalettes  (Common::SeekableSubReadStreamEndian &nsbtx);

	void createResourceList();

	const Palette *findPalette(const Texture &texture) const;
	void getPalette(ReadContext &ctx) const;

	static uint32_t getITEXSize(const Texture &texture);

	static void writeITEXHeader(const ReadContext &ctx);
	static void writePixel(const ReadContext &ctx, byte r, byte g, byte b, byte a);

	static void getTexture     (const ReadContext &ctx);
	static void getTexture2bpp (const ReadContext &ctx);
	static void getTexture4bpp (const ReadContext &ctx);
	static void getTexture8bpp (const ReadContext &ctx);
	static void getTexture16bpp(const ReadContext &ctx);
	static void getTextureA3I5 (const ReadContext &ctx);
	static void getTextureA5I3 (const ReadContext &ctx);
};

} // End of namespace Aurora

#endif // AURORA_NSBTXFILE_H
