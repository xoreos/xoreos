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

#include "graphics/cube.h"

#include "events/events.h"

namespace Graphics {

Cube::Cube() {
	_lastRotateTime = 0;
	_rotateX        = 0.0;

	addToRenderQueue();
}

Cube::~Cube() {
}

void Cube::render() {
	if (_lastRotateTime == 0)
		_lastRotateTime = EventMan.getTimestamp();

	uint32 curTime = EventMan.getTimestamp();
	if ((curTime - _lastRotateTime) > 5) {
		_rotateX += 0.5;
		_lastRotateTime = curTime;

		if (_rotateX >= 360.0)
			_rotateX = 0.0;
	}

	glColor3f(0.2, 1.0, 0.2);

	glTranslatef(0.0, 0.0, -2.0);
	glRotatef(_rotateX, 0.0, 1.0, 0.0);
	glRotatef(-45.0, 1.0, 0.0, 0.0);

	glBegin(GL_POLYGON);
		glVertex3f(-0.25, -0.25, 0.25);
		glVertex3f( 0.25, -0.25, 0.25);
		glVertex3f( 0.25,  0.25, 0.25);
		glVertex3f(-0.25,  0.25, 0.25);
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(-0.25, -0.25, -0.25);
		glVertex3f( 0.25, -0.25, -0.25);
		glVertex3f( 0.25,  0.25, -0.25);
		glVertex3f(-0.25,  0.25, -0.25);
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(-0.25, -0.25, -0.25);
		glVertex3f(-0.25, -0.25,  0.25);
		glVertex3f(-0.25,  0.25,  0.25);
		glVertex3f(-0.25,  0.25, -0.25);
	glEnd();
}

} // End of namespace Graphics
