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
 *  Camera management.
 */

#ifndef GRAPHICS_CAMERA_H
#define GRAPHICS_CAMERA_H

#include "src/common/types.h"
#include "src/common/maths.h"
#include "src/common/singleton.h"

namespace Graphics {

class CameraManager : public Common::Singleton<CameraManager> {
public:
	CameraManager();

	const float *getPosition   () const; ///< Get the current camera position cache.
	const float *getOrientation() const; ///< Get the current camera orientation cache.

	void reset(); ///< Reset the current position and orientation.

	/** Set limits on the camera position. */
	void limit(float minX = -FLT_MAX, float minY = -FLT_MAX, float minZ = -FLT_MAX,
	           float maxX =  FLT_MAX, float maxY =  FLT_MAX, float maxZ =  FLT_MAX);

	void setPosition   (float x, float y, float z); ///< Set the camera position.
	void setOrientation(float x, float y, float z); ///< Set the camera orientation.

	void turn(float x, float y, float z); ///< Turn along axes.
	void move(float x, float y, float z); ///< Move along axes.

	/** Move relative to the current view axis. */
	void moveRelative(float x, float y, float z);

	uint32_t lastChanged() const; ///< The timestamp the camera was changed last.

	/** Update the caches with the current position and orientation.
	 *
	 *  All changes to the camera are delayed until this method is called.
	 *  This stops camera lagging due to too frequent changes.
	 */
	void update();

private:
	uint32_t _lastChanged;

	float _minPosition[3];
	float _maxPosition[3];

	float _position[3];    ///< Current position.
	float _orientation[3]; ///< Current orientation.

	float _positionCache[3];    ///< Current position, cached.
	float _orientationCache[3]; ///< Current orientation, cached.

	bool _needUpdate;
};

} // End of namespace Graphics

/** Shortcut for accessing the camera manager. */
#define CameraMan Graphics::CameraManager::instance()

#endif // GRAPHICS_CAMERA_H
