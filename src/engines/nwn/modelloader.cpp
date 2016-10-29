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
 *  Neverwinter Nights model loader.
 */

#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/graphics/aurora/model_nwn.h"

#include "src/engines/nwn/modelloader.h"

namespace Engines {

namespace NWN {

Graphics::Aurora::Model *NWNModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	/* TODO: Modules and HAKs can overwrite model files, so we actually need
	 *       to clean the cache after every module unload. */

	return new Graphics::Aurora::Model_NWN(resref, type, texture, &_modelCache);
}

} // End of namespace NWN

} // End of namespace Engines
