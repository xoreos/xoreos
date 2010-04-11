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

namespace Graphics {

Cube::Cube(Common::SeekableReadStream &tgaTexture) {
	_lastRotateTime = 0;

	_textureImage = 0;

	initTexture(tgaTexture);

	addToRenderQueue();
}

Cube::~Cube() {
	delete _textureImage;
}

void Cube::initTexture(Common::SeekableReadStream &tgaTexture) {
	Events::RequestCreateTextures *createTex = new Events::RequestCreateTextures(1, &_texture);

	createTex->dispatch();

	_textureImage = new TGA(tgaTexture);

	createTex->waitReply();

	Events::RequestLoadTexture *loadTex = new Events::RequestLoadTexture(_texture, _textureImage);

	loadTex->dispatchAndWait();

	delete loadTex;
	delete createTex;
}

void Cube::reloadTextures() {
	if (glIsTexture(_texture))
		// No need to reload
		return;

	Events::RequestCreateTextures *createTex = new Events::RequestCreateTextures(1, &_texture);

	createTex->dispatchAndWait();

	delete createTex;

	Events::RequestLoadTexture *loadTex = new Events::RequestLoadTexture(_texture, _textureImage);

	loadTex->dispatchAndWait();

	delete loadTex;
}

void Cube::doCubeSolid() {
	glColor3f(1.00, 1.00, 1.00);

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00,  1.00,  1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00,  1.00,  1.00);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00, -1.00, -1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00, -1.00, -1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00,  1.00, -1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00,  1.00, -1.00);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00, -1.00, -1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(-1.00,  1.00,  1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00,  1.00, -1.00);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f( 1.00, -1.00, -1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00,  1.00,  1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f( 1.00,  1.00, -1.00);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00, -1.00,  1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00, -1.00, -1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00, -1.00, -1.00);
	glEnd();

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.00,  1.00,  1.00);
		glTexCoord2f(1.0, 0.0);
		glVertex3f( 1.00,  1.00,  1.00);
		glTexCoord2f(1.0, 1.0);
		glVertex3f( 1.00,  1.00, -1.00);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.00,  1.00, -1.00);
	glEnd();
}

void Cube::setRotate(float rotate) {
	glRotatef(-rotate, 1.0, 0.0, 0.0);
	glRotatef( rotate, 0.0, 1.0, 0.0);
	glRotatef( rotate, 0.0, 0.0, 1.0);
}

void Cube::render() {
	// If we were just added to the queue, look if we need to reload our textures
	if (_justAddedToQueue) {
		reloadTextures();
		_justAddedToQueue = false;
	}

	if (_lastRotateTime == 0)
		_lastRotateTime = EventMan.getTimestamp();

	uint32 curTime  = EventMan.getTimestamp();
	uint32 diffTime = curTime - _lastRotateTime;

	if (diffTime > 3600)
		curTime += 3600;

	glTranslatef(0.0, 0.0, -3.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glPushMatrix();
	setRotate(diffTime * 0.1);
	glScalef(0.5, 0.5, 0.5);
	doCubeSolid();
	glPopMatrix();
}

} // End of namespace Graphics
