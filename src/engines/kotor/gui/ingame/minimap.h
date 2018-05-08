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
 *  The ingame minimap.
 */

#ifndef ENGINES_KOTOR_GUI_INGAME_MINIMAP_H
#define ENGINES_KOTOR_GUI_INGAME_MINIMAP_H

#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/subscenequad.h"

namespace Engines {

namespace KotOR {

class Minimap : public Graphics::Aurora::SubSceneQuad {
public:
	Minimap(const Common::UString &map, int northAxis,
			float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y,
			float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y);

	void setPosition(float x, float y);

private:
	Graphics::Aurora::GUIQuad _mapQuad;

	int _northAxis;
	float _mapPt1X, _mapPt1Y, _mapPt2X, _mapPt2Y;
	float _worldPt1X, _worldPt1Y, _worldPt2X, _worldPt2Y;
};

} // End of namespace KotOR

} // End of namespace Engines


#endif // ENGINES_KOTOR_INGAME_GUI_MINIMAP_H
