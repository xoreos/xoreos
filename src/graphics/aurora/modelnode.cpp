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
 *  A node within a 3D model.
 */

#include <cassert>
#include <cstring>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/graphics/camera.h"

#include "src/graphics/images/txi.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

static bool nodeComp(ModelNode *a, ModelNode *b) {
	return a->isInFrontOf(*b);
}

ModelNode::ModelNode(Model &model) :
	_model(&model), _parent(0), _level(0),
	_isTransparent(false), _render(false), _hasTransparencyHint(false) {

	_position[0] = 0.0f; _position[1] = 0.0f; _position[2] = 0.0f;
	_rotation[0] = 0.0f; _rotation[1] = 0.0f; _rotation[2] = 0.0f;

	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;

	_scale[0] = 1.0f;
	_scale[1] = 1.0f;
	_scale[2] = 1.0f;
}

ModelNode::~ModelNode() {
	// dtor
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
	return _boundBox.getWidth() * _model->_scale[0];
}

float ModelNode::getHeight() const {
	return _boundBox.getHeight() * _model->_scale[1];
}

float ModelNode::getDepth() const {
	return _boundBox.getDepth() * _model->_scale[2];
}

bool ModelNode::isInFrontOf(const ModelNode &node) const {
	assert(_model == node._model);

	if (_model->getType() == kModelTypeGUIFront)
		return _position[2] > node._position[2];

	return _position[2] < node._position[2];
}

void ModelNode::getPosition(float &x, float &y, float &z) const {
	x = _position[0] * _model->_scale[0];
	y = _position[1] * _model->_scale[1];
	z = _position[2] * _model->_scale[2];
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
	x = _absolutePosition.getX() * _model->_scale[0];
	y = _absolutePosition.getY() * _model->_scale[1];
	z = _absolutePosition.getZ() * _model->_scale[2];
}

Common::TransformationMatrix ModelNode::getAsolutePosition() const {
	Common::TransformationMatrix absolutePosition = _absolutePosition;
	absolutePosition.scale(_model->_scale[0], _model->_scale[1], _model->_scale[2]);

	return absolutePosition;
}

void ModelNode::setPosition(float x, float y, float z) {
	lockFrameIfVisible();

	_position[0] = x / _model->_scale[0];
	_position[1] = y / _model->_scale[1];
	_position[2] = z / _model->_scale[2];

	if (_parent)
		_parent->orderChildren();

	unlockFrameIfVisible();
}

void ModelNode::setRotation(float x, float y, float z) {
	lockFrameIfVisible();

	_rotation[0] = x;
	_rotation[1] = y;
	_rotation[2] = z;

	unlockFrameIfVisible();
}

void ModelNode::setOrientation(float x, float y, float z, float a) {
	lockFrameIfVisible();

	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = a;

	unlockFrameIfVisible();
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
	node._textures      = _textures;
	node._render        = _render;
	node._isTransparent = _isTransparent;
	node._vertexBuffer  = _vertexBuffer;
	node._indexBuffer   = _indexBuffer;

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
}

void ModelNode::setTextures(const std::vector<Common::UString> &textures) {
	lockFrameIfVisible();

	// Assert that this node should be rendered and try to load the textures.
	// NOTE: loadTextures() will automatically disable rendering of the node
	//       again when texture loading fails.
	_render = true;
	loadTextures(textures);

	unlockFrameIfVisible();
}

