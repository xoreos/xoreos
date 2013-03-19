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

/** @file graphics/aurora/model.cpp
 *  A 3D model of an object.
 */

#include <SDL_timer.h>

#include "common/stream.h"
#include "common/debug.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/animation.h"
#include "graphics/aurora/modelnode.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Model::Model(ModelType type) : Renderable((RenderableType) type),
	_type(type), _supermodel(0), _currentState(0),
	_currentAnimation(0), _nextAnimation(0), _drawBound(false),
	_lists(0) {

	for (int i = 0; i < kRenderPassAll; i++)
		_needBuild[i] = true;

	_position[0] = 0.0; _position[1] = 0.0; _position[2] = 0.0;
	_rotation[0] = 0.0; _rotation[1] = 0.0; _rotation[2] = 0.0;

	_modelScale[0] = 1.0; _modelScale[1] = 1.0; _modelScale[2] = 1.0;

	// TODO: Is this the same as modelScale for non-UI?
	_animationScale = 1.0;
	_elapsedTime = 0.0;

	_loopAnimation = 0;
}

Model::~Model() {
	hide();

	if (_lists != 0)
		GfxMan.abandon(_lists, kRenderPassAll);

	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s) {
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			delete *n;

		delete *s;
	}
}

ModelType Model::getType() const {
	return _type;
}

const Common::UString &Model::getName() const {
	return _name;
}

bool Model::isIn(float x, float y) const {
	if (_type == kModelTypeGUIFront) {
		x /= _modelScale[0];
		y /= _modelScale[1];

		const float minX = _position[0];
		const float minY = _position[1];
		const float maxX = minX + _boundBox.getWidth();
		const float maxY = minY + _boundBox.getHeight();

		if ((x < minX) || (x > maxX))
			return false;
		if ((y < minY) || (y > maxY))
			return false;

		return true;
	}


	return _absoluteBoundBox.isIn(x, y);
}

bool Model::isIn(float x, float y, float z) const {
	if (_type == kModelTypeGUIFront)
		return isIn(x, y);

	return _absoluteBoundBox.isIn(x, y, z);
}

bool Model::isIn(float x1, float y1, float z1, float x2, float y2, float z2) const {
	if (_type == kModelTypeGUIFront)
		return false;

	return _absoluteBoundBox.isIn(x1, y1, z1, x2, y2, z2);
}

float Model::getWidth() const {
	return _boundBox.getWidth() * _modelScale[0];
}

float Model::getHeight() const {
	return _boundBox.getHeight() * _modelScale[1];
}

float Model::getDepth() const {
	return _boundBox.getDepth() * _modelScale[2];
}

void Model::drawBound(bool enabled) {
	_drawBound = enabled;
	needRebuild();
}

void Model::playAnimation(const Common::UString &anim, bool restart, int32 loopCount) {
	Animation *animation = getAnimation(anim);
	if (!animation)
		return;

	_loopAnimation = loopCount;

	if (restart || (animation != _currentAnimation))
		_nextAnimation = animation;
}

void Model::playDefaultAnimation() {
	_nextAnimation = selectDefaultAnimation();
	_loopAnimation = 0;
}

Animation *Model::selectDefaultAnimation() const {
	uint8 pick = std::rand() % 100;
	for (DefaultAnimations::const_iterator a = _defaultAnimations.begin(); a != _defaultAnimations.end(); ++a) {
		if (pick < a->probability)
			return a->animation;

		pick -= a->probability;
	}

	return 0;
}

void Model::getPosition(float &x, float &y, float &z) const {
	x = _position[0] * _modelScale[0];
	y = _position[1] * _modelScale[1];
	z = _position[2] * _modelScale[2];
}

void Model::getRotation(float &x, float &y, float &z) const {
	x = _rotation[0];
	y = _rotation[1];
	z = _rotation[2];
}

void Model::getAbsolutePosition(float &x, float &y, float &z) const {
	x = _absolutePosition.getX();
	y = _absolutePosition.getY();
	z = _absolutePosition.getZ();
}

void Model::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_position[0] = x / _modelScale[0];
	_position[1] = y / _modelScale[1];
	_position[2] = z / _modelScale[2];

	createAbsolutePosition();
	calculateDistance();
	needRebuild();

	resort();

	GfxMan.unlockFrame();
}

