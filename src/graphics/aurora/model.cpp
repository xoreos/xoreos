/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "events/events.h"
#include "events/requests.h"

#include "graphics/graphics.h"
#include "graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

Model::Mesh::Mesh() : faceCount(0) {
}


Model::Node::Node() : parent(0), dangly(false), displacement(0), render(true), list(0) {
	position   [0] = 0;
	position   [1] = 0;
	position   [2] = 0;
	orientation[0] = 0;
	orientation[1] = 0;
	orientation[2] = 0;
	orientation[3] = 0;
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
}

Model::~Model() {
	if ((_list != 0) && !_nodes.empty())
		RequestMan.dispatchAndForget(RequestMan.destroyLists(_list, _nodes.size()));

	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node) {
		if (*node) {
			for (std::vector<TextureHandle>::iterator t = (*node)->textures.begin(); t != (*node)->textures.end(); ++t)
				TextureMan.release(*t);

			delete *node;
		}
	}

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
	for (uint t = 0; t < mesh.textures.size(); t++) {
		try {
			if (!mesh.textures[t].empty() && (mesh.textures[t] != "NULL"))
				node.textures[t] = TextureMan.get(mesh.textures[t]);
		} catch (...) {
			warning("Failed loading texture \"%s\"", mesh.textures[t].c_str());
			node.textures[t].clear();
		}
	}

	node.faces.resize(mesh.faceCount);

	// Go over each face and assign the actual coordinates
	for (uint32 i = 0; i < mesh.faceCount; i++) {
		Face &face = node.faces[i];

		face.verts.resize(9);

		// Real face coordinates
		for (int v = 0; v < 3; v++)
			for (int c = 0; c < 3; c++)
				face.verts[v * 3 + c] = mesh.verts[3 * mesh.vertIndices[3 * i + v] + c];

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

void Model::buildLists() {
	RequestMan.dispatchAndForget(RequestMan.buildLists(this));
}

void Model::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Model::setOrientation(float x, float y) {
	_orientation[0] = 0.0;
	_orientation[1] = Common::rad2deg(acos(-y / sqrt(x * x + y * y)));
	_orientation[2] = 0.0;

	if (x < 0)
		_orientation[1] = -_orientation[1];
}

const std::list<Common::UString> &Model::getStates() const {
	return _stateNames;
}

void Model::setState(const Common::UString &name) {
	if (_states.empty())
		return;

	StateMap::iterator state = _states.find(name);
	if (state == _states.end())
		state = _states.find("");

	if (state == _states.end()) {
		_currentState = 0;
		return;
	}

	_currentState = state->second;
}

void Model::show() {
	Renderable::addToQueue();
}

void Model::hide() {
	Renderable::removeFromQueue();
}

bool Model::shown() {
	if (_fadeValue <= 0.0)
		Renderable::removeFromQueue();

	return Renderable::isInQueue();
}

void Model::fadeIn(uint32 length) {
	_fade      = true;
	_fadeStart = EventMan.getTimestamp();
	_fadeValue = 0.0;
	_fadeStep  = 1.0 / 100;

	show();
}

void Model::fadeOut(uint32 length) {
	_fadeStart = EventMan.getTimestamp();
	_fadeStep  = - (1.0 / 100);
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
		glScalef(100, 100, 100);

	glRotatef(_orientation[0], 1.0, 0.0, 0.0);
	glRotatef(_orientation[1], 0.0, 1.0, 0.0);
	glRotatef(_orientation[2], 0.0, 0.0, 1.0);

	if (_type == kModelTypeObject)
		glRotatef(90.0, -1.0, 0.0, 0.0);

	glTranslatef(_position[0], _position[1], _position[2]);

	glColor4f(1.0, 1.0, 1.0, _fadeValue);

	if (_fade) {
		uint32 now = EventMan.getTimestamp();
		if ((now - _fadeStart) >= 10) {
			_fadeValue += _fadeStep * ((now - _fadeStart) / 10.0);

			_fadeStart = now;
		}

		if        (_fadeValue > 1.0) {
			_fade      = false;
			_fadeValue = 1.0;
		} else if (_fadeValue < 0.0) {
			_fade      = false;
			_fadeValue = 0.0;
		}
	}

	for (uint32 i = 0; i < _textureCount; i++) {
		TextureMan.activeTexture(i);
		glEnable(GL_TEXTURE_2D);
	}

	TextureMan.activeTexture(0);

	renderState(*_currentState);

	for (uint32 i = 0; i < _textureCount; i++) {
		TextureMan.activeTexture(i);
		glDisable(GL_TEXTURE_2D);
	}

	TextureMan.activeTexture(0);
	glEnable(GL_TEXTURE_2D);
}

void Model::renderState(const State &state) {
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
		if (node.textures.empty()) {
			TextureMan.activeTexture(0);
			TextureMan.set();
		}

		for (uint32 i = 0; i < node.textures.size(); i++) {
			TextureMan.activeTexture(i);
			TextureMan.set(node.textures[i]);
		}

		if (node.list != 0)
			glCallList(node.list);
	}

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

void Model::rebuild() {
	if (_nodes.empty())
		return;

	ListID list = glGenLists(_nodes.size());

	_list = list;
	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node) {
		(*node)->list = list++;

		glNewList((*node)->list, GL_COMPILE);

		glBegin(GL_TRIANGLES);

		for (FaceList::const_iterator face = (*node)->faces.begin(); face != (*node)->faces.end(); ++face) {
			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 0 * 3 + 0], face->tverts[t + 0 * 3 + 1]);

			glVertex3f(face->verts[0 * 3 + 0], face->verts[0 * 3 + 1], face->verts[0 * 3 + 2]);

			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 1 * 3 + 0], face->tverts[t + 1 * 3 + 1]);

			glVertex3f(face->verts[1 * 3 + 0], face->verts[1 * 3 + 1], face->verts[1 * 3 + 2]);

			for (uint32 i = 0, t = 0; i < (*node)->textures.size(); i++, t += 9)
				TextureMan.textureCoord2f(i, face->tverts[t + 2 * 3 + 0], face->tverts[t + 2 * 3 + 1]);

			glVertex3f(face->verts[2 * 3 + 0], face->verts[2 * 3 + 1], face->verts[2 * 3 + 2]);
		}

		glEnd();

		glEndList();
	}
}

void Model::destroy() {
	if ((_list == 0) || _nodes.empty())
		return;

	glDeleteLists(_list, _nodes.size());

	_list = 0;
	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node)
		(*node)->list = 0;
}

} // End of namespace Aurora

} // End of namespace Graphics
