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
#include "common/stream.h"

#include "events/events.h"

#include "graphics/graphics.h"
#include "graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

Model::Node::Node() : parent(0), dangly(false), displacement(0), render(true) {
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
	_fadeValue(1.0), _fadeStep(0.0), _currentState(0) {

	_position[0] = 0.0;
	_position[1] = 0.0;
	_position[2] = 0.0;
}

Model::~Model() {
	for (NodeList::iterator node = _nodes.begin(); node != _nodes.end(); ++node) {
		if (*node) {
			TextureMan.release((*node)->texture);

			delete *node;
		}
	}
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
	node.faces.resize(mesh.faces.size());

	// Go over each face and assign the actual coordinates
	for (uint32 i = 0; i < mesh.faces.size(); i++) {
		Face &face = node.faces[i];

		// Real face coordinates
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				face.verts[j][k] = mesh.verts[3 * mesh.faces[i].verts[j] + k];

		// Real texture coordinates
		if (mesh.faces[i].tverts[0] >= (mesh.tverts.size() * 3))
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++)
					face.tverts[j][k] = 0.0;
		else
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++)
					face.tverts[j][k] = mesh.tverts[3 * mesh.faces[i].tverts[j] + k];

		face.smoothGroup = mesh.faces[i].smoothGroup;
		face.material    = mesh.faces[i].material;
	}

	// Try to load the texture
	try {
		if (!mesh.texture.empty() && (mesh.texture != "NULL"))
			node.texture = TextureMan.get(mesh.texture);
	} catch (...) {
		node.texture.clear();
	}
}

void Model::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
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
	addToQueue();
}

void Model::hide() {
	removeFromQueue();
}

bool Model::shown() {
	if (_fadeValue <= 0.0)
		removeFromQueue();

	return isInQueue();
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

	if (_type == kModelTypeObject) {
		glTranslatef(0.0, -1.0, -3.0);

		float rotate = EventMan.getTimestamp() * 0.1;

		glRotatef(rotate, 0.0, 1.0, 0.0);
		glScalef(1.2, 1.2, 1.2);
	}

	if (_type == kModelTypeGUIFront)
		glScalef(100, 100, 100);

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

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (_type == kModelTypeObject)
		glRotatef(90.0, -1.0, 0.0, 0.0);

	renderState(*_currentState);
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
		TextureMan.set(node.texture);

		glBegin(GL_TRIANGLES);

		for (FaceList::const_iterator face = node.faces.begin(); face != node.faces.end(); ++face) {
			glTexCoord2f(face->tverts[0][0], face->tverts[0][1]                   );
			glVertex3f  (face-> verts[0][0], face-> verts[0][1], face->verts[0][2]);
			glTexCoord2f(face->tverts[1][0], face->tverts[1][1]                   );
			glVertex3f  (face-> verts[1][0], face-> verts[1][1], face->verts[1][2]);
			glTexCoord2f(face->tverts[2][0], face->tverts[2][1]                   );
			glVertex3f  (face-> verts[2][0], face-> verts[2][1], face->verts[2][2]);
		}

		glEnd();
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

} // End of namespace Aurora

} // End of namespace Graphics
