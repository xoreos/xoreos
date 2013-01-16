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

/** @file graphics/aurora/modelnode.cpp
 *  A node within a 3D model.
 */

#include "common/util.h"
#include "common/maths.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "graphics/images/txi.h"

#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/model.h"
#include "graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

static bool nodeComp(ModelNode *a, ModelNode *b) {
	return a->isInFrontOf(*b);
}

// OpenGL < 2 vertex attribute helper functions

static void EnableVertexPos(const VertexAttrib & va) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(va.size, va.type, va.stride, va.pointer);
}

static void EnableVertexNorm(const VertexAttrib & va) {
	assert(va.size == 3);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(va.type, va.stride, va.pointer);
}

static void EnableVertexCol(const VertexAttrib & va) {
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(va.size, va.type, va.stride, va.pointer);
}

static void EnableVertexTex(const VertexAttrib & va) {
	glClientActiveTexture(GL_TEXTURE0 + va.index - VTCOORD);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(va.size, va.type, va.stride, va.pointer);
}

static void DisableVertexPos(const VertexAttrib & va) {
	glDisableClientState(GL_VERTEX_ARRAY);
}

static void DisableVertexNorm(const VertexAttrib & va) {
	glDisableClientState(GL_NORMAL_ARRAY);
}

static void DisableVertexCol(const VertexAttrib & va) {
	glDisableClientState(GL_COLOR_ARRAY);
}

static void DisableVertexTex(const VertexAttrib & va) {
	glClientActiveTexture(GL_TEXTURE0 + va.index - VTCOORD);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void EnableVertexAttrib(const VertexAttrib & va) {
	if (va.index == VPOSITION)
		EnableVertexPos(va);
	else if (va.index == VNORMAL)
		EnableVertexNorm(va);
	else if (va.index == VCOLOR)
		EnableVertexCol(va);
	else if (va.index >= VTCOORD)
		EnableVertexTex(va);
}

static void DisableVertexAttrib(const VertexAttrib & va) {
	if (va.index == VPOSITION)
		DisableVertexPos(va);
	else if (va.index == VNORMAL)
		DisableVertexNorm(va);
	else if (va.index == VCOLOR)
		DisableVertexCol(va);
	else if (va.index >= VTCOORD)
		DisableVertexTex(va);
}

ModelNode::ModelNode(Model &model) :
	_model(&model), _parent(0), _level(0),
	_vertCount(0), _vertSize(0), _vertData(0),
	_faceCount(0), _faceSize(0), _faceType(0), _faceData(0),
	_isTransparent(false), _render(false), _hasTransparencyHint(false) {

	_position[0] = 0.0; _position[1] = 0.0; _position[2] = 0.0;
	_rotation[0] = 0.0; _rotation[1] = 0.0; _rotation[2] = 0.0;

	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
	_orientation[3] = 0.0;
}

ModelNode::~ModelNode() {
	std::free(_vertData);
	std::free(_faceData);
}

ModelNode *ModelNode::getParent() {
	return _parent;
}

const ModelNode *ModelNode::getParent() const {
	return _parent;
}

void ModelNode::setParent(ModelNode *parent) {
	_parent = parent;

	if (_parent) {
		_level = parent->_level + 1;
		_parent->_children.push_back(this);
	}
}

const Common::UString &ModelNode::getName() const {
	return _name;
}

float ModelNode::getWidth() const {
	return _boundBox.getWidth() * _model->_modelScale[0];
}

float ModelNode::getHeight() const {
	return _boundBox.getHeight() * _model->_modelScale[1];
}

float ModelNode::getDepth() const {
	return _boundBox.getDepth() * _model->_modelScale[2];
}

bool ModelNode::isInFrontOf(const ModelNode &node) const {
	assert(_model == node._model);

	if (_model->getType() == kModelTypeGUIFront)
		return _position[2] > node._position[2];

	return _position[2] < node._position[2];
}

void ModelNode::getPosition(float &x, float &y, float &z) const {
	x = _position[0] * _model->_modelScale[0];
	y = _position[1] * _model->_modelScale[1];
	z = _position[2] * _model->_modelScale[2];
}

void ModelNode::getRotation(float &x, float &y, float &z) const {
	x = _rotation[0];
	y = _rotation[1];
	z = _rotation[2];
}

void ModelNode::getOrientation(float &x, float &y, float &z, float &a) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];
	a = _orientation[3];
}

