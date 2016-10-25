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
 *  The area mini map in Sonic Chronicles: The Dark Brotherhood.
 */

#ifndef ENGINES_SONIC_AREAMINIMAP_H
#define ENGINES_SONIC_AREAMINIMAP_H

#include "src/common/scopedptr.h"

namespace Common {
	class UString;
}

namespace Graphics {
	namespace Aurora {
		class GUIQuad;
	}
}

namespace Engines {

namespace Sonic {

class AreaMiniMap {
public:
	AreaMiniMap(const Common::UString &name);
	~AreaMiniMap();

	void show();
	void hide();

private:
	Common::ScopedPtr<Graphics::Aurora::GUIQuad> _miniMap;

	void loadMiniMap(const Common::UString &name);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_AREAMINIMAP_H
