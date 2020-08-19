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
 *  BioWare's Packed Layered Texture.
 */

/* A PLT file consist of up to 10 layers that can be tinted independently,
 * that are all flattened into one texture for display. The tinting is
 * dynamic, though, meaning the colors can be changed on-the-fly from the
 * engine code, even for PLTs that are currently displayed.
 *
 * For each pixel, the PLT specifies two values: to which layer it belongs
 * to, and its coloring intensity. As such, each pixel only ever belongs
 * to a single layer.
 *
 * For the actual colors involved, each layer in turn requires a palette
 * image, usually a TGA in BGRA. The coloring value given by the engine
 * code specifies the Y coordinate from where to pick the color from
 * the palette image, and the PLT-supplied intensity specifies the X
 * coordinate.
 *
 * So, this is the algorithm to build the final texture:
 *
 * for each pixel p in image {
 *   int layerIndex = read layer index value from the PLT file
 *   int intensity  = read intensity value from the PLT file
 *   int colorIndex = get color index value from the engine code
 *
 *   p = layerImages[layerIndex].getPixel(intensity, colorIndex)
 * }
 */

#include <cassert>
#include <cstring>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/strutil.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/surface.h"

#include "src/graphics/aurora/pltfile.h"

static const uint32_t kPLTID     = MKTAG('P', 'L', 'T', ' ');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Graphics {

namespace Aurora {

PLTFile::PLTFile(const Common::UString &name, Common::SeekableReadStream &plt) :
	_name(name), _surface(0) {

	for (size_t i = 0; i < kLayerMAX; i++)
		_colors[i] = 0;

	load(plt);
}

PLTFile::~PLTFile() {
}

bool PLTFile::isDynamic() const {
	return true;
}

bool PLTFile::reload() {
	// We can't reload PLT files
	return false;
}

void PLTFile::setLayerColor(Layer layer, uint8_t color) {
	assert((layer >= 0) && (layer < kLayerMAX));

	_colors[layer] = color;
}

void PLTFile::rebuild() {
	build();
	refresh();
}

void PLTFile::load(Common::SeekableReadStream &plt) {
	// --- PLT header ---
	AuroraFile::readHeader(plt);

	if (_id != kPLTID)
		throw Common::Exception("Not a PLT file (%s)", Common::debugTag(_id).c_str());
	if (_version != kVersion1)
		throw Common::Exception("Unsupported PLT file version %s", Common::debugTag(_version).c_str());

	const uint32_t layers = plt.readUint32LE();
	if (layers > kLayerMAX)
		throw Common::Exception("Too many layers (%d)", layers);

	plt.skip(4); // Unknown

	const size_t width  = plt.readUint32LE();
	const size_t height = plt.readUint32LE();

	if ((plt.size() - plt.pos()) < (2 * width * height))
		throw Common::Exception("Not enough data");

	// --- PLT layer data ---

	size_t size = width * height;

	_dataImage  = std::make_unique<uint8_t[]>(size);
	_dataLayers = std::make_unique<uint8_t[]>(size);

	uint8_t *image = _dataImage.get();
	uint8_t *layer = _dataLayers.get();
	while (size-- > 0) {
		*image++ = plt.readByte();
		*layer++ = MIN<uint8_t>(plt.readByte(), kLayerMAX - 1);
	}

	// --- Create the actual texture surface ---

	// Initialize it to pink, for high debug visibility
	_surface = new Surface(width, height);
	_surface->fill(0xFF, 0x00, 0xFF, 0xFF);

	set(_name, _surface, ::Aurora::kFileTypePLT, 0);
	addToQueues();
}

void PLTFile::build() {
	/* For all layers, copy one whole row of pixels into the row buffer.
	 * The row picked for each layer corresponds to the color index we want.
	 * We don't care about the other rows, as they belong to other color indices. */
	byte rows[4 * 256 * kLayerMAX];
	getColorRows(rows, _colors);

	const size_t pixels = _width * _height;
	const uint8_t *image  = _dataImage.get();
	const uint8_t *layer  = _dataLayers.get();
	      byte  *dst    = _surface->getData();

	/* Now iterate over all pixels, each time copying the correct BGRA values
	 * for the pixel's intensity into the final image. */
	for (size_t i = 0; i < pixels; i++, image++, layer++, dst += 4)
		memcpy(dst, rows + (*layer * 4 * 256) + (*image * 4), 4);
}

/** The palette image resource names for all layers. */
static const char * const kPalettes[PLTFile::kLayerMAX] = {
	"pal_skin01",
	"pal_hair01",
	"pal_armor01",
	"pal_armor02",
	"pal_cloth01",
	"pal_cloth01",
	"pal_leath01",
	"pal_leath01",
	"pal_tattoo01",
	"pal_tattoo01"
};

/** Load a specific layer palette image and perform some sanity checks. */
ImageDecoder *PLTFile::getLayerPalette(uint32_t layer, uint8_t row) {
	assert(layer < kLayerMAX);

	// TODO: We may want to cache these somehow...
	std::unique_ptr<ImageDecoder> palette(loadImage(kPalettes[layer]));

	if (palette->getFormat() != kPixelFormatBGRA)
		throw Common::Exception("Invalid format (%d)", palette->getFormat());

	if (palette->getMipMapCount() < 1)
		throw Common::Exception("No mip maps");

	const ImageDecoder::MipMap &mipMap = palette->getMipMap(0);

	if (mipMap.width != 256)
		throw Common::Exception("Invalid width (%d)", mipMap.width);

	if (row >= mipMap.height)
		throw Common::Exception("Invalid height (%d >= %d)", row, mipMap.height);

	return palette.release();
}

void PLTFile::getColorRows(byte rows[4 * 256 * kLayerMAX], const uint8_t colors[kLayerMAX]) {
	for (size_t i = 0; i < kLayerMAX; i++, rows += 4 * 256) {
		try {
			std::unique_ptr<ImageDecoder> palette(getLayerPalette(i, colors[i]));

			// The images have their origin at the bottom left, so we flip the color row
			const uint8_t row = palette->getMipMap(0).height - 1 - colors[i];

			// Copy the whole row into the buffer
			memcpy(rows, palette->getMipMap(0).data.get() + (row * 4 * 256), 4 * 256);

		} catch (...) {
			// On error set to pink (while honoring intensity), for high debug visibility
			for (size_t p = 0; p < 256; p++) {
				rows[p * 4 + 0] = p;
				rows[p * 4 + 1] = 0x00;
				rows[p * 4 + 2] = p;
				rows[p * 4 + 3] = 0xFF;
			}

			Common::exceptionDispatcherWarning("Failed to load palette \"%s\"", kPalettes[i]);
		}
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
