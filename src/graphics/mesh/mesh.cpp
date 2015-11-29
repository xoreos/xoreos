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
 *  Generic mesh handling class.
 */

#include "src/graphics/mesh/mesh.h"

namespace Graphics {

namespace Mesh {

Mesh::Mesh(GLuint type, GLuint hint) : GLContainer(), _type(type), _hint(hint), _usageCount(0), _vao(0) {
}

Mesh::~Mesh() {
	removeFromQueue(kQueueNewTexture); // In case the mesh is destroyed while waiting to be initialised.
	destroyGL();
}

VertexBuffer *Mesh::getVertexBuffer() {
	return &_vertexBuffer;
}

IndexBuffer *Mesh::getIndexBuffer() {
	return &_indexBuffer;
}

void Mesh::setName(const Common::UString &name) {
	_name = name;
}

const Common::UString &Mesh::getName() const {
	return _name;
}

void Mesh::setType(GLuint type) {
	_type = type;
}

GLuint Mesh::getType() const {
	return _type;
}

void Mesh::init() {
	// Borrowing kQueueNewTexture for now as a more generic GLContainer initialiser.
	removeFromQueue(kQueueNewTexture);
	addToQueue(kQueueNewTexture);
}

void Mesh::initGL() {
	_vertexBuffer.initGL();
	_indexBuffer.initGL();

	// GL3.x render path uses Vertex Array (attribute) Objects.
	if (GfxMan.isGL3()) {
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		const VertexDecl &decl = _vertexBuffer.getVertexDecl();

		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer.getVBO());
		for (uint32 i = 0; i < decl.size(); ++i) {
			// Using intptr_t to ensure correct bit length for the architecture.
			intptr_t offset = (intptr_t) (decl[i].pointer);
			offset -= (intptr_t) (_vertexBuffer.getData());
			glVertexAttribPointer(decl[i].index,
			                      decl[i].size,
			                      decl[i].type,
			                      GL_FALSE,
			                      decl[i].stride,
			                      reinterpret_cast<void *>(offset));
			glEnableVertexAttribArray(decl[i].index);
		}

		if (_indexBuffer.getCount()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer.getIBO());
		}

		// Finish up with the VAO, ready for use.
		glBindVertexArray(0);

		// This will eventually not be required, but definitely is when mixing with old-style OpenGL rendering calls.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	} else {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// TODO: create boundary volume descriptions.
}

void Mesh::updateGL() {
	_vertexBuffer.updateGL();
	_indexBuffer.updateGL();
}

void Mesh::destroyGL() {
	_vertexBuffer.destroyGL();
	_indexBuffer.destroyGL();

	// Should only be non-zero on GL3.x contexts anyway.
	if (_vao) {
		glDeleteVertexArrays(1, &_vao);
		_vao = 0;
	}
}

void Mesh::renderImmediate() {
	renderBind();
	render();
	renderUnbind();
}

void Mesh::renderBind() {
	if (GfxMan.isGL3()) {
		glBindVertexArray(_vao);
	} else {
		const VertexDecl &decl = _vertexBuffer.getVertexDecl();
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer.getVBO());
		for (size_t i = 0; i < decl.size(); ++i) {
			intptr_t offset = (intptr_t) (decl[i].pointer);
			offset -= (intptr_t) (_vertexBuffer.getData());
			switch (decl[i].index) {
				case VPOSITION:
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(decl[i].size, decl[i].type, decl[i].stride, (GLvoid *)(offset));
					break;
				case VNORMAL:
					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(decl[i].type, decl[i].stride, (GLvoid *)(offset));
					break;
				case VCOLOR:
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(decl[i].size, decl[i].type, decl[i].stride, (GLvoid *)(offset));
					break;
				default: // VTCOORD
					glClientActiveTextureARB(GL_TEXTURE0 + decl[i].index - VTCOORD);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(decl[i].size, decl[i].type, decl[i].stride, (GLvoid *)(offset));
					break;
			}
		}
	}
}

void Mesh::render() {
	if (GfxMan.isGL3()) {
		if (_indexBuffer.getCount()) {
			glDrawElements(_type, _indexBuffer.getCount(), _indexBuffer.getType(), 0);
		} else {
			glDrawArrays(_type, 0, _vertexBuffer.getCount());
		}
	} else {
		if (_indexBuffer.getCount()) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer.getIBO());
			glDrawElements(_type, _indexBuffer.getCount(), _indexBuffer.getType(), 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		} else {
			glDrawArrays(_type, 0, _vertexBuffer.getCount());
		}
	}
}

void Mesh::renderUnbind() {
	if (GfxMan.isGL3()) {
		// So long as each mesh rebinds what it needs, there's actually no need to bind 0 here.
		glBindVertexArray(0);
	} else {
		const VertexDecl &decl = _vertexBuffer.getVertexDecl();
		for (size_t i = 0; i < decl.size(); ++i) {
			switch (decl[i].index) {
				case VPOSITION:
					glDisableClientState(GL_VERTEX_ARRAY);
					break;
				case VNORMAL:
					glDisableClientState(GL_NORMAL_ARRAY);
					break;
				case VCOLOR:
					glDisableClientState(GL_COLOR_ARRAY);
					break;
				default: // VTCOORD
					glClientActiveTextureARB(GL_TEXTURE0 + decl[i].index - VTCOORD);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					break;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Mesh::useIncrement() {
	++_usageCount;
}

void Mesh::useDecrement() {
	if (_usageCount) {
		--_usageCount;
	}
}

uint32 Mesh::useCount() const {
	return _usageCount;
}

void Mesh::doRebuild() {
	// We could do some fancy things here, like checking to see if GL resources
	// have already been built, and just updating them. Let's just be lazy and
	// force a complete rebuild instead.

	destroyGL();
	initGL();
}

void Mesh::doDestroy() {
	destroyGL();
}

} // End of namespace Mesh

} // End of namespace Graphics