void ModelNode::getAbsolutePosition(float &x, float &y, float &z) const {
	x = _absolutePosition.getX() * _model->_modelScale[0];
	y = _absolutePosition.getY() * _model->_modelScale[1];
	z = _absolutePosition.getZ() * _model->_modelScale[2];
}

void ModelNode::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_position[0] = x / _model->_modelScale[0];
	_position[1] = y / _model->_modelScale[1];
	_position[2] = z / _model->_modelScale[2];

	if (_parent)
		_parent->orderChildren();

	_model->needRebuild();

	GfxMan.unlockFrame();
}

void ModelNode::setRotation(float x, float y, float z) {
	GfxMan.lockFrame();

	_rotation[0] = x;
	_rotation[1] = y;
	_rotation[2] = z;

	_model->needRebuild();

	GfxMan.unlockFrame();
}

void ModelNode::setOrientation(float x, float y, float z, float a) {
	GfxMan.lockFrame();

	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = a;

	_model->needRebuild();

	GfxMan.unlockFrame();
}

void ModelNode::move(float x, float y, float z) {
	float curX, curY, curZ;
	getPosition(curX, curY, curZ);

	setPosition(curX + x, curY + y, curZ + z);
}

void ModelNode::rotate(float x, float y, float z) {
	setRotation(_rotation[0] + x, _rotation[1] + y, _rotation[2] + z);
}

void ModelNode::inheritPosition(ModelNode &node) const {
	node._position[0] = _position[0];
	node._position[1] = _position[1];
	node._position[2] = _position[2];
}

void ModelNode::inheritOrientation(ModelNode &node) const {
	node._orientation[0] = _orientation[0];
	node._orientation[1] = _orientation[1];
	node._orientation[2] = _orientation[2];
	node._orientation[3] = _orientation[3];
}

void ModelNode::inheritGeometry(ModelNode &node) const {
	assert(!node._vertData);
	assert(!node._faceData);

	node._textures = _textures;

	node._render        = _render;
	node._isTransparent = _isTransparent;

	node._vertDecl = _vertDecl;
	node._vertCount = _vertCount;
	node._vertSize = _vertSize;
	node._vertData = std::malloc(_vertCount * _vertSize);
	memcpy(node._vertData, _vertData, _vertCount * _vertSize);

	node._faceCount = _faceCount;
	node._faceSize = _faceSize;
	node._faceType = _faceType;
	node._faceData = std::malloc(_faceCount * _faceSize);
	memcpy(node._faceData, _faceData, _faceCount * _faceSize);

	memcpy(node._center, _center, 3 * sizeof(float));
	node._boundBox = _boundBox;
}

void ModelNode::reparent(ModelNode &parent) {
	_model = parent._model;
	_level = parent._level + 1;

	_model->_currentState->nodeList.push_back(this);
	_model->_currentState->nodeMap.insert(std::make_pair(_name, this));

	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c)
		(*c)->reparent(parent);
}

void ModelNode::addChild(Model *model) {
	if (!model || !model->_currentState) {
		delete model;
		return;
	}

	model->hide();

	bool visible = _model->isVisible();
	_model->hide();

	// Take over the nodes in the model's currentstate

	for (Model::NodeList::iterator r = model->_currentState->rootNodes.begin();
	     r != model->_currentState->rootNodes.end(); ++r) {

		// TODO: Maybe we're REPLACING an existing node?
		_children.push_back(*r);

		(*r)->reparent(*this);
	}

	// Remove the nodes from the model's current state

	for (Model::StateList::iterator s = model->_stateList.begin();
	     s != model->_stateList.end(); ++s) {

		if (*s == model->_currentState) {
			(*s)->nodeList.clear();
			(*s)->nodeMap.clear();
			(*s)->rootNodes.clear();
		}
	}

	for (Model::StateMap::iterator s = model->_stateMap.begin();
	     s != model->_stateMap.end(); ++s) {

		if (s->second == model->_currentState) {
			s->second->nodeList.clear();
			s->second->nodeMap.clear();
			s->second->rootNodes.clear();
		}
	}

	// Delete the model
	delete model;

	// Rebuild our model
	_model->finalize();

	if (visible)
		_model->show();
}

