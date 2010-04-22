/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/cube.cpp
 *  A simple cube object, for testing.
 */

#include "common/util.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "graphics/cube.h"
#include "graphics/texture.h"
#include "graphics/images/tga.h"

#include "events/events.h"
#include "events/requests.h"

using Events::RequestID;

namespace Graphics {

CubeSide::CubeSide(Cube &parent, int n) : _parent(&parent), _n(n) {
	addToQueue();
}

void CubeSide::newFrame() {
	_parent->newFrame();

	glPushMatrix();
	_parent->applyTransformation(_n);
	setCurrentDistance();
	glPopMatrix();
}

void CubeSide::render() {
	_parent->applyTransformation(_n);
	_parent->setTexture();
	_parent->callList();
}


Cube::Cube(const Common::UString &texture) : _firstTime(true), _lastRotateTime(0), _list(0) {
	RequestMan.dispatchAndForget(RequestMan.buildLists(this));

	_texture = new Texture(texture);

	for (int i = 0; i < 6; i++)
		_sides[i] = new CubeSide(*this, i);
}

Cube::~Cube() {
	if (_list != 0)
		RequestMan.dispatchAndForget(RequestMan.destroyLists(_list, 1));

	delete _texture;

	for (int i = 0; i < 6; i++)
		delete _sides[i];
}

void Cube::rebuild() {
	_list = glGenLists(1);

	glNewList(_list, GL_COMPILE);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-1.00, -1.00,  0.00);
			glTexCoord2f(1.0, 0.0);
			glVertex3f( 1.00, -1.00,  0.00);
			glTexCoord2f(1.0, 1.0);
			glVertex3f( 1.00,  1.00,  0.00);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-1.00,  1.00,  0.00);
		glEnd();

	glEndList();
}

void Cube::destroy() {
	if (_list == 0)
		return;

	glDeleteLists(_list, 1);

	_list = 0;
}

void Cube::setRotate(float rotate) {
	glRotatef(-rotate, 1.0, 0.0, 0.0);
	glRotatef( rotate, 0.0, 1.0, 0.0);
	glRotatef( rotate, 0.0, 0.0, 1.0);
}

void Cube::newFrame() {
	if (_lastRotateTime == 0)
		_lastRotateTime = EventMan.getTimestamp();

	uint32 curTime  = EventMan.getTimestamp();
	uint32 diffTime = curTime - _lastRotateTime;

	_rotation = diffTime * 0.1;
	if (_rotation >= 360)
		_lastRotateTime = curTime;
}

void Cube::applyTransformation(int n) {
	glTranslatef(0.0, 0.0, -3.0);
	setRotate(_rotation);
	glScalef(0.5, 0.5, 0.5);

	switch (n) {
		case 0:
			glTranslatef(0.0, 0.0, 1.0);
			break;
		case 1:
			glTranslatef(0.0, 0.0, -1.0);
			break;
		case 2:
			glRotatef(90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, 1.0);
			break;
		case 3:
			glRotatef(90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			break;
		case 4:
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glTranslatef(0.0, 0.0, 1.0);
			break;
		case 5:
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glTranslatef(0.0, 0.0, -1.0);
			break;
	}
}

void Cube::setTexture() {
	if (_firstTime) {
		// Sync, to make sure that the texture has finished loading
		RequestMan.sync();
		_firstTime = false;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture->getID());
}

void Cube::callList() {
	glCallList(_list);
}

} // End of namespace Graphics
