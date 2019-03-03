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
 *  Handles camera movement in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_CAMERACONTROLLER_H
#define ENGINES_KOTORBASE_CAMERACONTROLLER_H

#include "external/glm/vec3.hpp"

#include "src/events/types.h"

namespace Engines {

namespace KotORBase {

class Module;

class CameraController {
public:
	CameraController(Module *module);

	// Flying camera

	bool isFlyCamera() const;

	void toggleFlyCamera();


	float getYaw() const;

	void setYaw(float value);

	void updateTarget();
	void updateCameraStyle();
	bool handleEvent(const Events::Event &e);
	void processRotation(float frameTime);
	void processMovement(float frameTime);
	void stopMovement();

private:
	Module *_module;
	bool _flycam { false };
	glm::vec3 _target;
	float _distance { 0.0f };
	float _yaw { 0.0f };
	float _pitch { 0.0f };
	float _height { 0.0f };
	bool _clockwiseMovementWanted { false };
	bool _counterClockwiseMovementWanted { false };
	bool _dirty { false };

	// Obstacles

	bool _obstacleExists { false };
	float _obstacleDistance { 0.0f };
	float _actualDistance { 0.0f };


	inline glm::vec3 getCameraPosition(float distance) const;

	inline bool shouldMoveClockwise() const;
	inline bool shouldMoveCounterClockwise() const;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_CAMERACONTROLLER_H
