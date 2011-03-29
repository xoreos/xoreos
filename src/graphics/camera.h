/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/camera.h
 *  Camera management.
 */

#ifndef GRAPHICS_CAMERA_H
#define GRAPHICS_CAMERA_H

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

namespace Graphics {

class CameraManager : public Common::Singleton<CameraManager> {
public:
	CameraManager();

	void lock();
	void unlock();

	const float *getPosition   () const; ///< Get the current camera position.
	const float *getOrientation() const; ///< Get the current camera orientation.

	void reset(); ///< Reset the current position and orientation.

	void setPosition   (float x, float y, float z); ///< Set the camera position.
	void setOrientation(float x, float y, float z); ///< Set the camera orientation.

	/** Set orientation from unit vector describing the bearing in the xy plane. */
	void setOrientation(float vX, float vY);

	void turn(float x, float y, float z); ///< Turn along axes.
	void move(float x, float y, float z); ///< Move along axes.

	void move  (float n); ///< Move along current view axis.
	void strafe(float n); ///< Move orthogonal (left/right) to current view axis.
	void fly   (float n); ///< Move orthogonal (up/down) to current view axis.

	uint32 lastChanged() const; ///< The timestamp the camera was changed last.

private:
	Common::Mutex _mutex;

	uint32 _lastChanged;

	float _position[3];    ///< Current position.
	float _orientation[3]; ///< Current orientation.
};

} // End of namespace Graphics

/** Shortcut for accessing the camera manager. */
#define CameraMan Graphics::CameraManager::instance()

#endif // GRAPHICS_CAMERA_H
