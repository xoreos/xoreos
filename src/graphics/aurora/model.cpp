/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model.cpp
 *  A 3D model of an object.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/transmatrix.h"

#include "events/events.h"

#include "graphics/graphics.h"
#include "graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

Model::Mesh::Mesh() : faceCount(0) {
}


Model::Node::Node() : parent(0), dangly(false), displacement(0), render(true), list(0) {
	position   [0] = 0.0;
	position   [1] = 0.0;
	position   [2] = 0.0;
	orientation[0] = 0.0;
	orientation[1] = 0.0;
	orientation[2] = 0.0;
	orientation[3] = 0.0;

	realPosition[0] = 0.0;
	realPosition[1] = 0.0;
	realPosition[2] = 0.0;
}


Model::Model(ModelType type) : Renderable(GfxMan.getRenderableQueue((Graphics::RenderableQueue) type)),
	_type(type), _superModel(0), _class(kClassOther), _scale(1.0), _fade(false), _fadeStart(0),
	_fadeValue(1.0), _fadeStep(0.0), _currentState(0), _textureCount(0), _list(0) {

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
	_bearing    [0] = 0.0;
	_bearing    [1] = 0.0;
	_bearing    [2] = 0.0;
}

Model::~Model() {
	Renderable::removeFromQueue();

	destroy();

	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node)
		delete *node;

	for (StateMap::iterator state = _states.begin(); state != _states.end(); ++state)
		delete state->second;
}

void Model::createStateNameList() {
	_stateNames.clear();

	for (StateMap::const_iterator state = _states.begin(); state != _states.end(); ++state)
		_stateNames.push_back(state->first);
}

void Model::readArray(Common::SeekableReadStream &stream, uint32 &start, uint32 &count) {
	start = stream.readUint32LE();

	uint32 usedCount      = stream.readUint32LE();
	uint32 allocatedCount = stream.readUint32LE();

	if (usedCount != allocatedCount)
		warning("Model::readArray(): usedCount != allocatedCount (%d, %d)", usedCount, allocatedCount);

	count = usedCount;
}

void Model::readArrayOffsets(Common::SeekableReadStream &stream, uint32 start, uint32 count,
		std::vector<uint32> &offsets) {

	uint32 pos = stream.seekTo(start);

	offsets.reserve(count);
	while (count-- > 0)
		offsets.push_back(stream.readUint32LE());

	stream.seekTo(pos);
}

void Model::readArrayFloats(Common::SeekableReadStream &stream, uint32 start, uint32 count,
		std::vector<float> &floats) {

	uint32 pos = stream.seekTo(start);

	floats.reserve(count);
	while (count-- > 0)
		floats.push_back(stream.readIEEEFloatLE());

	stream.seekTo(pos);
}

void Model::processMesh(const Mesh &mesh, Node &node) {
	_textureCount = MAX<uint32>(_textureCount, mesh.textures.size());

	node.textures.resize(mesh.textures.size());

	// Try to load the textures
	bool hasTexture = false;
	for (uint t = 0; t < mesh.textures.size(); t++) {
		try {
			if (!mesh.textures[t].empty() && (mesh.textures[t] != "NULL")) {
				node.textures[t] = TextureMan.get(mesh.textures[t]);
				hasTexture = true;
			}
		} catch (...) {
			warning("Failed loading texture \"%s\"", mesh.textures[t].c_str());
			node.textures[t].clear();
		}
	}

	if (!hasTexture) {
		// The node has no actual texture, so we just assume that
		// the geometry shouldn't be rendered. Cleaning up.

		node.textures.clear();
		node.render = false;
		return;
	}

	node.faces.resize(mesh.faceCount);

	// Go over each face and assign the actual coordinates
	for (uint32 i = 0; i < mesh.faceCount; i++) {
		Face &face = node.faces[i];

		face.verts.resize(9);

		// Real face coordinates
		for (int v = 0; v < 3; v++) {
			for (int c = 0; c < 3; c++)
				face.verts[v * 3 + c] = mesh.verts[3 * mesh.vertIndices[3 * i + v] + c];

			if (node.render)
				node.boundBox.add(face.verts[v * 3 + 0], face.verts[v * 3 + 1], face.verts[v * 3 + 2]);
		}

		face.tverts.resize(mesh.textures.size() * 9);

		// Real texture coordinates
		for (uint t = 0; t < mesh.textures.size(); t++) {
			uint32 vC  = mesh.verts.size();
			uint32 viC = mesh.vertIndices.size();

			if (((t * viC) >= mesh.tvertIndices.size()) || ((t * vC) >= mesh.tverts.size())) {
				for (int v = 0; v < 3; v++)
					for (int c = 0; c < 3; c++)
						face.tverts[t * 9 + v * 3 + c] = 0.0;
			} else {
				for (int v = 0; v < 3; v++)
					for (int c = 0; c < 3; c++)
						face.tverts[t * 9 + v * 3 + c] =
							mesh.tverts[t * vC + 3 * mesh.tvertIndices[t * viC + 3 * i + v] + c];
			}
		}

		if (i < mesh.smoothGroup.size())
			face.smoothGroup = mesh.smoothGroup[i];
		if (i < mesh.material.size())
		face.material = mesh.material[i];
	}

}

