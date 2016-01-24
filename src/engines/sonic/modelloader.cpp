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
 *  Sonic Chronicles: The Dark Brotherhood model loader.
 */

#include "src/common/system.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/graphics/aurora/model_sonic.h"

#include "src/engines/sonic/modelloader.h"

namespace Engines {

namespace Sonic {

Graphics::Aurora::Model *SonicModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType UNUSED(type), const Common::UString &UNUSED(texture)) {

	return new Graphics::Aurora::Model_Sonic(resref);
}

} // End of namespace Sonic

} // End of namespace Engines
