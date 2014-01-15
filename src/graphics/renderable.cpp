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

/** @file graphics/renderable.cpp
 *  Base class for renderable objects.
 */

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreSceneManager.h>

#include "graphics/util.h"
#include "graphics/renderable.h"

namespace Graphics {

Renderable::Renderable(const Common::UString &scene) : _scene(scene), _rootNode(0), _visible(false), _selectable(false) {
	_basePosition[0] = 0.0;
	_basePosition[1] = 0.0;
	_basePosition[2] = 0.0;

	_baseOrientation[0] = 0.0;
	_baseOrientation[1] = 1.0;
	_baseOrientation[2] = 0.0;
	_baseOrientation[3] = 0.0;

	_baseScale[0] = 1.0;
	_baseScale[1] = 1.0;
	_baseScale[2] = 1.0;
}

Renderable::~Renderable() {
}

Ogre::SceneNode *Renderable::getRootNode() {
	return _rootNode;
}

void Renderable::setVisible(bool visible) {
	LOCK_FRAME();

	_visible = visible;

	if (_rootNode)
		_rootNode->setVisible(visible);
}

bool Renderable::isVisible() const {
	return _visible;
}

Common::UString Renderable::getID() const {
	if (!_rootNode)
		return "";

	return _rootNode->getName().c_str();
}

void Renderable::setBasePosition(float x, float y, float z) {
	_basePosition[0] = x;
	_basePosition[1] = y;
	_basePosition[2] = z;

	setPosition(0.0, 0.0, 0.0);
}

void Renderable::setBaseOrientation(float radian, float x, float y, float z) {
	_baseOrientation[0] = radian;
	_baseOrientation[1] = x;
	_baseOrientation[2] = y;
	_baseOrientation[3] = z;

	setOrientation(0.0, 1.0, 0.0, 0.0);
}

void Renderable::setBaseScale(float x, float y, float z) {
	_baseScale[0] = x;
	_baseScale[1] = y;
	_baseScale[2] = z;

	setScale(1.0, 1.0, 1.0);
}

void Renderable::getPosition(float &x, float &y, float &z) const {
	const Ogre::Vector3 &p = _rootNode->getPosition();

	x = p.x - _basePosition[0];
	y = p.y - _basePosition[1];
	z = p.z - _basePosition[2];
}

void Renderable::getOrientation(float &radian, float &x, float &y, float &z) const {
	Ogre::Quaternion base(Ogre::Radian(_baseOrientation[0]),
	                      Ogre::Vector3(_baseOrientation[1], _baseOrientation[2], _baseOrientation[3]));

	const Ogre::Quaternion &q = base.Inverse() * _rootNode->getOrientation();


	Ogre::Radian  angle;
	Ogre::Vector3 axis;

	q.ToAngleAxis(angle, axis);

	radian = angle.valueRadians();
	x      = axis.x;
	y      = axis.y;
	z      = axis.z;
}

void Renderable::getScale(float &x, float &y, float &z) const {
	LOCK_FRAME();

	const Ogre::Vector3 &s = _rootNode->getScale();

	x = s.x / _baseScale[0];
	y = s.y / _baseScale[1];
	z = s.z / _baseScale[2];
}

void Renderable::setPosition(float x, float y, float z) {
	LOCK_FRAME();

	_rootNode->setPosition(x + _basePosition[0], y + _basePosition[1], z + _basePosition[2]);
}

void Renderable::setOrientation(float radian, float x, float y, float z) {
	LOCK_FRAME();

	Ogre::Quaternion base(Ogre::Radian(_baseOrientation[0]),
	                      Ogre::Vector3(_baseOrientation[1], _baseOrientation[2], _baseOrientation[3]));
	Ogre::Quaternion modi(Ogre::Quaternion(Ogre::Radian(radian), Ogre::Vector3(x, y, z)));

	_rootNode->setOrientation(base);
	_rootNode->rotate(modi);
}

void Renderable::setScale(float x, float y, float z) {
	LOCK_FRAME();

	_rootNode->setScale(x * _baseScale[0], y * _baseScale[1], z * _baseScale[2]);
}

void Renderable::move(float x, float y, float z) {
	LOCK_FRAME();

	_rootNode->translate(x, y, z);
}

void Renderable::rotate(float radian, float x, float y, float z) {
	LOCK_FRAME();

	_rootNode->rotate(Ogre::Quaternion(Ogre::Radian(radian), Ogre::Vector3(x, y, z)));
}

void Renderable::scale(float x, float y, float z) {
	LOCK_FRAME();

	_rootNode->scale(x, y, z);
}

void Renderable::showBoundingBox(bool show) {
	if (!_rootNode)
		return;

	_rootNode->showBoundingBox(show);
}

void Renderable::setSelectable(bool selectable) {
	_selectable = selectable;
}

void Renderable::destroyAnimation(const Common::UString &name) {
	if (getOgreSceneManager(_scene).hasAnimationState(name.c_str()))
		getOgreSceneManager(_scene).destroyAnimationState(name.c_str());
	if (getOgreSceneManager(_scene).hasAnimation(name.c_str()))
		getOgreSceneManager(_scene).destroyAnimation(name.c_str());
}

void Renderable::destroyAnimation(Ogre::Animation *anim) {
	if (!anim)
		return;

	destroyAnimation(anim->getName().c_str());
}

} // End of namespace Graphics