void Model::setRotation(float x, float y, float z) {
	GfxMan.lockFrame();

	_rotation[0] = x;
	_rotation[1] = y;
	_rotation[2] = z;

	createAbsolutePosition();
	calculateDistance();
	needRebuild();

	resort();

	GfxMan.unlockFrame();
}

void Model::move(float x, float y, float z) {
	x /= _modelScale[0];
	y /= _modelScale[1];
	z /= _modelScale[2];

	setPosition(_position[0] + x, _position[1] + y, _position[2] + z);
}

void Model::rotate(float x, float y, float z) {
	setRotation(_rotation[0] + x, _rotation[1] + y, _rotation[2] + z);
}

void Model::getTooltipAnchor(float &x, float &y, float &z) const {
	Common::TransformationMatrix pos = _absolutePosition;

	pos.translate(0.0, 0.0, _absoluteBoundBox.getHeight() + 0.5);

	pos.getPosition(x, y, z);
}

void Model::createAbsolutePosition() {
	_absolutePosition.loadIdentity();

	_absolutePosition.scale(_modelScale[0], _modelScale[1], _modelScale[2]);

	if (_type == kModelTypeObject)
		_absolutePosition.rotate(90.0, -1.0, 0.0, 0.0);

	_absolutePosition.translate(_position[0], _position[1], _position[2]);

	_absolutePosition.rotate( _rotation[0], 1.0, 0.0, 0.0);
	_absolutePosition.rotate( _rotation[1], 0.0, 1.0, 0.0);
	_absolutePosition.rotate(-_rotation[2], 0.0, 0.0, 1.0);

	_absoluteBoundBox = _boundBox;
	_absoluteBoundBox.transform(_absolutePosition);
	_absoluteBoundBox.absolutize();
}

const std::list<Common::UString> &Model::getStates() const {
	return _stateNames;
}

void Model::setState(const Common::UString &name) {
	if (_stateList.empty())
		return;

	State *state = 0;

	StateMap::iterator s = _stateMap.find(name);
	if (s == _stateMap.end())
		s = _stateMap.find("");

	if (s != _stateMap.end())
		state = s->second;
	else
		state = _stateList.front();

	assert(state);

	if (state == _currentState)
		return;

	GfxMan.lockFrame();

	bool visible = isVisible();
	if (visible)
		hide();

	_currentState = state;

	// TODO: Do we need to recreate the bounding box on a state change?

	// createBound();

	if (visible)
		show();

	needRebuild();

	GfxMan.unlockFrame();
}

static const Common::UString kNoState;
const Common::UString &Model::getState() const {
	if (!_currentState)
		return kNoState;

	return _currentState->name;
}

bool Model::hasNode(const Common::UString &node) const {
	if (!_currentState)
		return false;

	NodeMap::iterator n = _currentState->nodeMap.find(node);
	if (n == _currentState->nodeMap.end())
		return false;

	return true;
}

ModelNode *Model::getNode(const Common::UString &node) {
	if (!_currentState)
		return 0;

	NodeMap::iterator n = _currentState->nodeMap.find(node);
	if (n == _currentState->nodeMap.end()) {
		if (_supermodel)
			return _supermodel->getNode(node);

		return 0;
	}

	return n->second;
}

const ModelNode *Model::getNode(const Common::UString &node) const {
	if (!_currentState)
		return 0;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if (n == _currentState->nodeMap.end()) {
		if (_supermodel)
			return _supermodel->getNode(node);

		return 0;
	}

	return n->second;
}

Animation *Model::getAnimation(const Common::UString &anim) {

	AnimationMap::iterator n = _animationMap.find(anim);
	if (n == _animationMap.end()) {
		if (_supermodel)
			return _supermodel->getAnimation(anim);

		return 0;
	}

	return n->second;
}

float Model::getAnimationScale(const Common::UString &anim) {
	// TODO: We can cache this for performance
	AnimationMap::iterator n = _animationMap.find(anim);
	if (n == _animationMap.end()) {
		// Animation scaling only applies to inherited animations
		if (_supermodel)
			return _animationScale * _supermodel->getAnimationScale(anim);
		// Can't find it, return sensible default
		return 1.0f;
	}
	// We found it, don't scale further
	return 1.0f;
}

