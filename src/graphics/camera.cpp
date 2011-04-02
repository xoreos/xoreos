/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/camera.cpp
 *  Camera management.
 */

#include "common/util.h"
#include "common/maths.h"

#include "graphics/camera.h"
#include "graphics/graphics.h"

#include "events/events.h"
#include "events/notifications.h"

DECLARE_SINGLETON(Graphics::CameraManager)

namespace Graphics {

CameraManager::CameraManager() : _lastChanged(0) {
	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

void CameraManager::lock() {
	_mutex.lock();
}

void CameraManager::unlock() {
	_mutex.unlock();
}

const float *CameraManager::getPosition() const {
	return _position;
}

const float *CameraManager::getOrientation() const {
	return _orientation;
}

void CameraManager::reset() {
	Common::StackLock cameraLock(_mutex);

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;

	_lastChanged = EventMan.getTimestamp();

	GfxMan.recalculateObjectDistances();

	NotificationMan.cameraMoved();
}

void CameraManager::setPosition(float x, float y, float z) {
	Common::StackLock cameraLock(_mutex);

	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	_lastChanged = EventMan.getTimestamp();

	GfxMan.recalculateObjectDistances();

	NotificationMan.cameraMoved();
}

void CameraManager::setOrientation(float x, float y, float z) {
	Common::StackLock cameraLock(_mutex);

	_orientation[0] = fmodf(x, 360.0);
	_orientation[1] = fmodf(y, 360.0);
	_orientation[2] = fmodf(z, 360.0);

	_lastChanged = EventMan.getTimestamp();

	GfxMan.recalculateObjectDistances();

	NotificationMan.cameraMoved();
}

void CameraManager::setOrientation(float vX, float vY) {
	float x, y, z;

	Common::vector2orientation(vX, vY, x, y, z);

	setOrientation(x, y, z);
}

void CameraManager::turn(float x, float y, float z) {
	setOrientation(_orientation[0] + x, _orientation[1] + y, _orientation[2] + z);
}

void CameraManager::move(float x, float y, float z) {
	setPosition(_position[0] + x, _position[1] + y, _position[2] + z);
}

void CameraManager::move(float n) {
	float x = n * sin(Common::deg2rad(_orientation[1]));
	float y = n * sin(Common::deg2rad(_orientation[0]));
	float z = n * cos(Common::deg2rad(_orientation[1])) *
	              cos(Common::deg2rad(_orientation[0]));

	move(x, y, z);
}

void CameraManager::strafe(float n) {
	float x = n * sin(Common::deg2rad(_orientation[1] + 90.0)) *
	              cos(Common::deg2rad(_orientation[2]));
	float y = n * sin(Common::deg2rad(_orientation[2]));
	float z = n * cos(Common::deg2rad(_orientation[1] + 90.0));

	move(x, y, z);
}

void CameraManager::fly(float n) {
	float x = n * cos(Common::deg2rad(_orientation[2] + 90.0));
	float y = n * sin(Common::deg2rad(_orientation[0] + 90.0)) *
	              sin(Common::deg2rad(_orientation[2] + 90.0));
	float z = n * cos(Common::deg2rad(_orientation[0] + 90.0));

	move(x, y, z);
}

uint32 CameraManager::lastChanged() const {
	return _lastChanged;
}

} // End of namespace Graphics
