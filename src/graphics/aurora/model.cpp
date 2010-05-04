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

#include "events/events.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

Model::Node::Node() : parent(0), texture(0), dangly(false), displacement(0) {
}


Model::Model() : _superModel(0), _class(kClassOther), _scale(1.0) {
}

Model::~Model() {
	for (NodeMap::iterator node = _nodes.begin(); node != _nodes.end(); ++node)
		delete node->second;
}

void Model::show() {
	addToQueue();
}

void Model::hide() {
	removeFromQueue();
}

void Model::newFrame() {
}

void Model::render() {
	glTranslatef(0.0, 0.5, -3.0);

	float rotate = EventMan.getTimestamp() * 0.1;

	glRotatef(rotate, 0.0, 1.0, 0.0);
	glScalef(1.0, 1.0, 1.0);

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (NodeMap::const_iterator node = _nodes.begin(); node != _nodes.end(); ++node) {

		if (node->second->texture)
			glBindTexture(GL_TEXTURE_2D, node->second->texture->getID());

		glBegin(GL_TRIANGLES);

		for (FaceList::const_iterator face = node->second->faces.begin(); face != node->second->faces.end(); ++face) {
			glTexCoord2f(face->verticesTexture[0][0], face->verticesTexture[0][1]);
			glVertex3f(face->vertices[0][0], face->vertices[0][1], face->vertices[0][2]);
			glTexCoord2f(face->verticesTexture[1][0], face->verticesTexture[1][1]);
			glVertex3f(face->vertices[1][0], face->vertices[1][1], face->vertices[1][2]);
			glTexCoord2f(face->verticesTexture[2][0], face->verticesTexture[2][1]);
			glVertex3f(face->vertices[2][0], face->vertices[2][1], face->vertices[2][2]);
		}

		glEnd();

	}
}

} // End of namespace Aurora

} // End of namespace Graphics
