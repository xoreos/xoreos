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

DECLARE_SINGLETON(Graphics::CameraManager)

namespace Graphics {

CameraManager::CameraManager() {
	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

const float *CameraManager::getPosition() const {
	return _position;
}

const float *CameraManager::getOrientation() const {
	return _orientation;
}

void CameraManager::reset() {
	GfxMan.lockFrame();

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;

	GfxMan.unlockFrame();
}

void CameraManager::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	GfxMan.unlockFrame();
}

void CameraManager::setOrientation(float x, float y, float z) {
	GfxMan.lockFrame();

	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;

	// Clamp
	for (int i = 0; i < 3; i++) {
		while (_orientation[i] >  360)
			_orientation[i] -= 360;
		while (_orientation[i] < -360)
			_orientation[i] += 360;
	}

	GfxMan.unlockFrame();
}

void CameraManager::setOrientationVector(float x, float y) {
	float o[3];

	Common::vector2orientation(x, y, o[0], o[1], o[2]);
	setOrientation(o[0], o[1], o[2]);
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

} // End of namespace Graphics
