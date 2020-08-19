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
 *  Sonic Chronicles: The Dark Brotherhood utility functions.
 */

#ifndef ENGINES_SONIC_UTIL_H
#define ENGINES_SONIC_UTIL_H

#include "src/common/types.h"

#include "src/graphics/aurora/texturehandle.h"
#include "src/graphics/aurora/fonthandle.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace Sonic {

Graphics::Aurora::TextureHandle loadNCGR(const Common::UString &name, const Common::UString &nclr,
                                         uint32_t width, uint32_t height, ...);

Graphics::Aurora::FontHandle loadFont(const Common::UString &name, const Common::UString &nftr, bool invert);

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_UTIL_H
