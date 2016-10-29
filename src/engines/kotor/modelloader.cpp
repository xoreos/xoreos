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
 *  Star Wars: Knights of the Old Republic model loader.
 */

#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/graphics/aurora/model_kotor.h"

#include "src/engines/kotor/modelloader.h"

namespace Engines {

namespace KotOR {

Graphics::Aurora::Model *KotORModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	return new Graphics::Aurora::Model_KotOR(resref, false, type, texture, &_modelCache);
}

} // End of namespace KotOR

} // End of namespace Engines
