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
 *  A light descriptor in a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_LIGHT_H
#define ENGINES_NWN2_LIGHT_H

#include <memory>

#include "src/aurora/types.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"

#include "src/graphics/lightman.h"

namespace Engines {

namespace NWN2 {

/** A light within a NWN2 area. */
class Light : public Object {
public:
	Light(const Aurora::GFF3Struct &light);

	~Light();

protected:

	void load(const Aurora::GFF3Struct &light);

	Graphics::LightManager::LightNode _lightDiffuse;
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_LIGHT_H
