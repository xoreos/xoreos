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

#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"
#include "external/glm/gtx/matrix_decompose.hpp"

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

#include "src/graphics/render/renderman.h"

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

ModelNode::MeshData::MeshData() : rawMesh(0), envMapMode(kModeEnvironmentBlendedUnder) {
}

ModelNode::Mesh::Mesh() : shininess(1.0f), alpha(1.0f), tilefade(0), render(false),
	shadow(false), beaming(false), inheritcolor(false), rotatetexture(false),
	isTransparent(false), hasTransparencyHint(false), transparencyHint(false),
	data(0), dangly(0), skin(0) {
}

ModelNode::MaterialConfiguration::MaterialConfiguration() :
		pmesh(0),
		phandles(0),
		penvmap(0),
		envmapmode(kModeEnvironmentBlendedUnder),
		textureCount(0),
		material(0),
		materialFlags(0) {
}

ModelNode::ModelNode(Model &model) :
		_model(&model),
		_parent(0),
		_attachedModel(0),
		_level(0),
		_alpha(1.0f),
		_render(false),
		_dirtyRender(true),
		_mesh(0),
		_rootStateNode(0),
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

void ModelNode::reparentTo(ModelNode *parent) {
	if (!parent || _parent == parent)
		return;

	glm::mat4 transform;

	if (_parent) {
		_parent->_children.remove(this);

		std::vector<const ModelNode *> oldToNew(getPath(_parent, parent));
		for (std::vector<const ModelNode *>::reverse_iterator n = oldToNew.rbegin(); n != oldToNew.rend(); ++n) {
			transform *= (*n)->_localTransform;
		}

		transform *= _parent->_localTransform;
	}

	transform *= _localTransform;

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);

	setBasePosition(translation);
	setBaseOrientation(rotation);

	parent->_children.push_back(this);

	_level = parent->_level + 1;
	_parent = parent;
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

