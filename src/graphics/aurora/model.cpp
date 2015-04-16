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

#include <SDL_timer.h>

#include "src/common/stream.h"
#include "src/common/debug.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/modelnode.h"

#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/shader/materialman.h"
#include "src/graphics/mesh/meshman.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Model::Model(ModelType type) : Renderable((RenderableType) type),
	_type(type), _supermodel(0), _currentState(0),
	_currentAnimation(0), _nextAnimation(0), _drawBound(false) {

	_position[0] = 0.0; _position[1] = 0.0; _position[2] = 0.0;
	_rotation[0] = 0.0; _rotation[1] = 0.0; _rotation[2] = 0.0;

	_modelScale[0] = 1.0; _modelScale[1] = 1.0; _modelScale[2] = 1.0;

	// TODO: Is this the same as modelScale for non-UI?
	_animationScale = 1.0;
	_elapsedTime = 0.0;

	_loopAnimation = 0;

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

	if (_boundRenderable) {
		delete _boundRenderable;
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
	lockFrameIfVisible();

	_position[0] = x / _modelScale[0];
	_position[1] = y / _modelScale[1];
	_position[2] = z / _modelScale[2];

	createAbsolutePosition();
	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void Model::setRotation(float x, float y, float z) {
	lockFrameIfVisible();

	_rotation[0] = x;
	_rotation[1] = y;
	_rotation[2] = z;

	createAbsolutePosition();
	calculateDistance();

	resort();

	unlockFrameIfVisible();
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

	bool visible = isVisible();
	if (visible) {
		lockFrame();
		hide();
	}

	_currentState = state;

	// TODO: Do we need to recreate the bounding box on a state change?

	// createBound();

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

static std::list<ModelNode *> kEmptyNodeList;
const std::list<ModelNode *> &Model::getNodes() {
	if (!_currentState)
		return kEmptyNodeList;

	return _currentState->nodeList;
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

	Common::TransformationMatrix tform = _absolutePosition;
	tform.translate((maxX + minX) * 0.5f, (maxY + minY) * 0.5f, (maxZ + minZ) * 0.5f);
	tform.scale((maxX - minX) * 0.5f, (maxY - minY) * 0.5f, (maxZ - minZ) * 0.5f);

	_boundRenderable->renderImmediate(tform);
	/*
	glUseProgram(_boundRenderable->getProgram()->glid);
	_boundRenderable->getMaterial()->bindProgram(_boundRenderable->getProgram());
	_boundRenderable->getMaterial()->bindGLState();
	_boundRenderable->getSurface()->bindProgram(_boundRenderable->getProgram(), &tform);
	_boundRenderable->getSurface()->bindGLState();

	_boundRenderable->getMesh()->renderImmediate();

	_boundRenderable->getSurface()->unbindGLState();
	_boundRenderable->getMaterial()->unbindGLState();
	glUseProgram(0);
	*/
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
