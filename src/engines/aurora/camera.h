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
 *  Engine utility functions for camera handling.
 */

#ifndef ENGINES_AURORA_CAMERA_H
#define ENGINES_AURORA_CAMERA_H

#include "src/events/types.h"

namespace Engines {

/** Evaluate all input events that modify the camera position / orientation. */
bool handleCameraInput(const Events::Event &e);
/** Evaluate keyboard input events that modify the camera position / orientation. */
bool handleCameraKeyboardInput(const Events::Event &e);
/** Evaluate mouse input events that modify the camera position / orientation. */
bool handleCameraMouseInput(const Events::Event &e);

} // End of namespace Engines

#endif // ENGINES_AURORA_CAMERA_H