uint16_t ModelNode::getNodeNumber() const {
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

glm::vec3 ModelNode::getBasePosition() const {
	if (_positionFrames.empty())
		return glm::vec3();

	const PositionKeyFrame &pos = _positionFrames[0];

	return glm::vec3(pos.x, pos.y, pos.z);
}

glm::quat ModelNode::getBaseOrientation() const {
	if (_orientationFrames.empty())
		return glm::quat();

	const QuaternionKeyFrame &ori = _orientationFrames[0];

	return glm::quat(ori.q, ori.x, ori.y, ori.z);
}

bool ModelNode::hasPositionFrames() const {
	return !_positionFrames.empty();
}

bool ModelNode::hasOrientationFrames() const {
	return !_orientationFrames.empty();
}

void ModelNode::setBasePosition(const glm::vec3 &pos) {
	_position[0] = pos.x;
	_position[1] = pos.y;
	_position[2] = pos.z;

	_positionBuffer[0] = pos.x;
	_positionBuffer[1] = pos.y;
	_positionBuffer[2] = pos.z;

	if (_positionFrames.empty()) {
		_positionFrames.push_back(PositionKeyFrame { 0.0f, pos.x, pos.y, pos.z });
		return;
	}

	PositionKeyFrame &frame = _positionFrames[0];
	frame.x = pos.x;
	frame.y = pos.y;
	frame.z = pos.z;
}

void ModelNode::setBaseOrientation(const glm::quat &ori) {
	_orientation[0] = ori.x;
	_orientation[1] = ori.y;
	_orientation[2] = ori.z;
	_orientation[3] = Common::rad2deg(acosf(ori.w) * 2.0f);

	std::memcpy(_orientationBuffer, _orientation, 4 * sizeof(float));

	if (_orientationFrames.empty()) {
		_orientationFrames.push_back(QuaternionKeyFrame { 0.0f, ori.x, ori.y, ori.z, ori.w });
		return;
	}

	QuaternionKeyFrame &frame = _orientationFrames[0];
	frame.x = ori.x;
	frame.y = ori.y;
	frame.z = ori.z;
	frame.q = ori.w;
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
	_dirtyRender = true;
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

void ModelNode::setMaterial(Shader::ShaderMaterial *material) {
	_material = material;
	if (_shaderRenderable) {
		_shaderRenderable->setMaterial(_material);
	}
}

float ModelNode::getAlpha() {
	if (!_mesh) {
		return _alpha;
	} else {
		return _mesh->alpha;
	}
}

void ModelNode::setAlpha(float alpha, bool isRecursive) {
	if (!_mesh) {
		_alpha = alpha;
	} else {
		_mesh->alpha = alpha;
	}

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

	_dirtyRender = true;
	// If the node has no actual texture, we just assume
	// that the geometry shouldn't be rendered.
	if (!hasTexture)
		_render = false;
}

void ModelNode::createBound() {
	_boundBox.clear();

	if (!_mesh || !_mesh->data)
		return;

	VertexBuffer *vertexBuffer = _mesh->data->rawMesh->getVertexBuffer();

	const VertexDecl &vertexDecl = vertexBuffer->getVertexDecl();
	for (VertexDecl::const_iterator vA = vertexDecl.begin(); vA != vertexDecl.end(); ++vA) {
		if (vA->pointer) {
			if ((vA->index != VPOSITION) || (vA->type != GL_FLOAT))
				continue;

			const uint32_t stride = MAX<uint32_t>(vA->size, vA->stride / sizeof(float));

			const float *vertexData = reinterpret_cast<const float *>(vA->pointer);

			const float *vX = vertexData + 0;
			const float *vY = vertexData + 1;
			const float *vZ = vertexData + 2;

			for (uint32_t v = 0; v < vertexBuffer->getCount(); v++)
				_boundBox.add(vX[v * stride], vY[v * stride], vZ[v * stride]);
		}
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


		/**
		 * @todo So this is a great big dirty hack to keep relative positioning
		 * in place for rendering purposes, while still allowing bounding volumes
		 * to be properly created.
		 * This should be replaced by a _globalTransform / _localTransform system
		 * one day.
		 */
		glm::mat4 apos = _attachedModel->_absolutePosition;  // Hack part A.
		_attachedModel->_absolutePosition = modelPosition;
		_attachedModel->createBound();

		_absoluteBoundBox.add(_attachedModel->_absoluteBoundBox);
		_attachedModel->_absolutePosition = apos;  // Hack part B.
	}
}

void ModelNode::orderChildren() {
	_children.sort(nodeComp);

	// Order the children's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c)
		(*c)->orderChildren();
}

void ModelNode::renderGeometry(ModelNode::Mesh &mesh) {
	TextureHandle *penvmap = 0;
	EnvironmentMapMode envmapmode;
	penvmap = getEnvironmentMap(envmapmode);

	if (penvmap) {
		if (envmapmode == kModeEnvironmentBlendedUnder) {
			renderGeometryEnvMappedUnder(mesh);
		}
	}

	renderGeometryNormal(mesh);

	if (penvmap) {
		if (envmapmode == kModeEnvironmentBlendedOver) {
			renderGeometryEnvMappedOver(mesh);
		}
	}
}

void ModelNode::renderGeometryNormal(Mesh &mesh) {
	bool specialBlending = false;

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set(mesh.data->textures[t]);

		if (!mesh.data->textures[t].empty()) {
			const Graphics::Aurora::Texture &texture = mesh.data->textures[t].getTexture();
			if (!texture.hasAlpha() && (texture.getTXI().getFeatures().blending == TXI::kBlendingAdditive))
				specialBlending = true;
		}
	}

	if (mesh.data->textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (specialBlending)
		glBlendFunc(GL_SRC_COLOR, GL_ONE);

	mesh.data->rawMesh->renderImmediate();

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set();
	}

	if (mesh.data->textures.empty())
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ModelNode::renderGeometryEnvMappedUnder(Mesh &mesh) {
	/* First draw the node with only the environment map, then simply
	 * blend a semi-transparent diffuse texture on top.
	 *
	 * Neverwinter Nights uses this method.
	 */

	mesh.data->rawMesh->renderBind();

	TextureMan.set(mesh.data->envMap, TextureManager::kModeEnvironmentMapReflective);
	mesh.data->rawMesh->render();

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set(mesh.data->textures[t], TextureManager::kModeDiffuse);
	}

	mesh.data->rawMesh->render();

	for (size_t t = 0; t < mesh.data->textures.size(); t++) {
		TextureMan.activeTexture(t);
		TextureMan.set();
	}

	mesh.data->rawMesh->renderUnbind();
}

void ModelNode::renderGeometryEnvMappedOver(Mesh &mesh) {
	/* First draw the node with diffuse textures, then draw it again with
	 * only the environment map. This performs a more complex blending of
	 * the textures, allowing the color of a transparent diffuse texture
	 * to modulate the color of the environment map.
	 *
	 * KotOR and KotOR2 use this method.
	 */

	mesh.data->rawMesh->renderBind();

	if (!mesh.data->textures.empty()) {
		for (size_t t = 0; t < mesh.data->textures.size(); t++) {
			TextureMan.activeTexture(t);
			TextureMan.set(mesh.data->textures[t], TextureManager::kModeDiffuse);
		}

		glBlendFunc(GL_ONE, GL_ZERO);

		mesh.data->rawMesh->render();

		for (size_t t = 0; t < mesh.data->textures.size(); t++) {
			TextureMan.activeTexture(t);
			TextureMan.set();
		}

		TextureMan.activeTexture(0);
		TextureMan.set(mesh.data->textures[0], TextureManager::kModeDiffuse);

		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_ZERO, GL_ONE);

		mesh.data->rawMesh->render();
	}

	TextureMan.activeTexture(0);
	TextureMan.set(mesh.data->envMap, TextureManager::kModeEnvironmentMapReflective);

	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

	mesh.data->rawMesh->render();

	TextureMan.set();

	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mesh.data->rawMesh->renderUnbind();
}

