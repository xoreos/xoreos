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
 *  A simple cube object, for testing.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/matrix4x4.h"

#include "src/graphics/graphics.h"

#include "src/graphics/images/tga.h"

#include "src/graphics/aurora/cube.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/events/events.h"
#include "src/events/requests.h"

using Events::RequestID;

namespace Graphics {

namespace Aurora {

CubeSide::CubeSide(Cube &parent, int n) : _parent(&parent), _n(n) {
	calculateDistance();

	show();
}

CubeSide::~CubeSide() {
	hide();
}

void CubeSide::calculateDistance() {
	Common::Matrix4x4 m;

	_parent->applyTransformation(_n, m);

	_distance = ABS(m.getX()) + ABS(m.getY()) + ABS(m.getZ());
}

void CubeSide::render(RenderPass pass) {
	bool isTransparent = _parent->_texture.getTexture().hasAlpha();
	if (((pass == kRenderPassOpaque)      &&  isTransparent) ||
			((pass == kRenderPassTransparent) && !isTransparent))
		return;

	_parent->applyTransformation(_n);
	_parent->setTexture();
	_parent->callList();

	_parent->newFrame();
}


Cube::Cube(const Common::UString &texture) : _firstTime(true), _lastRotateTime(0), _rotation(0.0f), _list(0) {
	_texture = TextureMan.get(texture);

	for (int i = 0; i < 6; i++)
		_sides[i] = new CubeSide(*this, i);

	RequestMan.dispatchAndForget(RequestMan.rebuild(*this));
}

Cube::~Cube() {
	removeFromQueue(kQueueGLContainer);

	for (int i = 0; i < 6; i++)
		delete _sides[i];

	if (_list != 0)
		GfxMan.abandon(_list, 1);
}

void Cube::doRebuild() {
	_list = glGenLists(1);

	glNewList(_list, GL_COMPILE);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.00f, -1.00f,  0.00f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f( 1.00f, -1.00f,  0.00f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f( 1.00f,  1.00f,  0.00f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.00f,  1.00f,  0.00f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.00f, -1.00f,  0.00f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.00f,  1.00f,  0.00f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f( 1.00f,  1.00f,  0.00f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f( 1.00f, -1.00f,  0.00f);
		glEnd();

	glEndList();
}

void Cube::doDestroy() {
	if (_list == 0)
		return;

	glDeleteLists(_list, 1);

	_list = 0;
}

void Cube::newFrame() {
	if (_lastRotateTime == 0)
		_lastRotateTime = EventMan.getTimestamp();

	uint32 curTime  = EventMan.getTimestamp();
	uint32 diffTime = curTime - _lastRotateTime;

	_rotation = diffTime * 0.1f;
	if (_rotation >= 360)
		_lastRotateTime = curTime;
}

void Cube::applyTransformation(int n) {
	glTranslatef(0.0f, 0.0f, -3.0f);

	glRotatef(-_rotation, 1.0f, 0.0f, 0.0f);
	glRotatef( _rotation, 0.0f, 1.0f, 0.0f);
	glRotatef( _rotation, 0.0f, 0.0f, 1.0f);

	glScalef(0.5f, 0.5f, 0.5f);

	switch (n) {
		case 0:
			glTranslatef(0.0f, 0.0f, 1.0f);
			break;
		case 1:
			glTranslatef(0.0f, 0.0f, -1.0f);
			break;
		case 2:
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, 1.0f);
			break;
		case 3:
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, -1.0f);
			break;
		case 4:
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, 1.0f);
			break;
		case 5:
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, -1.0f);
			break;
	}
}

void Cube::applyTransformation(int n, Common::Matrix4x4 &m) {
	m.translate(0.0f, 0.0f, -3.0f);

	m.rotate(-_rotation, 1.0f, 0.0f, 0.0f);
	m.rotate( _rotation, 0.0f, 1.0f, 0.0f);
	m.rotate( _rotation, 0.0f, 0.0f, 1.0f);

	m.scale(0.5f, 0.5f, 0.5f);

	switch (n) {
		case 0:
			m.translate(0.0f, 0.0f, 1.0f);
			break;
		case 1:
			m.translate(0.0f, 0.0f, -1.0f);
			break;
		case 2:
			m.rotate(90.0f, 0.0f, 1.0f, 0.0f);
			m.translate(0.0f, 0.0f, 1.0f);
			break;
		case 3:
			m.rotate(90.0f, 0.0f, 1.0f, 0.0f);
			m.translate(0.0f, 0.0f, -1.0f);
			break;
		case 4:
			m.rotate(90.0f, 1.0f, 0.0f, 0.0f);
			m.translate(0.0f, 0.0f, 1.0f);
			break;
		case 5:
			m.rotate(90.0f, 1.0f, 0.0f, 0.0f);
			m.translate(0.0f, 0.0f, -1.0f);
			break;
	}
}

void Cube::setTexture() {
	TextureMan.set(_texture);
}

void Cube::callList() {
	glCallList(_list);
}

} // End of namespace Aurora

} // End of namespace Graphics
