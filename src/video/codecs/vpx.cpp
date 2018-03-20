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
 *  VP8/VP9 codec class.
 */

#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

// Undefine UNUSED, which is defined by libvpx
#undef UNUSED

#include "src/common/readstream.h"
#include "src/common/scopedptr.h"
#include "src/graphics/yuv_to_rgb.h"
#include "src/graphics/images/surface.h"
#include "src/video/codecs/codec.h"
#include "src/video/codecs/vpx.h"

namespace Video {

class VPXDecoder : public Codec {
public:
	VPXDecoder();
	~VPXDecoder();

	bool init(vpx_codec_iface_t *iface);

	void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &dataStream);

private:
	bool _initialized;
	vpx_codec_ctx _context;

	Common::ScopedPtr<Graphics::Surface> _surface;
};

VPXDecoder::VPXDecoder() : _initialized(false) {
}

VPXDecoder::~VPXDecoder() {
	if (_initialized)
		vpx_codec_destroy(&_context);
}

bool VPXDecoder::init(vpx_codec_iface_t *iface) {
	// Destroy any old context
	if (_initialized) {
		vpx_codec_destroy(&_context);
		_initialized = false;
	}

	// Set up the config params
	// TODO: More threads?
	vpx_codec_dec_cfg config;
	memset(&config, 0, sizeof(config));
	config.threads = 1;

	// Try to perform the initialization
	vpx_codec_err_t result = vpx_codec_dec_init(&_context, iface, &config, 0);
	if (result != VPX_CODEC_OK)
		return false;

	// All good
	_initialized = true;
	return true;
}

void VPXDecoder::decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &dataStream) {
	if (!_initialized)
		return;

	// Read all the data from the stream
	Common::ScopedArray<byte> data(new byte[dataStream.size()]);
	dataStream.read(data.get(), dataStream.size());

	// Perform the actual decode
	vpx_codec_err_t result = vpx_codec_decode(&_context, data.get(), dataStream.size(), 0, 0);
	if (result != VPX_CODEC_OK)
		return;

	// Try to get the image
	vpx_codec_iter_t iter = 0;
	vpx_image_t *image = vpx_codec_get_frame(&_context, &iter);
	if (!image)
		return;

	// Figure out the color range
	Graphics::YUVToRGBManager::LuminanceScale scale;
	switch (image->range) {
	case VPX_CR_STUDIO_RANGE:
		scale = Graphics::YUVToRGBManager::kScaleITU;
		break;
	case VPX_CR_FULL_RANGE:
		scale = Graphics::YUVToRGBManager::kScaleFull;
		break;
	default:
		return;
	}

	// If we don't have it already, create our local surface
	if (!_surface)
		_surface.reset(new Graphics::Surface(image->w, image->h));

	// Do the conversion based on the color space
	switch (image->fmt) {
	case VPX_IMG_FMT_I420:
		YUVToRGBMan.convert420(scale, _surface->getData(), _surface->getPitch(), image->planes[0], image->planes[1], image->planes[2], image->w, image->h, image->stride[0], image->stride[1]);
		break;
	default:
		return;
	}

	// Copy the subarea into the surface
	for (int y = 0; y < surface.getHeight(); y++)
		memcpy(surface.getData() + y * surface.getPitch(), _surface->getData() + (y * image->d_h) * image->d_w * 4, image->d_w * 4);
}

Codec *makeVP8Decoder() {
	Common::ScopedPtr<VPXDecoder> decoder(new VPXDecoder());
	if (!decoder->init(&vpx_codec_vp8_dx_algo))
		return 0;

	return decoder.release();
}

Codec *makeVP9Decoder() {
	Common::ScopedPtr<VPXDecoder> decoder(new VPXDecoder());
	if (!decoder->init(&vpx_codec_vp9_dx_algo))
		return 0;

	return decoder.release();
}

} // End of namespace Video

