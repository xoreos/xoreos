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
 *  h.263 video codec.
 */

#include <cstring>

#include <memory>

#include <xvid.h>

#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/graphics/yuv_to_rgb.h"

#include "src/graphics/images/surface.h"

#include "src/video/codecs/codec.h"
#include "src/video/codecs/h263.h"

namespace Video {

namespace {

class H263Codec : public Codec {
public:
	H263Codec(uint32_t width, uint32_t height, Common::SeekableReadStream &extraData);
	~H263Codec();

	void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &dataStream);

private:
	uint32_t _width;
	uint32_t _height;

	void *_decHandle;

	/**
	 * Internal decode function
	 */
	void decodeInternal(Common::SeekableReadStream &dataStream, Graphics::Surface *surface = 0);
};

H263Codec::H263Codec(uint32_t width, uint32_t height, Common::SeekableReadStream &extraData) : _width(width), _height(height) {
	xvid_gbl_init_t xvid_gbl_init;
	std::memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init_t));
	xvid_gbl_init.version = XVID_VERSION;
	xvid_gbl_init.debug = 0;//XVID_DEBUG_ERROR | XVID_DEBUG_HEADER | XVID_DEBUG_STARTCODE;
	xvid_global(0, XVID_GBL_INIT, &xvid_gbl_init, 0);

	xvid_dec_create_t xvid_dec_create;
	std::memset(&xvid_dec_create, 0, sizeof(xvid_dec_create_t));
	xvid_dec_create.version = XVID_VERSION;
	xvid_dec_create.width = width;
	xvid_dec_create.height = height;

	int result = xvid_decore(0, XVID_DEC_CREATE, &xvid_dec_create, 0);
	if (result != 0)
		throw Common::Exception("H263Codec::H263Codec(): Failed to create the decore context: %d", result);

	_decHandle = xvid_dec_create.handle;

	// Run the first extra data through the frame decoder, but don't bother
	// decoding to a surface.
	extraData.seek(0);
	decodeInternal(extraData);
}

H263Codec::~H263Codec() {
	xvid_decore(_decHandle, XVID_DEC_DESTROY, 0, 0);
}

void H263Codec::decodeInternal(Common::SeekableReadStream &dataStream, Graphics::Surface *surface) {
	// NOTE: When asking libxvidcore to decode the video into BGRA, it fills the alpha
	//       values with 0x00, rendering the output invisible (!).
	//       Since we, surprise, actually want to see the video, we would have to pass
	//       over the whole video data and fix-up the alpha values ourselves. Or
	//       alternatively do the YUV->BGRA conversion ourselves. We chose the latter.

	size_t dataSize = dataStream.size();
	std::unique_ptr<byte[]> data = std::make_unique<byte[]>(dataSize);
	dataStream.read(data.get(), dataSize);

	xvid_dec_frame_t xvid_dec_frame;
	std::memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));
	xvid_dec_frame.version    = XVID_VERSION;
	xvid_dec_frame.general    = XVID_DEBLOCKY | XVID_DEBLOCKUV | XVID_DERINGY | XVID_DERINGUV;
	xvid_dec_frame.bitstream  = data.get();
	xvid_dec_frame.length     = dataSize;
	xvid_dec_frame.output.csp = XVID_CSP_INTERNAL;

	xvid_dec_stats_t xvid_dec_stats;
	std::memset(&xvid_dec_stats, 0, sizeof(xvid_dec_stats_t));
	xvid_dec_stats.version = XVID_VERSION;

	int result = xvid_decore(_decHandle, XVID_DEC_DECODE, &xvid_dec_frame, &xvid_dec_stats);
	if (result < 0)
		throw Common::Exception("H263Codec::decodeFrame(): Failed to decode frame: %d", result);

	if (surface &&
	    xvid_dec_frame.output.plane[0] &&
	    xvid_dec_frame.output.plane[1] &&
	    xvid_dec_frame.output.plane[2])
		YUVToRGBMan.convert420(Graphics::YUVToRGBManager::kScaleFull,
				surface->getData(), surface->getWidth() * 4,
				static_cast<const byte *>(xvid_dec_frame.output.plane[0]),
				static_cast<const byte *>(xvid_dec_frame.output.plane[1]),
				static_cast<const byte *>(xvid_dec_frame.output.plane[2]), _width, _height,
				xvid_dec_frame.output.stride[0], xvid_dec_frame.output.stride[1]);
}

void H263Codec::decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &dataStream) {
	decodeInternal(dataStream, &surface);
}

} // End of anonymous namespace

Codec *makeH263Codec(int width, int height, Common::SeekableReadStream &extraData) {
	return new H263Codec(width, height, extraData);
}

} // End of namespace Video
