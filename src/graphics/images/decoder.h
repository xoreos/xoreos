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
 *  Generic image decoder interface.
 */

#ifndef GRAPHICS_IMAGES_DECODER_H
#define GRAPHICS_IMAGES_DECODER_H

#include <vector>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ptrvector.h"

#include "src/graphics/types.h"

#include "src/graphics/images/txi.h"

namespace Common {
	class SeekableReadStream;
	class UString;
}

namespace Graphics {

/** A generic interface for image decoders. */
class ImageDecoder : boost::noncopyable {
public:
	/** A mip map. */
	struct MipMap {
		int    width;  ///< The mip map's width.
		int    height; ///< The mip map's height.
		uint32 size;   ///< The mip map's size in bytes.

		Common::ScopedArray<byte> data; ///< The mip map's data.

		const ImageDecoder *image; ///< The image the mip map belongs to.

		MipMap(const ImageDecoder *i = 0);
		MipMap(const MipMap &mipMap, const ImageDecoder *i = 0);
		~MipMap();

		void swap(MipMap &right);

		/** Get the color values of the pixel at this position. */
		void getPixel(int x, int y, float &r, float &g, float &b, float &a) const;
		/** Get the color values of the pixel at this index. */
		void getPixel(int n, float &r, float &g, float &b, float &a) const;

		/** Set the color values of the pixel at this position. */
		void setPixel(int x, int y, float r, float g, float b, float a);
		/** Set the color values of the pixel at this index. */
		void setPixel(int n, float r, float g, float b, float a);
	};

	ImageDecoder();
	ImageDecoder(const ImageDecoder &image);
	virtual ~ImageDecoder();

	ImageDecoder &operator=(const ImageDecoder &image);

	/** Is the image data compressed? */
	bool isCompressed() const;

	/** Does the image data have alpha? .*/
	bool hasAlpha() const;

	/** Return the image data's general format. */
	PixelFormat    getFormat() const;
	/** Return the image data's raw format. */
	PixelFormatRaw getFormatRaw() const;
	/** Return the image data pixel's type. */
	PixelDataType  getDataType() const;

	/** Return the number of mip maps contained in the image. */
	size_t getMipMapCount() const;
	/** Return the number of layers contained in the image. */
	size_t getLayerCount() const;

	/** Is this image a cube map? */
	bool isCubeMap() const;

	/** Return a mip map. */
	const MipMap &getMipMap(size_t mipMap, size_t layer = 0) const;

	/** Manually decompress the texture image data. */
	void decompress();

	/** Return the texture information TXI, which may be embedded in the image. */
	const TXI &getTXI() const;

	/** Dump the image into a TGA. */
	bool dumpTGA(const Common::UString &fileName) const;

protected:
	typedef Common::PtrVector<MipMap> MipMaps;

	bool _compressed;
	bool _hasAlpha;

	PixelFormat    _format;
	PixelFormatRaw _formatRaw;
	PixelDataType  _dataType;

	/** Number of layers in this image. For layered 3D images and cubemaps. */
	size_t _layerCount;
	/** Is this image a cube map? A cube map always needs to have 6 layers! */
	bool _isCubeMap;

	MipMaps _mipMaps;

	TXI _txi;

	static void decompress(MipMap &out, const MipMap &in, PixelFormatRaw format);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_DECODER_H