void Model::createModelBound() {
	if (!_currentState)
		return;

	_boundBox.clear();

	Common::TransformationMatrix matrix;
	for (NodeList::const_iterator node = _currentState->nodes.begin(); node != _currentState->nodes.end(); ++node) {
		matrix.loadIdentity();

		recalculateNodeBound(**node, matrix);

		_boundBox.add((*node)->realBoundBox);
	}
}

void Model::recalculateNodeBound(Node &node, Common::TransformationMatrix &matrix) {
	matrix.translate(node.position[0], node.position[1], node.position[2]);
	matrix.rotate(node.orientation[3], node.orientation[0], node.orientation[1], node.orientation[2]);

	node.realPosition[0] = matrix.getX();
	node.realPosition[1] = matrix.getY();
	node.realPosition[2] = matrix.getZ();


	node.realBoundBox = node.boundBox;
	node.realBoundBox.translate(node.position[0], node.position[1], node.position[2]);
	node.realBoundBox.rotate(node.orientation[3], node.orientation[0], node.orientation[1], node.orientation[2]);


	for (NodeList::const_iterator child = node.children.begin(); child != node.children.end(); ++child) {
		Common::TransformationMatrix nodeMatrix = matrix;

		recalculateNodeBound(**child, nodeMatrix);

		Common::BoundingBox childBound = (*child)->realBoundBox.getAbsolute();
		node.realBoundBox.add(childBound);
	}
}

void Model::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Model::setOrientation(float x, float y, float z) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
}

void Model::setBearing(float x, float y, float z) {
	_bearing[0] = x;
	_bearing[1] = y;
	_bearing[2] = z;
}

bool Model::hasNode(const Common::UString &node) const {
	if (!_currentState)
		return false;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if ((n == _currentState->nodeMap.end()) || !n->second)
		return false;

	return true;
}

bool Model::getNodePosition(const Common::UString &node, float &x, float &y, float &z) const {
	if (!_currentState)
		return false;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if ((n == _currentState->nodeMap.end()) || !n->second)
		return false;

	x = n->second->realPosition[0];
	y = n->second->realPosition[1];
	z = n->second->realPosition[2];

	return true;
}

void Model::moveNode(const Common::UString &node, float x, float y, float z) {
	if (!_currentState)
		return;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if ((n == _currentState->nodeMap.end()) || !n->second)
		return;

	n->second->position[0] += x;
	n->second->position[1] += y;
	n->second->position[2] += x;

	createModelBound();
}

float Model::getNodeWidth(const Common::UString &node) const {
	if (!_currentState)
		return 0.0;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if ((n == _currentState->nodeMap.end()) || !n->second)
		return 0.0;

	return n->second->boundBox.getWidth();
}

float Model::getNodeHeight(const Common::UString &node) const {
	if (!_currentState)
		return 0.0;

	NodeMap::const_iterator n = _currentState->nodeMap.find(node);
	if ((n == _currentState->nodeMap.end()) || !n->second)
		return 0.0;

	return n->second->boundBox.getHeight();
}

float Model::getWidth() const {
	return _boundBox.getWidth();
}

