/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/cameraman.cpp
 *  A camera manager.
 */

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreRenderWindow.h>

#include "graphics/cameraman.h"
#include "graphics/util.h"

DECLARE_SINGLETON(Graphics::CameraManager)

namespace Graphics {

CameraManager::CameraManager() : _camera(0), _screenWidth(0), _screenHeight(0) {
}

CameraManager::~CameraManager() {
	deinit();
}

void CameraManager::init() {
	_camera = getOgreSceneManager().createCamera("camera");

	_camera->setNearClipDistance(1.0);
	_camera->setFarClipDistance(1000.0);
	_camera->setFOVy(Ogre::Degree(60.0));
}

void CameraManager::deinit() {
	if (_camera)
		getOgreSceneManager().destroyCamera(_camera);

	_camera = 0;
}

Ogre::Viewport *CameraManager::createViewport(Ogre::RenderWindow *window) {
	return window->addViewport(_camera);
}

void CameraManager::setScreenSize(int width, int height) {
	_screenWidth  = width;
	_screenHeight = height;

	_camera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height));
}

void CameraManager::reset() {
	_camera->setPosition(0.0, 0.0, 0.0);
	_camera->setOrientation(Ogre::Quaternion());
	_camera->lookAt(0.0, 0.0, 0.0);
}

void CameraManager::getPosition(float &x, float &y, float &z) const {
	const Ogre::Vector3 &pos = _camera->getPosition();

	x = pos.x;
	y = pos.y;
	z = pos.z;
}

void CameraManager::getDirection(float &x, float &y, float &z) const {
	const Ogre::Vector3 dir = _camera->getDirection();

	x = dir.x;
	y = dir.y;
	z = dir.z;
}

void CameraManager::getOrientation(float &radian, float &x, float &y, float &z) const {
	Ogre::Radian  angle;
	Ogre::Vector3 axis;

	_camera->getOrientation().ToAngleAxis(angle, axis);

	radian = angle.valueRadians();
	x      = axis.x;
	y      = axis.y;
	z      = axis.z;
}

void CameraManager::setPosition(float x, float y, float z) {
	_camera->setPosition(x, y, z);
}

void CameraManager::setDirection(float x, float y, float z) {
	_camera->setDirection(x, y, z);
}

void CameraManager::lookAt(float x, float y, float z) {
	_camera->lookAt(x, y, z);
}

void CameraManager::setOrientation(float radian, float x, float y, float z) {
	_camera->setOrientation(Ogre::Quaternion(Ogre::Radian(radian), Ogre::Vector3(x, y, z)));
}

void CameraManager::move(float x, float y, float z) {
	_camera->move(Ogre::Vector3(x, y, z));
}

void CameraManager::moveRelative(float x, float y, float z) {
	_camera->moveRelative(Ogre::Vector3(x, y, z));
}

void CameraManager::rotate(float radian, float x, float y, float z) {
	_camera->rotate(Ogre::Quaternion(Ogre::Radian(radian), Ogre::Vector3(x, y, z)));
}

void CameraManager::roll(float radian) {
	_camera->roll(Ogre::Radian(radian));
}

void CameraManager::yaw(float radian) {
	_camera->yaw(Ogre::Radian(radian));
}

void CameraManager::pitch(float radian) {
	_camera->pitch(Ogre::Radian(radian));
}

Ogre::Ray CameraManager::castRay(int x, int y) const {
	return _camera->getCameraToViewportRay(x / ((float) _screenWidth), y / ((float) _screenHeight));
}

} // End of namespace Graphics
