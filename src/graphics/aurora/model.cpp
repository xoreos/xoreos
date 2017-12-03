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
 *  A 3D model of an object.
 */

#include <cassert>
#include <cstdlib>

#include <SDL_timer.h>

#include "src/common/readstream.h"
#include "src/common/debug.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/animnode.h"

#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/shader/materialman.h"
#include "src/graphics/mesh/meshman.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Model::Model(ModelType type) : Renderable((RenderableType) type),
	_type(type), _superModel(0), _currentState(0),
	_currentAnimation(0), _nextAnimation(0), _skinned(false), _drawBound(false),
	_drawSkeleton(false), _drawSkeletonInvisible(false) {

	_scale   [0] = 1.0f; _scale   [1] = 1.0f; _scale   [2] = 1.0f;
	_position[0] = 0.0f; _position[1] = 0.0f; _position[2] = 0.0f;

	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;

	_center[0] = 0.0f; _center[1] = 0.0f; _center[2] = 0.0f;

	// TODO: Is this the same as modelScale for non-UI?
	_animationScale = 1.0f;

	_animationSpeed  = 1.0f;
	_animationLength = 1.0f;
	_animationTime   = 0.0f;

	_animationLoopLength = 1.0f;
	_animationLoopTime   = 0.0f;

	_boundRenderable = new Shader::ShaderRenderable();
	_boundRenderable->setSurface(SurfaceMan.getSurface("defaultSurface"));
	_boundRenderable->setMaterial(MaterialMan.getMaterial("defaultWhite"));
	_boundRenderable->setMesh(MeshMan.getMesh("defaultWireBox"));
}

Model::~Model() {
	hide();

	for (AnimationMap::iterator a = _animationMap.begin(); a != _animationMap.end(); ++a)
		delete a->second;

	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s) {
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			delete *n;

		delete *s;
	}

	delete _boundRenderable;
}

ModelType Model::getType() const {
	return _type;
}

const Common::UString &Model::getName() const {
	return _name;
}

