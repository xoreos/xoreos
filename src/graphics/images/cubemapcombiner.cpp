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
 *  A class creating a cube map by combining six images.
 */

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/graphics/images/cubemapcombiner.h"

namespace Graphics {

CubeMapCombiner::CubeMapCombiner(ImageDecoder *(&sides)[6]) {
	BOOST_SCOPE_EXIT( (&sides) ) {
		for (size_t i = 0; i < ARRAYSIZE(sides); i++)
			delete sides[i];
	} BOOST_SCOPE_EXIT_END

	_layerCount = ARRAYSIZE(sides);
	_isCubeMap  = true;

	const size_t mipMapCount = sides[0] ? sides[0]->getMipMapCount() : 0;
	if (mipMapCount < 1)
		throw Common::Exception("CubeMapCombiner: No mip maps");

	_compressed = sides[0]->isCompressed();
	_hasAlpha   = sides[0]->hasAlpha();

	_format    = sides[0]->getFormat();
	_formatRaw = sides[0]->getFormatRaw();
	_dataType  = sides[0]->getDataType();

	const int32_t width  = sides[0]->getMipMap(0).width;
	const int32_t height = sides[0]->getMipMap(0).height;

	for (size_t i = 0; i < ARRAYSIZE(sides); i++) {
		if (!sides[i])
			throw Common::Exception("CubeMapCombiner: Side %u is empty", (uint) i);

		if (sides[i]->getLayerCount() != 1)
			throw Common::Exception("CubeMapCombiner: Side %u is has multiple layers", (uint) i);

		if (sides[i]->getMipMapCount() != mipMapCount)
			throw Common::Exception("CubeMapCombiner: Mip map count mismatch (%u != %u)",
			                        (uint) sides[i]->getMipMapCount(), (uint) mipMapCount);

		if ((width != sides[i]->getMipMap(0).width) || (height != sides[i]->getMipMap(0).height))
			throw Common::Exception("CubeMapCombiner: Dimensions mismatch (%ux%u != %ux%u)",
			                        width, height, sides[i]->getMipMap(0).width, sides[i]->getMipMap(0).height);

		if ((_compressed != sides[i]->isCompressed()) ||
		    (_hasAlpha   != sides[i]->hasAlpha    ()) ||
		    (_format     != sides[i]->getFormat   ()) ||
		    (_formatRaw  != sides[i]->getFormatRaw()) ||
		    (_dataType   != sides[i]->getDataType ()))
			throw Common::Exception("CubeMapCombiner: Format mismatch (%u, %u, %u, %u, %u != %u, %u, %u, %u, %u)",
			                        (uint) _compressed, (uint) _hasAlpha, (uint) _format,
			                        (uint) _formatRaw, (uint) _dataType,
			                        (uint) sides[i]->isCompressed(), (uint) sides[i]->hasAlpha(),
			                        (uint) sides[i]->getFormat(), (uint) sides[i]->getFormatRaw(),
			                        (uint) sides[i]->getDataType());
	}

	_txi = sides[0]->getTXI();

	_mipMaps.resize(_layerCount * mipMapCount);

	for (size_t layer = 0; layer < _layerCount; layer++) {
		for (size_t mipMap = 0; mipMap < mipMapCount; mipMap++) {
			const size_t index = layer * mipMapCount + mipMap;

			_mipMaps[index] = new MipMap(sides[layer]->getMipMap(mipMap), this);
		}
	}
}

CubeMapCombiner::~CubeMapCombiner() {
}

} // End of namespace Graphics