bool ModelNode::renderableMesh(Mesh *mesh) {
	return mesh && mesh->data && mesh->data->rawMesh;
}

void ModelNode::render(RenderPass pass) {
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
		(*c)->render(pass);
		glPopMatrix();
	}
}

void ModelNode::calcRenderTransform(const glm::mat4 &parentTransform) {
	// Apply the node's transformation
	_renderTransform = parentTransform;
	_renderTransform = glm::translate(_renderTransform, glm::vec3(_position[0], _position[1], _position[2]));
	if (_orientation[0] != 0.0f ||
	    _orientation[1] != 0.0f ||
	    _orientation[2] != 0.0f) {
		_renderTransform = glm::rotate(_renderTransform,
		                               Common::deg2rad(_orientation[3]),
		                               glm::vec3(_orientation[0], _orientation[1], _orientation[2]));
	}
	_renderTransform = glm::rotate(_renderTransform, Common::deg2rad(_rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
	_renderTransform = glm::rotate(_renderTransform, Common::deg2rad(_rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	_renderTransform = glm::rotate(_renderTransform, Common::deg2rad(_rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
	_renderTransform = glm::scale(_renderTransform, glm::vec3(_scale[0], _scale[1], _scale[2]));
}

void ModelNode::renderImmediate(const glm::mat4 &parentTransform) {
	calcRenderTransform(parentTransform);
	/**
	 * Ignoring _render for now because it's being falsely set to false.
	 */
	/* if (_render) {} */

	if (_dirtyRender) {
		/**
		 * Do this regardless of if the modelnode is actually visible or not, to prevent
		 * stutter when things are first brought into view. Most things are loaded at the
		 * start of a level, so stutter won't be noticed then.
		 */
		buildMaterial();
	} else {
		/**
		 * @todo Ideally there should be some kind of check in here to determine visibility.
		 * if the node isn't (camera) visible, then don't bother trying to render it.
		 */
		if (_renderableArray.size() == 0) {
			if (_rootStateNode) {
				for (size_t i = 0; i < _rootStateNode->_renderableArray.size(); ++i) {
					_rootStateNode->_renderableArray[i].renderImmediate(_renderTransform, this->getAlpha());
				}
			}
		} else {
			for (size_t i = 0; i < _renderableArray.size(); ++i) {
				_renderableArray[i].renderImmediate(_renderTransform, this->getAlpha());
			}
		}
	}

	if (_attachedModel) {
		_attachedModel->renderImmediate(_renderTransform);
	}
	// Render the node's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		(*c)->renderImmediate(_renderTransform);
	}
}

void ModelNode::queueRender(const glm::mat4 &parentTransform) {
	calcRenderTransform(parentTransform);
	/**
	 * Ignoring _render for now because it's being falsely set to false.
	 */
	/* if (_render) {} */

	if (_dirtyRender) {
		/**
		 * Do this regardless of if the modelnode is actually visible or not, to prevent
		 * stutter when things are first brought into view. Most things are loaded at the
		 * start of a level, so stutter won't be noticed then.
		 */
		buildMaterial();
	} else {
		/**
		 * @todo Ideally there should be some kind of check in here to determine visibility.
		 * if the node isn't (camera) visible, then don't bother trying to render it.
		 */
		if (_renderableArray.size() == 0) {
			if (_rootStateNode) {
				for (size_t i = 0; i < _rootStateNode->_renderableArray.size(); ++i) {
					RenderMan.queueRenderable(&(_rootStateNode->_renderableArray[i]), &_renderTransform, this->getAlpha());
				}
			}
		} else {
			for (size_t i = 0; i < _renderableArray.size(); ++i) {
				RenderMan.queueRenderable(&_renderableArray[i], &_renderTransform, this->getAlpha());
			}
		}
	}

	if (_attachedModel) {
		_attachedModel->queueRender(_renderTransform);
	}
	// Render the node's children
	for (std::list<ModelNode *>::iterator c = _children.begin(); c != _children.end(); ++c) {
		(*c)->queueRender(_renderTransform);
	}
}

void ModelNode::drawSkeleton(const glm::mat4 &parent, bool showInvisible) {
	glm::mat4 mine = parent;

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
		_mesh->data->rawMesh->getVertexBuffer()->updateGL();
		_vertexCoordsBuffered = false;
	}
}

int ModelNode::getBoneCount() const {
	return _mesh->skin->boneMappingCount;
}

int ModelNode::getBoneIndexByNodeNumber(int nodeNumber) const {
	return static_cast<int>(_mesh->skin->boneMapping[nodeNumber]);
}

ModelNode *ModelNode::getBoneNode(int index) {
	return _mesh->skin->boneNodeMap[index];
}

const std::vector<float> &ModelNode::getInitialVertexCoords() const {
	return _mesh->data->initialVertexCoords;
}

const std::vector<float> &ModelNode::getBoneIndices() const {
	return _mesh->skin->boneMappingId;
}

const std::vector<float> &ModelNode::getBoneWeights() const {
	return _mesh->skin->boneWeights;
}

bool ModelNode::hasSkinNode() const {
	return _mesh && _mesh->skin;
}

void ModelNode::computeTransforms() {
	computeLocalBaseTransform();
	computeLocalTransform();

	if (_parent) {
		_absoluteBaseTransform = _parent->_absoluteBaseTransform * _localBaseTransform;
		_absoluteTransform = _parent->_absoluteTransform * _localTransform;
	} else {
		_absoluteBaseTransform = _localBaseTransform;
		_absoluteTransform = _localTransform;
	}

	_boneTransform = _absoluteTransform * _absoluteBaseTransformInv;
	_absoluteBaseTransformInv = glm::inverse(_absoluteBaseTransform);
	_absoluteTransformInv = glm::inverse(_absoluteTransform);

	for (const auto &n : _children) {
		n->computeTransforms();
	}
}

void ModelNode::computeLocalBaseTransform() {
	_localBaseTransform = glm::mat4();

	if (_positionFrames.size() > 0) {
		const PositionKeyFrame &pos = _positionFrames[0];
		_localBaseTransform = glm::translate(_localBaseTransform, glm::vec3(pos.x, pos.y, pos.z));
	}

	if (_orientationFrames.size() > 0) {
		const QuaternionKeyFrame &ori = _orientationFrames[0];
		if ((ori.x != 0.0f) || (ori.y != 0.0f) || (ori.z != 0.0f)) {
			_localBaseTransform = glm::rotate(
					_localBaseTransform,
					acosf(ori.q) * 2.0f,
					glm::vec3(ori.x, ori.y, ori.z));
		}
	}

	_localBaseTransformInv = glm::inverse(_localBaseTransform);
}

void ModelNode::computeLocalTransform() {
	_localTransform = glm::mat4();

	_localTransform = glm::translate(
			_localTransform,
			glm::vec3(_positionBuffer[0], _positionBuffer[1], _positionBuffer[2]));

	if     ((_orientationBuffer[0] != 0.0f) ||
			(_orientationBuffer[1] != 0.0f) ||
			(_orientationBuffer[2] != 0.0f)) {

		_localTransform = glm::rotate(
				_localTransform,
				Common::deg2rad(_orientationBuffer[3]),
				glm::vec3(_orientationBuffer[0], _orientationBuffer[1], _orientationBuffer[2]));
	}

	_localTransformInv = glm::inverse(_localTransform);
}

std::vector<const ModelNode *> ModelNode::getPath(const ModelNode *from, const ModelNode *to) const {
	if (!from || !to)
		return std::vector<const ModelNode *>();

	std::vector<const ModelNode *> path;
	for (const ModelNode *node = from->_parent; node && (node != to); node = node->_parent) {
		if (node->_level == 0)
			return std::vector<const ModelNode *>();

		path.push_back(node);
	}
	return path;
}

void ModelNode::notifyVertexCoordsBuffered() {
	_vertexCoordsBuffered = true;
}

ModelNode::Mesh *ModelNode::getMesh() const {
	if (_mesh) {
		return _mesh;
	}

	if (_model->getState().empty()) {
		return NULL; // Stateless and no internal _mesh.
	}

	ModelNode *rootStateNode = _model->getNode("", _name);
	if (rootStateNode && rootStateNode != this) {
		return rootStateNode->_mesh;
	}

	return NULL; // No mesh found in the root state.
}

TextureHandle *ModelNode::getTextures(uint32_t &count) {
	TextureHandle *rval = NULL;
	count = 0;
	if (_mesh && _mesh->data) {
		count = _mesh->data->textures.size();
		if (count) {
			rval = &(_mesh->data->textures[0]);
		}
	}

	if (!rval) {
		// Nothing here, see if the parent has something for us.
		ModelNode *rootStateNode = _model->getNode("", _name);
		if (rootStateNode && rootStateNode != this) {
			rval = rootStateNode->getTextures(count);
		}
	}

	return rval;
}

TextureHandle *ModelNode::getEnvironmentMap(EnvironmentMapMode &mode) {
	TextureHandle *rval = NULL;
	if (_mesh && _mesh->data) {
		if (!_mesh->data->envMap.empty()) {
			rval = &(_mesh->data->envMap);
			mode = _mesh->data->envMapMode;
		}
	}

	if (!rval) {
		// Nothing here, see if the parent has something for us.
		ModelNode *rootStateNode = _model->getNode("", _name);
		if (rootStateNode && rootStateNode != this) {
			rval = rootStateNode->getEnvironmentMap(mode);
		}
	}

	return rval;
}

void ModelNode::buildMaterial() {
	_renderableArray.clear();

	/**
	 * If there's no override of mesh, textures, or environment mapping, then don't bother
	 * to create any new renderables. Just make sure _rootStateNode has some, and have the
	 * render queuing use the renderables from there instead. This isn't really a problem,
	 * as the per-modelnode data (modelview matrix in this case) is still supplied from
	 * _this_ object.
	 */

	if (!_model->getState().empty()) {
		_rootStateNode = _model->getNode("", _name);
		if (_rootStateNode == this) {
			_rootStateNode = 0;
		}
	} else {
		_rootStateNode = 0;
	}

	_dirtyRender = false;

	if (!_mesh || !_mesh->data ||
			((_mesh->data->textures.size() == 0) && _mesh->data->envMap.empty() && !_mesh->data->rawMesh))
		return;

	MaterialConfiguration config;
	config.pmesh = _mesh;
	config.phandles = getTextures(config.textureCount);
	config.penvmap = getEnvironmentMap(config.envmapmode);

	if ((config.textureCount == 0) || !_render || !config.pmesh->data->rawMesh || config.phandles[0].empty())
		return;

	config.materialName = "xoreos.";
	Shader::ShaderDescriptor cripter;
	_renderableArray.clear();
	declareShaderInputs(config, cripter);

	if (_name == "Plane237")
		config.pmesh->isTransparent = true;  // Hack hack hack hack. For NWN.

	if (config.penvmap) {
		setupEnvMapSampler(config, cripter);
		if (config.envmapmode == kModeEnvironmentBlendedUnder)
			addBlendedUnderEnvMapPass(config, cripter);
	}

	if (config.pmesh->isTransparent &&
			!(config.materialFlags & Shader::ShaderMaterial::MATERIAL_OPAQUE)) {
		config.materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT;
	}

	for (uint32_t i = 0; (i < 4) && (i < config.textureCount); ++i)
		setupShaderTexture(config, i, cripter);

	if (config.penvmap && (config.envmapmode == kModeEnvironmentBlendedOver))
		addBlendedOverEnvMapPass(config, cripter);

	if (config.materialFlags & Shader::ShaderMaterial::MATERIAL_OPAQUE) {
		cripter.addPass(Shader::ShaderDescriptor::FORCE_OPAQUE,
		                Shader::ShaderDescriptor::BLEND_IGNORED);
	}

	if ((config.materialFlags & Shader::ShaderMaterial::MATERIAL_TRANSPARENT) &&
			(config.pmesh->data->rawMesh->getVertexBuffer()->getCount() <= 6)) {
		config.materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT_B;
	}

	Shader::ShaderSurface *surface;

	config.material = MaterialMan.getMaterial(config.materialName);
	if (config.material) {
		surface = SurfaceMan.getSurface(config.materialName);
		_renderableArray.push_back(Shader::ShaderRenderable(surface, config.material, _mesh->data->rawMesh));
		return;
	}

	if (_mesh->alpha < 1.0f) {
		config.materialFlags &= ~Shader::ShaderMaterial::MATERIAL_OPAQUE;  // Make sure it's not actually opaque.
		config.materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT;
	}

	Common::UString vertexShaderName;
	Common::UString fragmentShaderName;
	cripter.genName(vertexShaderName);
	fragmentShaderName = vertexShaderName + ".frag";
	vertexShaderName += ".vert";

	// Ok, material doesn't exist. Check on the shaders.
	Shader::ShaderObject *vertexObject = ShaderMan.getShaderObject(vertexShaderName, Shader::SHADER_VERTEX);
	Shader::ShaderObject *fragmentObject = ShaderMan.getShaderObject(fragmentShaderName, Shader::SHADER_FRAGMENT);

	// Should be checking vert and frag shader separately, but they really should exist together anyway.
	if (!vertexObject) {
		// No object found. Generate a shader then.
		bool isGL3 = GfxMan.isGL3();

		Common::UString vertexStringFinal;
		Common::UString fragmentStringFinal;

		cripter.build(isGL3, vertexStringFinal, fragmentStringFinal);
		vertexObject = ShaderMan.getShaderObject(vertexShaderName, vertexStringFinal, Shader::SHADER_VERTEX);
		fragmentObject = ShaderMan.getShaderObject(fragmentShaderName, fragmentStringFinal, Shader::SHADER_FRAGMENT);
	}

	// Shader objects should now exist, so go ahead and make the material and surface.
	surface = new Shader::ShaderSurface(vertexObject, config.materialName);
	config.material = new Shader::ShaderMaterial(fragmentObject, config.materialName);
	config.material->setFlags(config.materialFlags);
	if (config.materialFlags & Shader::ShaderMaterial::MATERIAL_CUSTOM_BLEND) {
		config.material->setBlendSrcRGB(GL_ZERO);
		config.material->setBlendSrcAlpha(GL_ZERO);
		config.material->setBlendDstRGB(GL_ONE_MINUS_SRC_COLOR);
		config.material->setBlendDstAlpha(GL_ONE_MINUS_SRC_ALPHA);
	}
	MaterialMan.addMaterial(config.material);
	SurfaceMan.addSurface(surface);

	bindTexturesToSamplers(config, cripter);

	_renderableArray.push_back(Shader::ShaderRenderable(surface, config.material, _mesh->data->rawMesh));
}

void ModelNode::declareShaderInputs(MaterialConfiguration &UNUSED(config), Shader::ShaderDescriptor &cripter) {
	cripter.declareInput(Shader::ShaderDescriptor::INPUT_POSITION0);
	cripter.declareInput(Shader::ShaderDescriptor::INPUT_NORMAL0);
	cripter.declareInput(Shader::ShaderDescriptor::INPUT_UV0);
}

void ModelNode::setupEnvMapSampler(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter) {
	if (config.penvmap->getTexture().getImage().isCubeMap()) {
		cripter.declareInput(Shader::ShaderDescriptor::INPUT_UV_CUBE);

		cripter.declareSampler(Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
		                      Shader::ShaderDescriptor::SAMPLER_CUBE);

		cripter.connect(Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
		                Shader::ShaderDescriptor::INPUT_UV_CUBE,
		                Shader::ShaderDescriptor::ENV_CUBE);

	} else {
		cripter.declareInput(Shader::ShaderDescriptor::INPUT_UV_SPHERE);

		cripter.declareSampler(Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
		                       Shader::ShaderDescriptor::SAMPLER_2D);

		cripter.connect(Shader::ShaderDescriptor::SAMPLER_TEXTURE_7,
		                Shader::ShaderDescriptor::INPUT_UV_SPHERE,
		                Shader::ShaderDescriptor::ENV_SPHERE);
	}
}

void ModelNode::addBlendedUnderEnvMapPass(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter) {
	config.materialName += config.penvmap->getName();

	// Figure out if a cube or sphere map is used.
	if (config.penvmap->getTexture().getImage().isCubeMap()) {
		if (!config.pmesh->isTransparent)
			config.materialFlags |= Shader::ShaderMaterial::MATERIAL_OPAQUE;

		cripter.addPass(Shader::ShaderDescriptor::ENV_CUBE,
		                Shader::ShaderDescriptor::BLEND_ONE);

	} else {
		/**
		 * Seems that, regardless of _isTransparent, anything with shperical env mapping is opaque. This mostly comes from
		 * NWN, where it's seen that things marked as transparent actually shouldn't be. It's assumed this carries over to
		 * other game titles as well.
		 */
		config.materialFlags |= Shader::ShaderMaterial::MATERIAL_OPAQUE;

		cripter.addPass(Shader::ShaderDescriptor::ENV_SPHERE,
		                Shader::ShaderDescriptor::BLEND_ONE);
	}
}

void ModelNode::setupShaderTexture(MaterialConfiguration &config, int textureIndex, Shader::ShaderDescriptor &cripter) {
	switch (textureIndex) {
	case 0:
		if (config.phandles[0].empty())
			break;

		config.materialName += config.phandles[0].getName();

		cripter.declareSampler(Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
		                       Shader::ShaderDescriptor::SAMPLER_2D);

		cripter.connect(Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
		                Shader::ShaderDescriptor::INPUT_UV0,
		                Shader::ShaderDescriptor::TEXTURE_DIFFUSE);

		if (config.phandles[0].getTexture().getTXI().getFeatures().blending) {
			config.materialFlags |= Shader::ShaderMaterial::MATERIAL_CUSTOM_BLEND;
			// For KotOR2, this is required to get some windows showing up properly.
			if (config.pmesh->hasTransparencyHint &&
					!(config.materialFlags & Shader::ShaderMaterial::MATERIAL_OPAQUE)) {
				config.materialFlags |= Shader::ShaderMaterial::MATERIAL_TRANSPARENT;
			}
		}

		// Check to see if it's actually a decal texture.
		if (config.phandles[0].getTexture().getTXI().getFeatures().decal)
			config.materialFlags |= Shader::ShaderMaterial::MATERIAL_DECAL;

		if (config.penvmap && (config.envmapmode == kModeEnvironmentBlendedUnder)) {
			cripter.addPass(Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
			                Shader::ShaderDescriptor::BLEND_SRC_ALPHA);
		} else {
			cripter.addPass(Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
			                Shader::ShaderDescriptor::BLEND_ONE);
		}

		break;

	case 1:
		if (config.phandles[1].empty())
			break;

		config.materialName += "." + config.phandles[1].getName();

		cripter.declareSampler(Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
		                       Shader::ShaderDescriptor::SAMPLER_2D);

		cripter.connect(Shader::ShaderDescriptor::SAMPLER_TEXTURE_1,
		                Shader::ShaderDescriptor::INPUT_UV0,
		                Shader::ShaderDescriptor::TEXTURE_LIGHTMAP);

		cripter.addPass(Shader::ShaderDescriptor::TEXTURE_LIGHTMAP,
		                Shader::ShaderDescriptor::BLEND_MULTIPLY);

		break;

	case 2:
		if (!config.phandles[2].empty()) {
			config.materialName += ".";
			config.materialName += config.phandles[2].getName();

			cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_2,
			                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);

		} else {
			cripter.addPass(Shader::ShaderDescriptor::FORCE_OPAQUE,
			                Shader::ShaderDescriptor::BLEND_IGNORED);
		}
		break;

	case 3:
		cripter.addPass(Shader::ShaderDescriptor::FORCE_OPAQUE,
		                Shader::ShaderDescriptor::BLEND_IGNORED);

		break;
	}
}

void ModelNode::addBlendedOverEnvMapPass(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter) {
	config.materialName += config.penvmap->getName();

	// Figure out if a cube or sphere map is used.
	if (config.penvmap->getTexture().getImage().isCubeMap()) {
		cripter.addPass(Shader::ShaderDescriptor::ENV_CUBE,
		                Shader::ShaderDescriptor::BLEND_DST_ALPHA);

	} else {
		cripter.addPass(Shader::ShaderDescriptor::ENV_SPHERE,
		                Shader::ShaderDescriptor::BLEND_DST_ALPHA);
	}
}

void ModelNode::bindTexturesToSamplers(MaterialConfiguration &config, Shader::ShaderDescriptor &UNUSED(cripter)) {
	Shader::ShaderSampler *sampler;

	if (config.penvmap) {
		sampler = (Shader::ShaderSampler *)(config.material->getVariableData("sampler_7_id"));
		sampler->handle = *config.penvmap;
	}

	if ((config.textureCount > 0) && !config.phandles[0].empty()) {
		sampler = (Shader::ShaderSampler *)(config.material->getVariableData("sampler_0_id"));
		sampler->handle = config.phandles[0];
	}

	if ((config.textureCount > 1) && !config.phandles[1].empty()) {
		sampler = (Shader::ShaderSampler *)(config.material->getVariableData("sampler_1_id"));
		sampler->handle = config.phandles[1];
	}

	if ((config.textureCount > 2) && !config.phandles[2].empty()) {
		sampler = (Shader::ShaderSampler *)(config.material->getVariableData("sampler_2_id"));
		sampler->handle = config.phandles[2];
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
