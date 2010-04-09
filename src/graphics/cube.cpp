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

namespace Graphics {

Cube::Cube() {
	addToRenderQueue();
}

Cube::~Cube() {
}

void Cube::render() {
	glColor3f(0.2,1.0,0.2);

	glTranslatef(0.0, 0.0, -2.0);
	glRotatef(45.0, 0.0, 1.0, 0.0);
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