void ModelNode::loadTextures(const std::vector<Common::UString> &textures) {
	bool hasTexture = false;

	_textures.resize(textures.size());

	bool hasAlpha = true;
	bool isDecal  = true;

	for (size_t t = 0; t != textures.size(); t++) {

		try {

			if (!textures[t].empty() && (textures[t] != "NULL")) {
				_textures[t] = TextureMan.get(textures[t]);
				if (_textures[t].empty())
					continue;

				hasTexture = true;

				if (!_textures[t].getTexture().hasAlpha())
					hasAlpha = false;
				if (_textures[t].getTexture().getTXI().getFeatures().alphaMean == 1.0f)
					hasAlpha = false;

				if (!_textures[t].getTexture().getTXI().getFeatures().decal)
					isDecal = false;
			}

		} catch (Common::Exception &e) {
			Common::printException(e, "WARNING: ");
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
	_boundBox.clear();

	const VertexDecl vertexDecl = _vertexBuffer.getVertexDecl();
	for (VertexDecl::const_iterator vA = vertexDecl.begin(); vA != vertexDecl.end(); ++vA) {
		if ((vA->index != VPOSITION) || (vA->type != GL_FLOAT))
			continue;

		const uint32 stride = MAX<uint32>(vA->size, vA->stride / sizeof(float));

		float *vX = ((float *) vA->pointer) + 0;
		float *vY = ((float *) vA->pointer) + 1;
		float *vZ = ((float *) vA->pointer) + 2;

		for (uint32 v = 0; v < _vertexBuffer.getCount(); v++)
			_boundBox.add(vX[v * stride], vY[v * stride], vZ[v * stride]);
	}

	createCenter();
}

void ModelNode::createCenter() {

	float minX, minY, minZ, maxX, maxY, maxZ;
	_boundBox.getMin(minX, minY, minZ);
	_boundBox.getMax(maxX, maxY, maxZ);

	_center[0] = minX + ((maxX - minX) / 2.0f);
	_center[1] = minY + ((maxY - minY) / 2.0f);
	_center[2] = minZ + ((maxZ - minZ) / 2.0f);
}

const Common::BoundingBox &ModelNode::getAbsoluteBound() const {
	return _absoluteBoundBox;
}

void ModelNode::createAbsoluteBound() {
	Common::BoundingBox bound;

	createAbsoluteBound(bound);
}

void ModelNode::createAbsoluteBound(Common::BoundingBox parentPosition) {
	// Transform by our position/orientation/rotation
	parentPosition.translate(_position[0], _position[1], _position[2]);
	parentPosition.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);

	parentPosition.rotate(_rotation[0], 1.0f, 0.0f, 0.0f);
	parentPosition.rotate(_rotation[1], 0.0f, 1.0f, 0.0f);
	parentPosition.rotate(_rotation[2], 0.0f, 0.0f, 1.0f);

	parentPosition.scale(_scale[0], _scale[1], _scale[2]);

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
	for (size_t t = 0; t < _textures.size(); t++) {
		TextureMan.activeTexture(t);
		glEnable(GL_TEXTURE_2D);

		TextureMan.set(_textures[t]);
	}

	if (_textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Render the node's faces
	_vertexBuffer.draw(GL_TRIANGLES, _indexBuffer);

	// Disable the texture units again
	for (size_t i = 0; i < _textures.size(); i++) {
		TextureMan.activeTexture(i);
		glDisable(GL_TEXTURE_2D);
	}

	if (_textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModelNode::render(RenderPass pass) {
	// Apply the node's transformation

	glTranslatef(_position[0], _position[1], _position[2]);
	glRotatef(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);

	glRotatef(_rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(_rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(_rotation[2], 0.0f, 0.0f, 1.0f);

	glScalef(_scale[0], _scale[1], _scale[2]);


	// Render the node's geometry

	bool shouldRender = _render && (_indexBuffer.getCount() > 0);
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

void ModelNode::drawSkeleton(const Common::TransformationMatrix &parent, bool showInvisible) {
	Common::TransformationMatrix mine = parent;

	mine.translate(_position[0], _position[1], _position[2]);
	mine.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	mine.scale(_scale[0], _scale[1], _scale[2]);

	if (_render || showInvisible) {
		glPointSize(5.0f);

		if (_render)
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		else
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_POINTS);
			glVertex3f(mine.getX(), mine.getY(), mine.getZ());
		glEnd();

		glLineWidth(2.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);
			glVertex3f(parent.getX(), parent.getY(), parent.getZ());
			glVertex3f(mine.getX(), mine.getY(), mine.getZ());
		glEnd();
	}

	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c)
		(*c)->drawSkeleton(mine, showInvisible);
}

void ModelNode::lockFrame() {
	_model->lockFrame();
}

void ModelNode::unlockFrame() {
	_model->unlockFrame();
}

void ModelNode::lockFrameIfVisible() {
	_model->lockFrameIfVisible();
}

void ModelNode::unlockFrameIfVisible() {
	_model->unlockFrameIfVisible();
}

void ModelNode::interpolatePosition(float time, float &x, float &y, float &z) const {
	// If less than 2 keyframes, don't interpolate, just return the only position
	if (_positionFrames.size() < 2) {
		getPosition(x, y, z);
		return;
	}

	size_t lastFrame = 0;
	for (size_t i = 0; i < _positionFrames.size(); i++) {
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

	size_t lastFrame = 0;
	for (size_t i = 0; i < _orientationFrames.size(); i++) {
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
