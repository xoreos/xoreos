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

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/graphics/camera.h"

#include "src/graphics/images/txi.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/model.h"

#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"

#include "src/graphics/images/decoder.h"

namespace Graphics {

namespace Aurora {

static bool nodeComp(ModelNode *a, ModelNode *b) {
	return a->isInFrontOf(*b);
}

ModelNode::Skin::Skin() : boneMappingCount(0) {
}

ModelNode::Dangly::Dangly() : period(1.0f), tightness(1.0f), displacement(1.0f),
	data(0) {
}

ModelNode::MeshData::MeshData() : envMapMode(kModeEnvironmentBlendedUnder) {
}

ModelNode::Mesh::Mesh() : shininess(1.0f), alpha(1.0f), tilefade(0), render(false),
	shadow(false), beaming(false), inheritcolor(false), rotatetexture(false),
	isTransparent(false), hasTransparencyHint(false), transparencyHint(false),
	data(0), dangly(0), skin(0) {
}


ModelNode::ModelNode(Model &model)
		: _model(&model),
		  _parent(0),
		  _attachedModel(0),
		  _level(0),
		  _render(false),
		  _mesh(0),
		  _nodeNumber(0),
		  _positionBuffered(false),
		  _orientationBuffered(false),
		  _vertexCoordsBuffered(false) {

	_position[0] = 0.0f; _position[1] = 0.0f; _position[2] = 0.0f;
	_rotation[0] = 0.0f; _rotation[1] = 0.0f; _rotation[2] = 0.0f;

	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;

	_scale[0] = 1.0f;
	_scale[1] = 1.0f;
	_scale[2] = 1.0f;

	_positionBuffer[0] = 0.0f;
	_positionBuffer[1] = 0.0f;
	_positionBuffer[2] = 0.0f;

	_orientationBuffer[0] = 0.0f;
	_orientationBuffer[1] = 0.0f;
	_orientationBuffer[2] = 0.0f;
	_orientationBuffer[3] = 0.0f;

	_mesh = 0; // Should also be added to MeshMan, so this class won't "own" it.
	_material = 0;
	_shaderRenderable = 0;

	_alpha = 1.0f;
}

ModelNode::~ModelNode() {
	if (_mesh) {
		if (_mesh->dangly) {
			delete _mesh->dangly->data;
			delete _mesh->dangly;
		}
		if (_mesh->skin) {
			delete _mesh->skin;
		}
		if (_mesh->data) {
			delete _mesh->data;
		}
	}

	delete _mesh;
	_mesh = 0;

	delete _attachedModel;
	_attachedModel = 0;
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

std::list<ModelNode *> &ModelNode::getChildren() {
	return _children;
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
	x = _absolutePosition[3][0] * _model->_scale[0];
	y = _absolutePosition[3][1] * _model->_scale[1];
	z = _absolutePosition[3][2] * _model->_scale[2];
}

glm::mat4 ModelNode::getAbsolutePosition() const {
	glm::mat4 absolutePosition = _absolutePosition;
	absolutePosition = glm::scale(absolutePosition, glm::vec3(_model->_scale[0], _model->_scale[1], _model->_scale[2]));

	return absolutePosition;
}

uint16 ModelNode::getNodeNumber() const {
	return _nodeNumber;
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

void ModelNode::setEnvironmentMap(const Common::UString &environmentMap) {
	if (_attachedModel)
		_attachedModel->setEnvironmentMap(environmentMap);

	if (!_mesh || !_mesh->data)
		return;

	_mesh->data->envMap.clear();

	if (!environmentMap.empty()) {
		try {
			_mesh->data->envMap = TextureMan.get(environmentMap);
		} catch (...) {
		}
	}
	this->buildMaterial();
}

void ModelNode::setInvisible(bool invisible) {
	_render = !invisible;
}

void ModelNode::setTextures(const std::vector<Common::UString> &textures) {
	if (!_mesh || !_mesh->data)
		return;

	lockFrameIfVisible();

	// NOTE: loadTextures() will automatically disable rendering of the node
	//       again when texture loading fails.
	_render = true;
	loadTextures(textures);

	unlockFrameIfVisible();
}

ModelNode::Mesh *ModelNode::getMesh() const {
	return _mesh;
}

void ModelNode::setMaterial(Shader::ShaderMaterial *material) {
	_material = material;
	if (_shaderRenderable) {
		_shaderRenderable->setMaterial(_material);
	}
}

float ModelNode::getAlpha() {
	return _alpha;
}

void ModelNode::setAlpha(float alpha, bool isRecursive) {
	_alpha = alpha;

	if (isRecursive) {
		for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
			(*c)->setAlpha(alpha, true);
		}
	}
}

void ModelNode::loadTextures(const std::vector<Common::UString> &textures) {
	bool hasTexture = false;

	_mesh->data->textures.resize(textures.size());

	bool hasAlpha = true;
	bool isDecal  = true;

	Common::UString envMap;

	for (size_t t = 0; t != textures.size(); t++) {

		try {

			if (!textures[t].empty() && (textures[t] != "NULL")) {
				_mesh->data->textures[t] = TextureMan.get(textures[t]);
				if (_mesh->data->textures[t].empty())
					continue;

				hasTexture = true;

				if (!_mesh->data->textures[t].getTexture().hasAlpha())
					hasAlpha = false;
				if (_mesh->data->textures[t].getTexture().getTXI().getFeatures().alphaMean == 1.0f)
					hasAlpha = false;

				if (!_mesh->data->textures[t].getTexture().getTXI().getFeatures().decal)
					isDecal = false;

				if (!_mesh->data->textures[t].getTexture().getTXI().getFeatures().bumpyShinyTexture.empty())
					envMap = _mesh->data->textures[t].getTexture().getTXI().getFeatures().bumpyShinyTexture;
				if (!_mesh->data->textures[t].getTexture().getTXI().getFeatures().envMapTexture.empty())
					envMap = _mesh->data->textures[t].getTexture().getTXI().getFeatures().envMapTexture;
			}

		} catch (...) {
			Common::exceptionDispatcherWarning();
		}

	}

	envMap.trim();
	if (!envMap.empty()) {
		try {
			_mesh->data->envMap = TextureMan.get(envMap);
		} catch (...) {
			Common::exceptionDispatcherWarning();
		}
	}

	if (_mesh->hasTransparencyHint) {
		_mesh->isTransparent = _mesh->transparencyHint;
		if (isDecal)
			_mesh->isTransparent = true;
	} else {
		_mesh->isTransparent = hasAlpha;
	}

	// If the node has no actual texture, we just assume
	// that the geometry shouldn't be rendered.
	if (!hasTexture)
		_render = false;
	else {
		this->buildMaterial();
	}
}

void ModelNode::createBound() {
	_boundBox.clear();

	if (!_mesh || !_mesh->data)
		return;

	VertexBuffer vertexBuffer = _mesh->data->vertexBuffer;

	const VertexDecl vertexDecl = vertexBuffer.getVertexDecl();
	for (VertexDecl::const_iterator vA = vertexDecl.begin(); vA != vertexDecl.end(); ++vA) {
		if ((vA->index != VPOSITION) || (vA->type != GL_FLOAT))
			continue;

		const uint32 stride = MAX<uint32>(vA->size, vA->stride / sizeof(float));

		const float *vertexData = reinterpret_cast<const float *>(vA->pointer);

		const float *vX = vertexData + 0;
		const float *vY = vertexData + 1;
		const float *vZ = vertexData + 2;

		for (uint32 v = 0; v < vertexBuffer.getCount(); v++)
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

	// If this node is empty, add the root state node
	if (_boundBox.empty()) {
		ModelNode *rootStateNode = _model->getNode("", _name);
		if (rootStateNode)
			_absoluteBoundBox.add(rootStateNode->_boundBox);
	}

	_absoluteBoundBox.absolutize();

	// Recurse into the children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		(*c)->createAbsoluteBound(parentPosition);

		_absoluteBoundBox.add((*c)->getAbsoluteBound());
	}

	if (_attachedModel) {
		glm::mat4 modelPosition = _absoluteBoundBox.getOrigin();

		modelPosition = glm::translate(modelPosition, glm::vec3(_attachedModel->_position[0],
		                                                        _attachedModel->_position[1],
		                                                        _attachedModel->_position[2]));

		if (_attachedModel->_orientation[0] != 0 ||
				_attachedModel->_orientation[1] != 0 ||
				_attachedModel->_orientation[2] != 0)
			modelPosition = glm::rotate(modelPosition,
					Common::deg2rad(_attachedModel->_orientation[3]),
					glm::vec3(_attachedModel->_orientation[0],
					          _attachedModel->_orientation[1],
					          _attachedModel->_orientation[2]));

		modelPosition = glm::scale(modelPosition, glm::vec3(_attachedModel->_scale[0],
		                                                    _attachedModel->_scale[1],
		                                                    _attachedModel->_scale[2]));

		_attachedModel->_absolutePosition = modelPosition;
		_attachedModel->createBound();

		_absoluteBoundBox.add(_attachedModel->_absoluteBoundBox);
	}
}

void ModelNode::orderChildren() {
	_children.sort(nodeComp);

	// Order the children's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c)
		(*c)->orderChildren();
}


void ModelNode::renderGeometry(Mesh &mesh) {
	if (!mesh.data->envMap.empty()) {
		switch (mesh.data->envMapMode)
	}
	renderGeometryNormal(mesh);
}

void ModelNode::renderGeometryNormal(Mesh &mesh) {
	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set(mesh.data->textures[t]);
	}

	if (mesh.data->textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set();
	}

	if (mesh.data->textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModelNode::renderGeometryEnvMappedUnder(Mesh &mesh) {
	/* First draw the node with only the environment map, then simply
	 * blend a semi-transparent diffuse texture on top.
	 *
	 * Neverwinter Nights uses this method.
	 */

	TextureMan.set(mesh.data->envMap, TextureManager::kModeEnvironmentMapReflective);
	mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set(mesh.data->textures[t], TextureManager::kModeDiffuse);
	}

	mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set();
	}
}

void ModelNode::renderGeometryEnvMappedOver(Mesh &mesh) {
	/* First draw the node with diffuse textures, then draw it again with
	 * only the environment map. This performs a more complex blending of
	 * the textures, allowing the color of a transparent diffuse texture
	 * to modulate the color of the environment map.
	 *
	 * KotOR and KotOR2 use this method.
	 */

	if (!mesh.data->textures.empty()) {
		for (size_t t = 0; t < mesh.data->textures.size(); t++) {
			TextureMan.activeTexture(t);
			TextureMan.set(mesh.data->textures[t], TextureManager::kModeDiffuse);
		}

		glBlendFunc(GL_ONE, GL_ZERO);

		mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);

		for (size_t t = 0; t < mesh.data->textures.size(); t++) {
			TextureMan.activeTexture(t);
			TextureMan.set();
		}

		TextureMan.activeTexture(0);
		TextureMan.set(mesh.data->textures[0], TextureManager::kModeDiffuse);

		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_ZERO, GL_ONE);

		mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);
	}

	TextureMan.activeTexture(0);
	TextureMan.set(mesh.data->envMap, TextureManager::kModeEnvironmentMapReflective);

	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

	mesh.data->vertexBuffer.draw(GL_TRIANGLES, mesh.data->indexBuffer);

	TextureMan.set();

	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool ModelNode::renderableMesh(Mesh *mesh) {
	return mesh && mesh->data && mesh->data->indexBuffer.getCount() > 0;
}

void ModelNode::render(RenderPass pass, const Common::TransformationMatrix &parentTransform) {
	// Apply the node's transformation

	glTranslatef(_position[0], _position[1], _position[2]);
	glRotatef(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);

	glRotatef(_rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(_rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(_rotation[2], 0.0f, 0.0f, 1.0f);

	glScalef(_scale[0], _scale[1], _scale[2]);

	Mesh *mesh = _mesh;
	bool doRender = _render;
	if (!_model->getState().empty() && !renderableMesh(mesh)) {
		ModelNode *rootStateNode = _model->getNode("", _name);
		if (rootStateNode && renderableMesh(rootStateNode->_mesh)) {
			mesh = rootStateNode->_mesh;
			doRender = rootStateNode->_render;
		}
	}

	//_renderTransform.loadIdentity();
	_renderTransform = parentTransform;
	_renderTransform.translate(_position[0], _position[1], _position[2]);
	_renderTransform.rotate(_orientation[3], _orientation[0], _orientation[1], _orientation[2]);
	_renderTransform.rotate(_rotation[0], 1.0f, 0.0f, 0.0f);
	_renderTransform.rotate(_rotation[1], 0.0f, 1.0f, 0.0f);
	_renderTransform.rotate(_rotation[2], 0.0f, 0.0f, 1.0f);
	_renderTransform.scale(_scale[0], _scale[1], _scale[2]);

	// Render the node's geometry

	bool isTransparent = mesh && mesh->isTransparent;
	bool shouldRender = doRender && renderableMesh(mesh);
	if (((pass == kRenderPassOpaque)      &&  isTransparent) ||
	    ((pass == kRenderPassTransparent) && !isTransparent))
		shouldRender = false;

	if (shouldRender)
		renderGeometry(*mesh);

	if (_attachedModel) {
		glPushMatrix();
		_attachedModel->render(pass);
		glPopMatrix();
	}

	// Render the node's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		glPushMatrix();
		(*c)->render(pass, _renderTransform);
		glPopMatrix();
	}
}

void ModelNode::drawSkeleton(const glm::mat4 &parent, bool showInvisible) {
	glm::mat4 mine = parent;

	mine = glm::translate(mine, glm::vec3(_position[0], _position[1], _position[2]));

	if (_orientation[0] != 0 || _orientation[1] != 0 || _orientation[2] != 0)
		mine = glm::rotate(mine,
				Common::deg2rad(_orientation[3]),
				glm::vec3(_orientation[0], _orientation[1], _orientation[2]));

	mine = glm::scale(mine, glm::vec3(_scale[0], _scale[1], _scale[2]));

	if (_render || showInvisible) {
		glPointSize(5.0f);

		if (_render)
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		else
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_POINTS);
			glVertex3f(mine[3][0], mine[3][1], mine[3][2]);
		glEnd();

		glLineWidth(2.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);
			glVertex3f(parent[3][0], parent[3][1], parent[3][2]);
			glVertex3f(mine[3][0], mine[3][1], mine[3][2]);
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

void ModelNode::setBufferedPosition(float x, float y, float z) {
	_positionBuffer[0] = x;
	_positionBuffer[1] = y;
	_positionBuffer[2] = z;
	_positionBuffered = true;
}

void ModelNode::setBufferedOrientation(float x, float y, float z, float angle) {
	_orientationBuffer[0] = x;
	_orientationBuffer[1] = y;
	_orientationBuffer[2] = z;
	_orientationBuffer[3] = angle;
	_orientationBuffered = true;
}

void ModelNode::flushBuffers() {
	if (_positionBuffered) {
		_position[0] = _positionBuffer[0] / _model->_scale[0];
		_position[1] = _positionBuffer[1] / _model->_scale[1];
		_position[2] = _positionBuffer[2] / _model->_scale[2];
		_positionBuffered = false;
	}

	if (_orientationBuffered) {
		_orientation[0] = _orientationBuffer[0];
		_orientation[1] = _orientationBuffer[1];
		_orientation[2] = _orientationBuffer[2];
		_orientation[3] = _orientationBuffer[3];
		_orientationBuffered = false;
	}

	if (_vertexCoordsBuffered) {
		const float *vcb = &_vertexCoordsBuffer[0];
		VertexBuffer &vb = _mesh->data->vertexBuffer;
		int vertexCount = vb.getCount();
		int stride = vb.getSize() / sizeof(float);
		float *v = reinterpret_cast<float *>(vb.getData());
		for (int i = 0; i < vertexCount; ++i) {
			v[0] = vcb[0];
			v[1] = vcb[1];
			v[2] = vcb[2];
			v += stride;
			vcb += 3;
		}
		_vertexCoordsBuffered = false;
	}
}

void ModelNode::computeInverseBindPose() {
	std::vector<ModelNode *> nodeChain;
	for (ModelNode *node = this; node; node = node->_parent) {
		nodeChain.push_back(node);
	}

	_invBindPose = glm::mat4();

	for (std::vector<ModelNode *>::reverse_iterator n = nodeChain.rbegin();
			n != nodeChain.rend();
			++n) {
		const ModelNode *node = *n;

		if (node->_positionFrames.size() > 0) {
			const PositionKeyFrame &pos = node->_positionFrames[0];
			_invBindPose = glm::translate(_invBindPose, glm::vec3(pos.x, pos.y, pos.z));
		}

		if (node->_orientationFrames.size() > 0) {
			const QuaternionKeyFrame &ori = node->_orientationFrames[0];
			if (ori.x != 0 || ori.y != 0 || ori.z != 0)
				_invBindPose = glm::rotate(_invBindPose,
						acosf(ori.q) * 2.0f,
						glm::vec3(ori.x, ori.y, ori.z));
		}
	}

	_invBindPose = glm::inverse(_invBindPose);
}

void ModelNode::computeAbsoluteTransform() {
	std::vector<ModelNode *> nodeChain;
	for (ModelNode *node = this; node; node = node->_parent) {
		nodeChain.push_back(node);
	}

	_absoluteTransform = glm::mat4();

	for (std::vector<ModelNode *>::reverse_iterator n = nodeChain.rbegin();
			n != nodeChain.rend();
			++n) {
		const ModelNode *node = *n;

		_absoluteTransform = glm::translate(_absoluteTransform,
				glm::vec3(node->_positionBuffer[0],
				          node->_positionBuffer[1],
				          node->_positionBuffer[2]));

		if (node->_orientationBuffer[0] != 0 ||
				node->_orientationBuffer[1] != 0 ||
				node->_orientationBuffer[2] != 0)
			_absoluteTransform = glm::rotate(_absoluteTransform,
					Common::deg2rad(node->_orientationBuffer[3]),
					glm::vec3(node->_orientationBuffer[0],
					          node->_orientationBuffer[1],
					          node->_orientationBuffer[2]));
	}
}

void ModelNode::buildMaterial() {
	Common::UString materialName = "xoreos";

	for (size_t t = 0; t != _textures.size(); t++) {
		materialName += ".";
		materialName += _textures[t].getName();
	}

	if (!_envMap.empty()) {
		materialName += ".";
		materialName += _envMap.getName();
	}

	_material = MaterialMan.getMaterial(materialName);
	if (!_material) {
		Shader::ShaderSampler * sampler;
		uint32 flags = 0;
		if (!_envMap.empty()) {
			if (_envMap.getTexture().getImage().isCubeMap()) {
				if (_envMapMode == kModeEnvironmentBlendedUnder) {
					flags |= Shader::ShaderBuilder::ENV_CUBE_PRE;
				} else {
					flags |= Shader::ShaderBuilder::ENV_CUBE_POST;
				}
			} else {
				if (_envMapMode == kModeEnvironmentBlendedUnder) {
					flags |= Shader::ShaderBuilder::ENV_SPHERE_PRE;
				} else {
					flags |= Shader::ShaderBuilder::ENV_SPHERE_POST;
				}
			}
		}

		if (_textures.size() > 0) {
			flags |= Shader::ShaderBuilder::TEXTURE;
		}

		_material = new Shader::ShaderMaterial(ShaderMan.getShaderObject(flags, Shader::SHADER_FRAGMENT), materialName);

		if (!_envMap.empty()) {
			if (_envMap.getTexture().getImage().isCubeMap()) {
				sampler = (Shader::ShaderSampler *)(_material->getVariableData("_textureCube0"));
			} else {
				sampler = (Shader::ShaderSampler *)(_material->getVariableData("_textureSphere0"));
			}
			sampler->texture = &(_envMap.getTexture());
			sampler->handle = _envMap;
		}

		if (_textures.size() > 0) {
			sampler = (Shader::ShaderSampler *)(_material->getVariableData("_texture0"));
			sampler->texture = &(_textures[0].getTexture());
			sampler->handle = _textures[0];
		}

		if (_isTransparent) {
			flags |= SHADER_MATERIAL_TRANSPARENT;
			flags |= Shader::ShaderMaterial::genBlendFlags(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			_material->setFlags(flags);
		}

		MaterialMan.addMaterial(_material);
		status("Added material: %s\n", materialName.c_str());
	}

	if (_shaderRenderable) {
		_shaderRenderable->setMaterial(_material);
	}
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
