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
 *  Engine utility class for free-roam camera handling.
 */

#ifndef ENGINES_AURORA_FREEROAMCAMERA_H
#define ENGINES_AURORA_FREEROAMCAMERA_H

#include "src/common/singleton.h"

#include "src/events/types.h"

namespace Engines {

class FreeRoamCamera : public Common::Singleton<FreeRoamCamera> {
public:
	bool handleCameraInput(const Events::Event &e);
private:
	bool handleCameraKeyboardInput(const Events::Event &e);
	bool handleCameraMouseInput(const Events::Event &e);
};

} // End of namespace Engines

#define FreeRoamCam ::Engines::FreeRoamCamera::instance()

#endif // ENGINES_AURORA_FREEROAMCAMERA_H
