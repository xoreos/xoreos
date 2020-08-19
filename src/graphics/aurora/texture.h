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
 *  A texture as used in the Aurora engines.
 */

#ifndef GRAPHICS_AURORA_TEXTURE_H
#define GRAPHICS_AURORA_TEXTURE_H

#include <memory>

#include "src/common/ustring.h"

#include "src/graphics/types.h"
#include "src/graphics/texture.h"

#include "src/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class ImageDecoder;
class TXI;

namespace Aurora {

/** A texture. */
class Texture : public Graphics::Texture {
public:
	virtual ~Texture();

	uint32_t getWidth()  const;
	uint32_t getHeight() const;

	bool hasAlpha() const;

	/** Is this a dynamic texture, or a shared static one? */
	virtual bool isDynamic() const;

	/** Return the TXI. */
	const TXI &getTXI() const;
	/** Return the image. */
	const ImageDecoder &getImage() const;

	/** Try to reload the texture. */
	virtual bool reload();

	/** Dump the texture into a TGA. */
	bool dumpTGA(const Common::UString &fileName) const;


	/** Load an image in any of the common texture formats. */
	static ImageDecoder *loadImage(const Common::UString &name, bool deswizzle = false);
	/** Load an image in any of the common texture formats. */
	static ImageDecoder *loadImage(const Common::UString &name, ::Aurora::FileType &type,
	                               bool deswizzle = false);

	/** Create a texture from this image resource. */
	static Texture *create(const Common::UString &name, bool deswizzle = false);
	/** Take over the image and create a texture from it. */
	static Texture *create(ImageDecoder *image, ::Aurora::FileType type = ::Aurora::kFileTypeNone,
	                       TXI *txi = 0, bool deswizzle = false);


protected:
	Common::UString    _name; ///< The name of the texture's image's file.
	::Aurora::FileType _type; ///< The type of the texture's image's file.

	std::unique_ptr<ImageDecoder> _image; ///< The actual image.
	std::unique_ptr<TXI> _txi;            ///< The TXI.

	uint32_t _width;
	uint32_t _height;

	bool _deswizzle;


	Texture();
	Texture(const Common::UString &name, ImageDecoder *image, ::Aurora::FileType type, TXI *txi = 0,
	        bool deswizzle = false);

	void set(const Common::UString &name, ImageDecoder *image, ::Aurora::FileType type, TXI *txi,
	         bool deswizzle = false);

	void addToQueues();
	void removeFromQueues();
	void refresh();


	// GLContainer
	void doRebuild();
	void doDestroy();


	void create2DTexture();
	void createCubeMapTexture();

	void setWrap(GLenum target, GLint wrapModeX, GLint wrapModeY);
	void setAlign();
	void setFilter(GLenum target);
	void setMipMaps(GLenum target);
	void setMipMapData(GLenum target, size_t layer, size_t mipMap);

	static TXI *loadTXI(const Common::UString &name);
	static ImageDecoder *loadImage(Common::SeekableReadStream *imageStream, ::Aurora::FileType type,
	                               TXI *txi = 0, bool deswizzle = false);

	static ImageDecoder *loadImage(const Common::UString &name, ::Aurora::FileType &type, TXI *txi,
	                               bool deswizzle = false);

	static Texture *createPLT(const Common::UString &name, Common::SeekableReadStream *imageStream);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXTURE_H
