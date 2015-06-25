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
 *  A simple 3D object.
 */

#include <cassert>

#include "src/common/util.h"

#include "src/graphics/aurora/geometryobject.h"
#include "src/graphics/aurora/textureman.h"

namespace Graphics {

namespace Aurora {

GeometryObject::GeometryObject(const VertexBuffer &vBuf, const IndexBuffer &iBuf) :
	Renderable(kRenderableTypeObject), _vertexBuffer(vBuf), _indexBuffer(iBuf) {

	_position[0] = 0.0f;
	_position[1] = 0.0f;
	_position[2] = 0.0f;
	_rotation[0] = 0.0f;
	_rotation[1] = 0.0f;
	_rotation[2] = 0.0f;
}

GeometryObject::~GeometryObject() {
}

void GeometryObject::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void GeometryObject::getRotation(float &x, float &y, float &z) const {
	x = _rotation[0];
	y = _rotation[1];
	z = _rotation[2];
}

void GeometryObject::setPosition(float x, float y, float z) {
	lockFrameIfVisible();

	_position[0] = x;
	_position[1] = y;
	_position[2] = z;

	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void GeometryObject::setRotation(float x, float y, float z) {
	lockFrameIfVisible();

	_rotation[0] = x;
	_rotation[1] = y;
	_rotation[2] = z;

	calculateDistance();

	resort();

	unlockFrameIfVisible();
}

void GeometryObject::move(float x, float y, float z) {
	setPosition(_position[0] + x, _position[1] + y, _position[2] + z);
}

void GeometryObject::rotate(float x, float y, float z) {
	setRotation(_rotation[0] + x, _rotation[1] + y, _rotation[2] + z);
}

void GeometryObject::calculateDistance() {
	_distance = 0;
}

static void EnableVertexPos(const VertexAttrib &va) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(va.size, va.type, va.stride, va.pointer);
}

static void EnableVertexNorm(const VertexAttrib &va) {
	assert(va.size == 3);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(va.type, va.stride, va.pointer);
}

static void EnableVertexCol(const VertexAttrib &va) {
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(va.size, va.type, va.stride, va.pointer);
}

static void EnableVertexTex(const VertexAttrib &va) {
	glClientActiveTextureARB(GL_TEXTURE0 + va.index - VTCOORD);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(va.size, va.type, va.stride, va.pointer);
}

static void DisableVertexPos(const VertexAttrib &UNUSED(va)) {
	glDisableClientState(GL_VERTEX_ARRAY);
}

static void DisableVertexNorm(const VertexAttrib &UNUSED(va)) {
	glDisableClientState(GL_NORMAL_ARRAY);
}

static void DisableVertexCol(const VertexAttrib &UNUSED(va)) {
	glDisableClientState(GL_COLOR_ARRAY);
}

static void DisableVertexTex(const VertexAttrib &va) {
	glClientActiveTextureARB(GL_TEXTURE0 + va.index - VTCOORD);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
static void EnableVertexAttrib(const VertexAttrib &va) {
	if (va.index == VPOSITION)
		EnableVertexPos(va);
	else if (va.index == VNORMAL)
		EnableVertexNorm(va);
	else if (va.index == VCOLOR)
		EnableVertexCol(va);
	else if (va.index >= VTCOORD)
		EnableVertexTex(va);
}

static void DisableVertexAttrib(const VertexAttrib &va) {
	if (va.index == VPOSITION)
		DisableVertexPos(va);
	else if (va.index == VNORMAL)
		DisableVertexNorm(va);
	else if (va.index == VCOLOR)
		DisableVertexCol(va);
	else if (va.index >= VTCOORD)
		DisableVertexTex(va);
}

void GeometryObject::render(RenderPass pass) {
	if (pass == kRenderPassTransparent)
		return;

	glTranslatef(_position[0], _position[1], _position[2]);

	glRotatef(_rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(_rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(_rotation[2], 0.0f, 0.0f, 1.0f);

	TextureMan.reset();

	// Render the node's faces

	const VertexDecl &vertexDecl = _vertexBuffer.getVertexDecl();

	for (size_t i = 0; i < vertexDecl.size(); i++)
		EnableVertexAttrib(vertexDecl[i]);

	glDrawElements(GL_TRIANGLES, _indexBuffer.getCount(), _indexBuffer.getType(), _indexBuffer.getData());

	for (size_t i = 0; i < vertexDecl.size(); i++)
		DisableVertexAttrib(vertexDecl[i]);
}

} // End of namespace Aurora

} // End of namespace Graphics
