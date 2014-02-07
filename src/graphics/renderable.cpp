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
#include <OgreControllerManager.h>

#include "common/threads.h"

#include "graphics/graphics.h"
#include "graphics/util.h"
#include "graphics/renderable.h"
#include "graphics/materialman.h"

#include "events/requests.h"

namespace Graphics {

Renderable::Renderable(const Common::UString &scene) : _scene(scene), _rootNode(0), _visible(false), _selectable(false), _fader(0) {
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
	destroy();
}

void Renderable::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Renderable::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_fader)
		Ogre::ControllerManager::getSingleton().destroyController(_fader);
	_fader = 0;
}

void Renderable::fade(FadeDirection direction, float length, bool loop) {
	LOCK_FRAME();

	Ogre::ControllerManager &controllerMan = Ogre::ControllerManager::getSingleton();

	if (_fader)
		controllerMan.destroyController(_fader);
	_fader = 0;

	std::list<Ogre::MaterialPtr> materials;
	collectMaterials(materials, true, true);

	for (std::list<Ogre::MaterialPtr>::iterator m = materials.begin(); m != materials.end(); ++m)
		MaterialMan.setTransparent(*m, true);

	float startAlpha = 0.0f;
	if ((direction == kFadeDirectionOut) || (direction == kFadeDirectionOutIn))
		startAlpha = 1.0f;

	for (std::list<Ogre::MaterialPtr>::iterator m = materials.begin(); m != materials.end(); ++m)
		MaterialMan.setAlphaModifier(*m, startAlpha);

	Ogre::SharedPtr< Ogre::ControllerValue   <Ogre::Real> > matVal(new MaterialAlphaControllerValue(materials));
	Ogre::SharedPtr< Ogre::ControllerFunction<Ogre::Real> > animFunc(new AnimationControllerFunction(length, 0.0, (AnimationFunction) direction, loop));

	_fader = controllerMan.createController(controllerMan.getFrameTimeSource(), matVal, animFunc);
}

void Renderable::stopFade() {
	LOCK_FRAME();

	if (_fader) {
		Ogre::ControllerManager::getSingleton().destroyController(_fader);

		std::list<Ogre::MaterialPtr> materials;
		collectMaterials(materials);

		for (std::list<Ogre::MaterialPtr>::iterator m = materials.begin(); m != materials.end(); ++m) {
			MaterialMan.setAlphaModifier(*m, 1.0);
			MaterialMan.resetTransparent(*m);
		}
	}

	_fader = 0;
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

void Renderable::getSize(float &width, float &height, float &depth) const {
	_rootNode->_update(true, true);

	getNodeSize(_rootNode, width, height, depth);
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

static void addBoundBox(Ogre::AxisAlignedBox &bound, const Ogre::SceneNode &node) {
	// Move all attached visible objects into the bounding box
	for (Ogre::SceneNode::ConstObjectIterator o = node.getAttachedObjectIterator(); o.hasMoreElements(); o.moveNext())
		bound.merge(o.current()->second->getWorldBoundingBox(false));

	// Recurse into the child nodes
	for (Ogre::Node::ConstChildNodeIterator c = node.getChildIterator(); c.hasMoreElements(); c.moveNext())
		addBoundBox(bound, *((Ogre::SceneNode *) c.current()->second));
}

void getNodeSize(Ogre::SceneNode *node, float &width, float &height, float &depth) {
	if (!node) {
		width = height = depth = 0.0;
		return;
	}

	Ogre::AxisAlignedBox bound;
	addBoundBox(bound, *node);

	const Ogre::Vector3 &min = bound.getMinimum();
	const Ogre::Vector3 &max = bound.getMaximum();

	width  = ABS(max[0] - min[0]);
	height = ABS(max[1] - min[1]);
	depth  = ABS(max[2] - min[2]);
}

} // End of namespace Graphics