float Model::getHeight() const {
	return _boundBox.getHeight();
}

float Model::getDepth() const {
	return _boundBox.getDepth();
}

void Model::transformBoundBox(Common::TransformationMatrix &world,
		Common::BoundingBox &object) const {

	// Apply position translation
	world.translate(-_position[0], -_position[1], -_position[2]);

	if (_type == kModelTypeObject)
		// Aurora world objects have a rotated axis
		world.rotate(-90.0, -1.0, 0.0, 0.0);

	// Apply rotation around the world center
	world.rotate(-_orientation[2], 0.0, 0.0, 1.0);
	world.rotate(-_orientation[1], 0.0, 1.0, 0.0);
	world.rotate(-_orientation[0], 1.0, 0.0, 0.0);

	if (_type == kModelTypeGUIFront)
		// Aurora GUI objects use 0.01 units / pixel
		world.scale(0.01, 0.01, 0.01);

	object = _boundBox;

	// Apply rotation around the object's center
	object.rotate(_bearing[0], 1.0, 0.0, 0.0);
	object.rotate(_bearing[1], 0.0, 0.0, 1.0);
	object.rotate(_bearing[2], 0.0, 1.0, 0.0);
}

bool Model::isIn(float x, float y) const {
	Common::TransformationMatrix world;
	Common::BoundingBox object;

	transformBoundBox(world, object);

	world.translate(x, y, 0.0);

	return object.isIn(world.getX(), world.getY());
}

bool Model::isIn(float x, float y, float z) const {
	Common::TransformationMatrix world;
	Common::BoundingBox object;

	transformBoundBox(world, object);

	world.translate(x, y, z);

	return object.isIn(world.getX(), world.getY(), world.getZ());
}

const std::list<Common::UString> &Model::getStates() const {
	return _stateNames;
}

void Model::setState(const Common::UString &name) {
	if (_states.empty())
		return;

	StateMap::iterator state = _states.find(name);
	if (state == _states.end())
		// No such state, get the default one
		state = _states.find("");

	if (state == _states.end()) {
		// No states at all

		_currentState = 0;
		return;
	}

	_currentState = state->second;

	createModelBound();
}

void Model::show() {
	Renderable::addToQueue();
}

void Model::hide() {
	Renderable::removeFromQueue();
}

bool Model::isVisible() {
	if (_fadeValue <= 0.0)
		// Model faded out, don't render it anymore
		Renderable::removeFromQueue();

	return Renderable::isInQueue();
}

void Model::fadeIn(uint32 length) {
	_fade      = true;
	_fadeStart = EventMan.getTimestamp();
	_fadeValue = 0.0;
	_fadeStep  = 10.0 / length;

	show();
}

void Model::fadeOut(uint32 length) {
	_fadeStart = EventMan.getTimestamp();
	_fadeStep  = - (10.0 / length);
	_fade      = true;
}

void Model::newFrame() {
	_distance = -_position[2];
}