void Model::calculateDistance() {
	if (_type == kModelTypeGUIFront) {
		_distance = _position[2];
		return;
	}


	Common::TransformationMatrix center = _absolutePosition;

	center.translate(_center[0], _center[1], _center[2]);


	const float cameraX =  CameraMan.getPosition()[0];
	const float cameraY =  CameraMan.getPosition()[1];
	const float cameraZ = -CameraMan.getPosition()[2];

	const float x = ABS(center.getX() - cameraX);
	const float y = ABS(center.getY() - cameraY);
	const float z = ABS(center.getZ() - cameraZ);


	_distance = x + y + z;
}

bool Model::buildList(RenderPass pass) {
	if (!_needBuild[pass])
		return false;

	if (_lists == 0)
		_lists = glGenLists(kRenderPassAll);

	glNewList(_lists + pass, GL_COMPILE);


	// Apply our global model transformation

	glScalef(_modelScale[0], _modelScale[1], _modelScale[2]);

	if (_type == kModelTypeObject)
		// Aurora world objects have a rotated axis
		glRotatef(90.0, -1.0, 0.0, 0.0);

	glTranslatef(_position[0], _position[1], _position[2]);

	glRotatef( _rotation[0], 1.0, 0.0, 0.0);
	glRotatef( _rotation[1], 0.0, 1.0, 0.0);
	glRotatef(-_rotation[2], 0.0, 0.0, 1.0);


	// Draw the bounding box, if requested
	doDrawBound();

	// Draw the nodes
	for (NodeList::iterator n = _currentState->rootNodes.begin();
	     n != _currentState->rootNodes.end(); ++n) {

		glPushMatrix();
		(*n)->render(pass);
		glPopMatrix();
	}


	glEndList();


	_needBuild[pass] = false;
	return true;
}

void Model::advanceTime(float dt) {
	manageAnimations(dt);
}

void Model::manageAnimations(float dt) {
	float lastFrame = _elapsedTime;
	float nextFrame = _elapsedTime + dt;
	_elapsedTime = nextFrame;

	// Start a new animation if scheduled, interrupting the currently playing animation
	if (_nextAnimation) {
		_currentAnimation = _nextAnimation;
		_nextAnimation    = 0;

		_elapsedTime = 0.0f;
		lastFrame    = 0.0f;
		nextFrame    = 0.0f;
	}

	// Animation finished?
	if (_currentAnimation && (nextFrame >= _currentAnimation->getLength())) {
		// Update the loop counter. If it's 0, then end the animation; otherwise, restart it

		if (_loopAnimation != 0) {
			if (_loopAnimation > 0)
				_loopAnimation--;

			_elapsedTime = 0.0f;
			lastFrame    = 0.0f;
			nextFrame    = 0.0f;
		} else
			_currentAnimation = 0;
	}

	// No animation, select a default one
	if (!_currentAnimation) {
		_currentAnimation = selectDefaultAnimation();

		_elapsedTime = 0.0f;
		lastFrame    = 0.0f;
		nextFrame    = 0.0f;
	}

	// Update the animation, if we have any
	if (_currentAnimation)
		_currentAnimation->update(this, lastFrame, nextFrame);
}

void Model::render(RenderPass pass) {
	if (!_currentState || (pass > kRenderPassAll))
		return;

	if (pass == kRenderPassAll) {
		Model::render(kRenderPassOpaque);
		Model::render(kRenderPassTransparent);
		return;
	}

	// Render
	buildList(pass);
	glCallList(_lists + pass);

	// Reset the first texture units
	TextureMan.reset();
}