bool Model::isIn(float x, float y) const {
	if (_type == kModelTypeGUIFront) {
		const float minX = _position[0];
		const float minY = _position[1];
		const float maxX = minX + (_boundBox.getWidth()  * _scale[0]);
		const float maxY = minY + (_boundBox.getHeight() * _scale[1]);

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
	return _boundBox.getWidth() * _scale[0];
}

float Model::getHeight() const {
	return _boundBox.getHeight() * _scale[1];
}

float Model::getDepth() const {
	return _boundBox.getDepth() * _scale[2];
}

void Model::drawBound(bool enabled) {
	_drawBound = enabled;
}

void Model::drawSkeleton(bool enabled, bool showInvisible) {
	_drawSkeleton = enabled;
	_drawSkeletonInvisible = showInvisible;
}

void Model::setEnvironmentMap(const Common::UString &environmentMap) {
	lockFrameIfVisible();

	for (StateList::iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		for (NodeList::iterator n = (*s)->nodeList.begin(); n != (*s)->nodeList.end(); ++n)
			(*n)->setEnvironmentMap(environmentMap);

	unlockFrameIfVisible();
}

void Model::playAnimation(const Common::UString &anim, bool restart, float length, float speed) {
	Animation *animation = getAnimation(anim);
	if (!animation || (speed <= 0.0f))
		return;

	if (length == 0.0f)
		length = animation->getLength() / speed;

	_animationSpeed  = speed;
	_animationLength = length;
	_animationTime   = 0.0f;

	_animationLoopLength = animation->getLength();

	if (restart || (animation != _currentAnimation))
		_nextAnimation = animation;
}

void Model::playAnimationCount(const Common::UString &anim, bool restart, int32 loopCount) {
	Animation *animation = getAnimation(anim);
	if (!animation)
		return;

	float length = -1.0f;
	if (loopCount >= 0)
		length = (loopCount + 1) * animation->getLength();

	playAnimation(anim, restart, length, 1.0f);
}

void Model::playDefaultAnimation() {
	_nextAnimation = selectDefaultAnimation();

	_animationSpeed  = 1.0f;
	_animationLength = 1.0f;
	_animationTime   = 0.0f;

	_animationLoopLength = 1.0f;
	_animationLoopTime   = 0.0f;

	if (_nextAnimation) {
		_animationLength     = _nextAnimation->getLength();
		_animationLoopLength = _nextAnimation->getLength();
	}
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

void Model::setCurrentAnimation(Animation *anim) {
	if (!_currentState)
		return;

	makeAnimationNodeMap(anim);

	_currentAnimation  = anim;
	_animationLoopTime = 0.0f;

	for (NodeList::iterator n = _currentState->nodeList.begin(); n != _currentState->nodeList.end(); ++n)
		if ((*n)->_attachedModel) {
			(*n)->_attachedModel->makeAnimationNodeMap(anim);

			(*n)->_attachedModel->_currentAnimation  = anim;
			(*n)->_attachedModel->_animationLoopTime = 0.0f;
		}

}

void Model::makeAnimationNodeMap(Animation *anim) {
	if (!anim)
		return;

	const std::list<AnimNode *> &nodes = anim->getNodes();
	int maxNodeNumber = -1;

	for (std::list<AnimNode *>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		int nodeNumber = (*n)->getNodeData()->getNodeNumber();
		if (nodeNumber > maxNodeNumber) {
			maxNodeNumber = nodeNumber;
		}
	}

	_animationNodeMap.clear();

	if (maxNodeNumber >= 0) {
		_animationNodeMap.resize(maxNodeNumber + 1, 0);
		for (std::list<AnimNode *>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
			ModelNode *animNode = (*n)->getNodeData();
			int nodeNumber = animNode->getNodeNumber();
			_animationNodeMap[nodeNumber] = getNode(animNode->getName());
		}
	}
}

void Model::getScale(float &x, float &y, float &z) const {
	x = _scale[0];
	y = _scale[1];
	z = _scale[2];
}

void Model::getOrientation(float &x, float &y, float &z, float &angle) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];

	angle = _orientation[3];
}

void Model::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Model::getAbsolutePosition(float &x, float &y, float &z) const {
	x = _absolutePosition.getX();
	y = _absolutePosition.getY();
	z = _absolutePosition.getZ();
}

void Model::setScale(float x, float y, float z) {
	lockFrameIfVisible();

	_scale[0] = x;
	_scale[1] = y;
	_scale[2] = z;

	createAbsolutePosition();
	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void Model::setOrientation(float x, float y, float z, float angle) {
	lockFrameIfVisible();

	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = angle;

	createAbsolutePosition();
	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void Model::setPosition(float x, float y, float z) {
	lockFrameIfVisible();

	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	createAbsolutePosition();
	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void Model::scale(float x, float y, float z) {
	setScale(_scale[0] * x, _scale[1] * y, _scale[2] * z);
}

void Model::rotate(float x, float y, float z, float angle) {
	Common::Matrix4x4 orientation;

	orientation.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	orientation.rotate(angle, x, y, z);

	orientation.getAxisAngle(angle, x, y, z);

	setOrientation(x, y, z, angle);
}

void Model::move(float x, float y, float z) {
	setPosition(_position[0] + x, _position[1] + y, _position[2] + z);
}

void Model::getTooltipAnchor(float &x, float &y, float &z) const {
	Common::Matrix4x4 pos = _absolutePosition;

	pos.translate(0.0f, 0.0f, _absoluteBoundBox.getDepth());

	pos.getPosition(x, y, z);
}

void Model::createAbsolutePosition() {
	_absolutePosition.loadIdentity();

	_absolutePosition.translate(_position[0], _position[1], _position[2]);
	_absolutePosition.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	_absolutePosition.scale(_scale[0], _scale[1], _scale[2]);

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

	bool visible = isVisible();
	if (visible) {
		lockFrame();
		hide();
	}

	_currentState = state;

	createBound();

	if (visible) {
		show();
		unlockFrame();
	}
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
		if (_superModel)
			return _superModel->getNode(node);

		return 0;
	}

	return n->second;
}

const ModelNode *Model::getNode(const Common::UString &node) const {
	if (!_currentState)
		return 0;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if (n == _currentState->nodeMap.end()) {
		if (_superModel)
			return _superModel->getNode(node);

		return 0;
	}

	return n->second;
}

ModelNode *Model::getNode(const Common::UString &stateName, const Common::UString &node) {
	StateMap::const_iterator s = _stateMap.find(stateName);
	if (s == _stateMap.end())
		return 0;

	State *state = s->second;

	NodeMap::iterator n = state->nodeMap.find(node);
	if (n == state->nodeMap.end()) {
		if (_superModel)
			return _superModel->getNode(stateName, node);

		return 0;
	}

	return n->second;
}

const ModelNode *Model::getNode(const Common::UString &stateName, const Common::UString &node) const {
	StateMap::const_iterator s = _stateMap.find(stateName);
	if (s == _stateMap.end())
		return 0;

	State *state = s->second;

	NodeMap::const_iterator n = state->nodeMap.find(node);
	if (n == state->nodeMap.end()) {
		if (_superModel)
			return _superModel->getNode(stateName, node);

		return 0;
	}

	return n->second;
}

ModelNode *Model::getNode(uint16 nodeNumber) {
	if (_currentState) {
		const std::list<ModelNode *> &nodes = _currentState->nodeList;
		for (NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			ModelNode *node = *it;
			if (node->getNodeNumber() == nodeNumber) {
				return node;
			}
		}
	}
	return 0;
}

const ModelNode *Model::getNode(uint16 nodeNumber) const {
	if (_currentState) {
		const std::list<ModelNode *> &nodes = _currentState->nodeList;
		for (NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			ModelNode *node = *it;
			if (node->getNodeNumber() == nodeNumber) {
				return node;
			}
		}
	}
	return 0;
}

static std::list<ModelNode *> kEmptyNodeList;
const std::list<ModelNode *> &Model::getNodes() {
	if (!_currentState)
		return kEmptyNodeList;

	return _currentState->nodeList;
}

void Model::attachModel(const Common::UString &nodeName, Model *model) {
	ModelNode *node = getNode(nodeName);
	if (node) {
		node->_attachedModel = model;
		createBound();
	}
}

Animation *Model::getAnimation(const Common::UString &anim) {

	AnimationMap::iterator n = _animationMap.find(anim);
	if (n == _animationMap.end()) {
		if (_superModel)
			return _superModel->getAnimation(anim);

		return 0;
	}

	return n->second;
}

bool Model::hasAnimation(const Common::UString &anim) const {
	return _animationMap.find(anim) != _animationMap.end();
}

float Model::getAnimationScale(const Common::UString &anim) {
	// TODO: We can cache this for performance
	AnimationMap::iterator n = _animationMap.find(anim);
	if (n == _animationMap.end()) {
		// Animation scaling only applies to inherited animations
		if (_superModel)
			return _animationScale * _superModel->getAnimationScale(anim);
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


	Common::Matrix4x4 center = _absolutePosition;

	center.translate(_center[0], _center[1], _center[2]);


	const float cameraX = -CameraMan.getPosition()[0];
	const float cameraY = -CameraMan.getPosition()[1];
	const float cameraZ = -CameraMan.getPosition()[2];

	const float x = ABS(center.getX() - cameraX);
	const float y = ABS(center.getY() - cameraY);
	const float z = ABS(center.getZ() - cameraZ);


	_distance = x + y + z;
}

void Model::advanceTime(float dt) {
	manageAnimations(dt);
}

void Model::setSkinned(bool skinned) {
	_skinned = skinned;
}

void Model::manageAnimations(float dt) {
	float lastFrame = _animationLoopTime;
	float nextFrame = _animationLoopTime + _animationSpeed * dt;

	// No animation and no new one scheduled? Select a default one
	if (!_currentAnimation && !_nextAnimation)
		playDefaultAnimation();

	// Start a new animation if scheduled, interrupting the currently playing animation
	if (_nextAnimation) {
		setCurrentAnimation(_nextAnimation);
		_nextAnimation = 0;

		dt           = 0.0f;
		lastFrame    = 0.0f;
		nextFrame    = 0.0f;
	}

	if (!_currentAnimation)
		return;

	// The loop of the animation ended: make sure to play the last frame
	if ((lastFrame < _animationLoopLength) && (nextFrame >= _animationLoopLength)) {
		_currentAnimation->update(this, lastFrame, _animationLoopLength);

		_animationTime    += dt;
		_animationLoopTime = _animationLoopLength;
		return;
	}

	// The animation has run its requested course: return to the default animation.
	if ((_animationLength >= 0.0f) && (_animationTime >= _animationLength)) {
		playDefaultAnimation();
		setCurrentAnimation(_nextAnimation);
		_nextAnimation = 0;

		if (_currentAnimation)
			_currentAnimation->update(this, 0.0f, 0.0f);

		createBound();
		return;
	}

	// Start the next loop of the animation
	if (lastFrame >= _animationLoopLength) {
		_currentAnimation->update(this, 0.0f, 0.0f);

		lastFrame = 0.0f;
		nextFrame = _animationSpeed * dt;

		createBound();
	}

	// Update the animation
	_currentAnimation->update(this, lastFrame, nextFrame);

	_animationTime    += dt;
	_animationLoopTime = nextFrame;

	if (nextFrame == 0.0f)
		createBound();
}

void Model::render(RenderPass pass) {
	if (!_currentState || (pass > kRenderPassAll))
		return;

	if (pass == kRenderPassAll) {
		Model::render(kRenderPassOpaque);
		Model::render(kRenderPassTransparent);
		return;
	}

	// Apply our global model transformation
	glTranslatef(_position[0], _position[1], _position[2]);
	glRotatef(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	glScalef(_scale[0], _scale[1], _scale[2]);

	// Draw the bounding box, if requested
	doDrawBound();

	// Draw the nodes
	for (NodeList::iterator n = _currentState->rootNodes.begin();
	     n != _currentState->rootNodes.end(); ++n) {

		glPushMatrix();
		(*n)->render(pass);
		glPopMatrix();
	}

	// Reset the first texture units
	TextureMan.reset();

	// Draw the skeleton, if requested
	doDrawSkeleton();
}

void Model::doDrawBound() {
	if (!_drawBound)
		return;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	Common::BoundingBox object = _boundBox;

	float minX, minY, minZ, maxX, maxY, maxZ;
	object.getMin(minX, minY, minZ);
	object.getMax(maxX, maxY, maxZ);

	/*
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
	*/

	Common::Matrix4x4 tform = _absolutePosition;
	tform.translate((maxX + minX) * 0.5f, (maxY + minY) * 0.5f, (maxZ + minZ) * 0.5f);
	tform.scale((maxX - minX) * 0.5f, (maxY - minY) * 0.5f, (maxZ - minZ) * 0.5f);
	_boundRenderable->renderImmediate(tform);
}

void Model::doDrawSkeleton() {
	if (!_drawSkeleton)
		return;

	Common::Matrix4x4 tform;

	if (_type == kModelTypeObject)
		glDisable(GL_DEPTH_TEST);

	for (NodeList::iterator n = _currentState->rootNodes.begin(); n != _currentState->rootNodes.end(); ++n)
		(*n)->drawSkeleton(tform, _drawSkeletonInvisible);

	if (_type == kModelTypeObject)
		glEnable(GL_DEPTH_TEST);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

void Model::doRebuild() {
	// TODO: remove this and all references to it.
}

void Model::doDestroy() {
	// TODO: remove this and all references to it.
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

	_currentAnimation = selectDefaultAnimation();
	makeAnimationNodeMap(_currentAnimation);
}

void Model::createStateNamesList(std::list<Common::UString> *stateNames) {
	bool isRoot = false;

	if (!stateNames) {
		_stateNames.clear();

		stateNames = &_stateNames;
		isRoot     = true;
	}

	for (StateList::const_iterator s = _stateList.begin(); s != _stateList.end(); ++s)
		stateNames->push_back((*s)->name);

	if (_superModel)
		_superModel->createStateNamesList(stateNames);

	if (isRoot) {
		stateNames->sort();
		stateNames->unique();
	}
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

	_center[0] = minX + ((maxX - minX) / 2.0f);
	_center[1] = minY + ((maxY - minY) / 2.0f);
	_center[2] = minZ + ((maxZ - minZ) / 2.0f);


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

	const size_t pos = stream.seek(offset);

	values.resize(count);
	for (uint32 i = 0; i < count; i++)
		readValue(stream, values[i]);

	stream.seek(pos);
}

template
void Model::readArray<uint32>(Common::SeekableReadStream &stream,
                              uint32 offset, uint32 count, std::vector<uint32> &values);
template
void Model::readArray<float>(Common::SeekableReadStream &stream,
                             uint32 offset, uint32 count, std::vector<float> &values);

} // End of namespace Aurora

} // End of namespace Graphics
