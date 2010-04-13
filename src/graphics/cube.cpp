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
#include "common/stream.h"

#include "graphics/cube.h"
#include "graphics/images/tga.h"

#include "events/events.h"
#include "events/requests.h"

using Events::RequestID;

namespace Graphics {

CubeSide::CubeSide(Cube &parent, int n) : _parent(&parent), _n(n) {
	addToRenderQueue();
}

void CubeSide::newFrame() {
	_parent->newFrame();

	glPushMatrix();
	_parent->applyTransformation(_n);
	setCurrentDistance();
	glPopMatrix();
}

void CubeSide::reloadTextures() {
	_parent->reloadTextures();
}

void CubeSide::render() {
	_parent->applyTransformation(_n);
	_parent->setTexture();

	glColor4f(1.0, 1.0, 1.0, 0.5);

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00, -1.00,  0.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00, -1.00,  0.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00,  1.00,  0.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00,  1.00,  0.00);
	glEnd();
}


Cube::Cube(ImageDecoder *texture) : _textureImage(texture) {
	_lastRotateTime = 0;

	_texture = 0xFFFFFFFF;

	reloadTextures();

	for (int i = 0; i < 6; i++)
		_sides[i] = new CubeSide(*this, i);
}

Cube::~Cube() {
	RequestID destroyTex = RequestMan.destroyTextures(1, &_texture);
	RequestMan.dispatchAndForget(destroyTex);

	delete _textureImage;

	for (int i = 0; i < 6; i++)
		delete _sides[i];
}

void Cube::reloadTextures() {
	bool noReload;
	RequestID isTexture = RequestMan.isTexture(_texture, &noReload);
	RequestMan.dispatchAndWait(isTexture);
	if (noReload)
		return;

	RequestID createTex = RequestMan.createTextures(1, &_texture);
	RequestMan.dispatch(createTex);

	_textureImage->load();

	RequestMan.waitReply(createTex);

	RequestID loadTex = RequestMan.loadTexture(_texture, _textureImage);
	RequestMan.dispatchAndWait(loadTex);
}

	/*
	GLfloat modelView[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	status("%f", modelView[12] * modelView[12] + modelView[13] * modelView[13] + modelView[14] * modelView[14]);
	*/

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

	if (diffTime > 3600)
		curTime += 3600;

	_rotation = diffTime * 0.1;
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
	if (_sides[0]->_justAddedToQueue) {
		reloadTextures();
		_sides[0]->_justAddedToQueue = false;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture);
}

} // End of namespace Graphics