void Model::render() {
	if (!_currentState)
		return;

	if (_type == kModelTypeObject)
		// Roughly head position. TODO: This doesn't belong here :P
		glTranslatef(0.0, -1.5, 0.0);

	if (_type == kModelTypeGUIFront)
		// Aurora GUI objects use 0.01 units / pixel
		glScalef(100, 100, 100);

	// Apply rotation around the world center
	glRotatef(_orientation[0], 1.0, 0.0, 0.0);
	glRotatef(_orientation[1], 0.0, 1.0, 0.0);
	glRotatef(_orientation[2], 0.0, 0.0, 1.0);

	if (_type == kModelTypeObject)
		// Aurora world objects have a rotated axis
		glRotatef(90.0, -1.0, 0.0, 0.0);

	// Apply position translation
	glTranslatef(_position[0], _position[1], _position[2]);

	// Apply rotation around the object's center
	glRotatef(_bearing[0], 1.0, 0.0, 0.0);
	glRotatef(_bearing[1], 0.0, 0.0, 1.0);
	glRotatef(_bearing[2], 0.0, 1.0, 0.0);

	// Apply current fade value
	glColor4f(1.0, 1.0, 1.0, _fadeValue);

	if (_fade) {
		// Evaluate fading

		uint32 now = EventMan.getTimestamp();
		if ((now - _fadeStart) >= 10) {
			// Get new fade value every 10ms
			_fadeValue += _fadeStep * ((now - _fadeStart) / 10.0);

			_fadeStart = now;
		}

		if        (_fadeValue > 1.0) {
			// Fade in stepped
			_fade      = false;
			_fadeValue = 1.0;
		} else if (_fadeValue < 0.0) {
			// Fade out stopped
			_fade      = false;
			_fadeValue = 0.0;
		}
	}

	// Activate all needed texture units
	for (uint32 i = 0; i < _textureCount; i++) {
		TextureMan.activeTexture(i);
		glEnable(GL_TEXTURE_2D);
	}

	TextureMan.activeTexture(0);

	// Render the model
	renderState(*_currentState);

	// Disable all extra texture units
	for (uint32 i = 1; i < _textureCount; i++) {
		TextureMan.activeTexture(i);
		glDisable(GL_TEXTURE_2D);
	}

	TextureMan.activeTexture(0);
	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void Model::renderState(const State &state) {
	// For each starting node, apply translation and rotation, then render it
	for (NodeList::const_iterator node = state.nodes.begin(); node != state.nodes.end(); ++node) {
		glPushMatrix();
		glTranslatef((*node)->position[0], (*node)->position[1], (*node)->position[2]);
		glRotatef((*node)->orientation[3], (*node)->orientation[0], (*node)->orientation[1], (*node)->orientation[2]);
		renderNode(**node);
		glPopMatrix();
	}
}

void Model::renderNode(const Node &node) {
	if (node.render) {
		// If the node is visible...

		if (node.textures.empty()) {
			// If it doesn't have textures, "empty" the texture unit

			TextureMan.activeTexture(0);
			TextureMan.set();
		}

		for (uint32 i = 0; i < node.textures.size(); i++) {
			// Set all needed textures

			TextureMan.activeTexture(i);
			TextureMan.set(node.textures[i]);
		}

		// Call the node's OpenGL list
		if (node.list != 0)
			glCallList(node.list);
	}

	// Recurse over all child nodes, adding to the translation/rotation
	for (NodeList::const_iterator child = node.children.begin(); child != node.children.end(); ++child) {
		if (!*child)
			continue;

		glPushMatrix();
		glTranslatef((*child)->position[0], (*child)->position[1], (*child)->position[2]);
		glRotatef((*child)->orientation[3], (*child)->orientation[0], (*child)->orientation[1], (*child)->orientation[2]);
		renderNode(**child);
		glPopMatrix();
	}

}

void Model::doRebuild() {
	// Rebuild all node lists

	if (_nodes.empty())
		return;

	ListID list = glGenLists(_nodes.size());

	_list = list;
	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node) {
		if (!(*node)->render)
			continue;

		(*node)->list = list++;

		glNewList((*node)->list, GL_COMPILE);

		glBegin(GL_TRIANGLES);

		// For all faces
		for (FaceList::const_iterator face = (*node)->faces.begin(); face != (*node)->faces.end(); ++face) {
			// Texture vertex A
			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 0 * 3 + 0], face->tverts[t + 0 * 3 + 1]);

			// Geometry vertex A
			glVertex3f(face->verts[0 * 3 + 0], face->verts[0 * 3 + 1], face->verts[0 * 3 + 2]);

			// Texture vertex B
			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 1 * 3 + 0], face->tverts[t + 1 * 3 + 1]);

			// Geometry vertex B
			glVertex3f(face->verts[1 * 3 + 0], face->verts[1 * 3 + 1], face->verts[1 * 3 + 2]);

			// Texture vertex C
			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 2 * 3 + 0], face->tverts[t + 2 * 3 + 1]);

			// Geometry vertex C
			glVertex3f(face->verts[2 * 3 + 0], face->verts[2 * 3 + 1], face->verts[2 * 3 + 2]);
		}

		glEnd();

		glEndList();
	}
}

void Model::doDestroy() {
	// Destroy all node lists

	if ((_list == 0) || _nodes.empty())
		return;

	glDeleteLists(_list, _nodes.size());

	_list = 0;
	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node)
		(*node)->list = 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