void ModelNode::setInvisible(bool invisible) {
	_render = !invisible;
	_model->needRebuild();
}

void ModelNode::loadTextures(const std::vector<Common::UString> &textures) {
	bool hasTexture = false;

	_textures.resize(textures.size());

	bool hasAlpha = true;
	bool isDecal  = true;

	for (uint t = 0; t != textures.size(); t++) {

		try {

			if (!textures[t].empty() && (textures[t] != "NULL")) {
				_textures[t] = TextureMan.get(textures[t]);
				hasTexture = true;

				if (!_textures[t].getTexture().hasAlpha())
					hasAlpha = false;
				if (_textures[t].getTexture().getTXI().getFeatures().alphaMean == 1.0)
					hasAlpha = false;

				if (!_textures[t].getTexture().getTXI().getFeatures().decal)
					isDecal = false;
			}

		} catch (...) {
			warning("Failed loading texture \"%s\"", textures[t].c_str());
		}

	}

	if (_hasTransparencyHint) {
		_isTransparent = _transparencyHint;
		if (isDecal)
			_isTransparent = true;
	} else {
		_isTransparent = hasAlpha;
	}

	// If the node has no actual texture, we just assume
	// that the geometry shouldn't be rendered.
	if (!hasTexture)
		_render = false;
}

void ModelNode::createBound() {
	assert(_vertDecl[0].index == VPOSITION);
	assert(_vertDecl[0].type == GL_FLOAT);
	uint32 stride = MAX<uint32>(_vertDecl[0].size, _vertDecl[0].stride / sizeof(float));
	float *vX = (float *) _vertDecl[0].pointer;
	float *vY = vX + 1;
	float *vZ = vY + 1;
	for (uint32 v = 0; v < _vertCount; v++)
		_boundBox.add(vX[v * stride], vY[v * stride], vZ[v * stride]);

	createCenter();
}

void ModelNode::createCenter() {

	float minX, minY, minZ, maxX, maxY, maxZ;
	_boundBox.getMin(minX, minY, minZ);
	_boundBox.getMax(maxX, maxY, maxZ);

	_center[0] = minX + ((maxX - minX) / 2.0);
	_center[1] = minY + ((maxY - minY) / 2.0);
	_center[2] = minZ + ((maxZ - minZ) / 2.0);
}

const Common::BoundingBox &ModelNode::getAbsoluteBound() const {
	return _absoluteBoundBox;
}

void ModelNode::createAbsoluteBound(Common::BoundingBox parentPosition) {
	// Transform by our position/orientation/rotation
	parentPosition.translate(_position[0], _position[1], _position[2]);
	parentPosition.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);

	parentPosition.rotate(_rotation[0], 1.0, 0.0, 0.0);
	parentPosition.rotate(_rotation[1], 0.0, 1.0, 0.0);
	parentPosition.rotate(_rotation[2], 0.0, 0.0, 1.0);


	// That's our absolute position
	_absolutePosition = parentPosition.getOrigin();


	// Add our bounding box, creating the absolute bounding box
	_absoluteBoundBox = parentPosition;
	_absoluteBoundBox.add(_boundBox);
	_absoluteBoundBox.absolutize();


	// Recurse into the children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		(*c)->createAbsoluteBound(parentPosition);

		_absoluteBoundBox.add((*c)->getAbsoluteBound());
	}
}

void ModelNode::orderChildren() {
	_children.sort(nodeComp);

	// Order the children's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c)
		(*c)->orderChildren();
}