void Model::doDrawBound() {
	if (!_drawBound)
		return;

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glLineWidth(1.0);

	Common::BoundingBox object = _boundBox;

	float minX, minY, minZ, maxX, maxY, maxZ;
	object.getMin(minX, minY, minZ);
	object.getMax(maxX, maxY, maxZ);

	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, minY, minZ);
		glVertex3f(maxX, minY, minZ);
		glVertex3f(maxX, maxY, minZ);
		glVertex3f(minX, maxY, minZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, minY, maxZ);
		glVertex3f(maxX, minY, maxZ);
		glVertex3f(maxX, maxY, maxZ);
		glVertex3f(minX, maxY, maxZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, minY, minZ);
		glVertex3f(minX, maxY, minZ);
		glVertex3f(minX, maxY, maxZ);
		glVertex3f(minX, minY, maxZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(maxX, minY, minZ);
		glVertex3f(maxX, maxY, minZ);
		glVertex3f(maxX, maxY, maxZ);
		glVertex3f(maxX, minY, maxZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, minY, minZ);
		glVertex3f(maxX, minY, minZ);
		glVertex3f(maxX, minY, maxZ);
		glVertex3f(minX, minY, maxZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3f(minX, maxY, minZ);
		glVertex3f(maxX, maxY, minZ);
		glVertex3f(maxX, maxY, maxZ);
		glVertex3f(minX, maxY, maxZ);
	glEnd();
}

void Model::doRebuild() {
	needRebuild();
}

void Model::doDestroy() {
	if (_lists == 0)
		return;

	glDeleteLists(_lists, kRenderPassAll);
	_lists = 0;
}

void Model::finalize() {
	_currentState = 0;

	createStateNamesList();
	setState();

	createBound();

	// Order all node children lists
	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		for (NodeList::iterator n = (*s)->rootNodes.begin(); n != (*s)->rootNodes.end(); ++n)
			(*n)->orderChildren();

	needRebuild();

	_currentAnimation = selectDefaultAnimation();
}

void Model::needRebuild() {
	for (int i = 0; i < kRenderPassAll; i++)
		_needBuild[i] = true;
}

void Model::createStateNamesList() {
	_stateNames.clear();

	for (StateList::const_iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		_stateNames.push_back((*s)->name);
}

void Model::createBound() {
	_boundBox.clear();

	if (!_currentState)
		return;

	for (NodeList::iterator n = _currentState->rootNodes.begin();
	     n != _currentState->rootNodes.end(); ++n) {

		Common::BoundingBox position;

		(*n)->createAbsoluteBound(position);

		_boundBox.add((*n)->getAbsoluteBound());
	}

	float minX, minY, minZ, maxX, maxY, maxZ;
	_boundBox.getMin(minX, minY, minZ);
	_boundBox.getMax(maxX, maxY, maxZ);

	_center[0] = minX + ((maxX - minX) / 2.0);
	_center[1] = minY + ((maxY - minY) / 2.0);
	_center[2] = minZ + ((maxZ - minZ) / 2.0);


	_absoluteBoundBox = _boundBox;
	_absoluteBoundBox.transform(_absolutePosition);
	_absoluteBoundBox.absolutize();
}

void Model::readValue(Common::SeekableReadStream &stream, uint32 &value) {
	value = stream.readUint32LE();
}

void Model::readValue(Common::SeekableReadStream &stream, float &value) {
	value = stream.readIEEEFloatLE();
}

void Model::readArrayDef(Common::SeekableReadStream &stream,
                         uint32 &offset, uint32 &count) {

	offset = stream.readUint32LE();

	uint32 usedCount      = stream.readUint32LE();
	uint32 allocatedCount = stream.readUint32LE();

	if (usedCount != allocatedCount)
		warning("Model::readArrayDef(): usedCount != allocatedCount (%d, %d)",
		        usedCount, allocatedCount);

	count = usedCount;
}

template<typename T>
void Model::readArray(Common::SeekableReadStream &stream,
                      uint32 offset, uint32 count, std::vector<T> &values) {

	uint32 pos = stream.seekTo(offset);

	values.resize(count);
	for (uint32 i = 0; i < count; i++)
		readValue(stream, values[i]);

	stream.seekTo(pos);
}

template
void Model::readArray<uint32>(Common::SeekableReadStream &stream,
                              uint32 offset, uint32 count, std::vector<uint32> &values);
template
void Model::readArray<float>(Common::SeekableReadStream &stream,
                             uint32 offset, uint32 count, std::vector<float> &values);

} // End of namespace Aurora

} // End of namespace Graphics
