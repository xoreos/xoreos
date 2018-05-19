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
 *  Engine utility class for camera handling where camera rotates around PC.
 */

#ifndef ENGINES_AURORA_SATELLITECAMERA_H
#define ENGINES_AURORA_SATELLITECAMERA_H

#include "glm/vec3.hpp"

#include "src/common/singleton.h"

#include "src/events/types.h"

namespace Engines {

class SatelliteCamera : public Common::Singleton<SatelliteCamera> {
public:
	SatelliteCamera();
	void setTarget(float x, float y, float z);
	void setDistance(float value);
	void setPitch(float value);
	float getYaw() const;
	bool handleCameraInput(const Events::Event &e);
	void update(float dt);
private:
	glm::vec3 _target;
	float _distance;
	float _yaw;
	float _pitch;
	float _pitchSin;
	float _pitchCos;
	bool _leftBtnPressed;
	bool _rightBtnPressed;
	bool _dirty;
};

} // End of namespace Engines

#define SatelliteCam ::Engines::SatelliteCamera::instance()

#endif // ENGINES_AURORA_SATELLITECAMERA_H