void ModelNode::renderGeometry() {
	// Enable all needed texture units
	for (uint32 t = 0; t < _textures.size(); t++) {
		TextureMan.activeTexture(t);
		glEnable(GL_TEXTURE_2D);

		TextureMan.set(_textures[t]);
	}

	// Render the node's faces

	for (uint32 i = 0; i < _vertDecl.size(); i++)
		EnableVertexAttrib(_vertDecl[i]);

	glDrawElements(GL_TRIANGLES, _faceCount * 3, _faceType, _faceData);

	for (uint32 i = 0; i < _vertDecl.size(); i++)
		DisableVertexAttrib(_vertDecl[i]);

	// Disable the texture units again
	for (uint32 i = 0; i < _textures.size(); i++) {
		TextureMan.activeTexture(i);
		glDisable(GL_TEXTURE_2D);
	}
}

void ModelNode::render(RenderPass pass) {
	// Apply the node's transformation

	glTranslatef(_position[0], _position[1], _position[2]);
	glRotatef(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);

	glRotatef(_rotation[0], 1.0, 0.0, 0.0);
	glRotatef(_rotation[1], 0.0, 1.0, 0.0);
	glRotatef(_rotation[2], 0.0, 0.0, 1.0);


	// Render the node's geometry

	bool shouldRender = _render && (_faceCount > 0);
	if (((pass == kRenderPassOpaque)      &&  _isTransparent) ||
	    ((pass == kRenderPassTransparent) && !_isTransparent))
		shouldRender = false;

	if (shouldRender)
		renderGeometry();


	// Render the node's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		glPushMatrix();
		(*c)->render(pass);
		glPopMatrix();
	}
}

void ModelNode::interpolatePosition(float time, float &x, float &y, float &z) const {
	// If less than 2 keyframes, don't interpolate, just return the only position
	if (_positionFrames.size() < 2) {
		getPosition(x, y, z);
		return;
	}

	uint32 lastFrame = 0;
	for (uint32 i = 0; i < _positionFrames.size(); i++) {
		const PositionKeyFrame &pos = _positionFrames[i];
		if (pos.time >= time)
			break;

		lastFrame = i;
	}

	const PositionKeyFrame &last = _positionFrames[lastFrame];
	if (lastFrame + 1 >= _positionFrames.size() || last.time == time) {
		x = last.x;
		y = last.y;
		z = last.z;
		return;
	}

	const PositionKeyFrame &next = _positionFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	x = f * next.x + (1.0f - f) * last.x;
	y = f * next.y + (1.0f - f) * last.y;
	z = f * next.z + (1.0f - f) * last.z;
}

void ModelNode::interpolateOrientation(float time, float &x, float &y, float &z, float &a) const {
	// If less than 2 keyframes, don't interpolate just return the only orientation
	if (_orientationFrames.size() < 2) {
		getOrientation(x, y, z, a);
		return;
	}

	uint32 lastFrame = 0;
	for (uint32 i = 0; i < _orientationFrames.size(); i++) {
		const QuaternionKeyFrame &pos = _orientationFrames[i];
		if (pos.time >= time)
			break;

		lastFrame = i;
	}

	const QuaternionKeyFrame &last = _orientationFrames[lastFrame];
	if (lastFrame + 1 >= _orientationFrames.size() || last.time == time) {
		x = last.x;
		y = last.y;
		z = last.z;
		a = Common::rad2deg(acos(last.q) * 2.0);
	}

	const QuaternionKeyFrame &next = _orientationFrames[lastFrame + 1];

	const float f = (time - last.time) / (next.time - last.time);
	x = f * next.x + (1.0f - f) * last.x;
	y = f * next.y + (1.0f - f) * last.y;
	z = f * next.z + (1.0f - f) * last.z;

	const float q = f * next.q + (1.0f - f) * last.q;
	a = Common::rad2deg(acos(q) * 2.0);
}

} // End of namespace Aurora

} // End of namespace Graphics
