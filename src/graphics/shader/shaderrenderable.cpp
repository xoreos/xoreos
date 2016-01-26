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
 *  Shader renderable, a class for easier managing of a collection of items (surface, material, mesh) to render with.
 */

#include "glm/gtc/type_ptr.hpp"

#include "src/graphics/shader/shaderrenderable.h"

namespace Graphics {

namespace Shader {

ShaderRenderable::ShaderRenderable() : _surface(0), _material(0), _program(0), _mesh(0) {
}

ShaderRenderable::ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh) : _surface(surface), _material(material), _program(0), _mesh(mesh) {
	_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	_surface->useIncrement();
	_material->useIncrement();
	_mesh->useIncrement();
}

ShaderRenderable::~ShaderRenderable() {
	if (_surface) {
		_surface->useDecrement();
	}
	if (_material) {
		_material->useDecrement();
	}
	if (_mesh) {
		_mesh->useDecrement();
	}
}

ShaderSurface *ShaderRenderable::getSurface() {
	return _surface;
}

ShaderMaterial *ShaderRenderable::getMaterial() {
	return _material;
}

ShaderProgram *ShaderRenderable::getProgram() {
	return _program;
}

Mesh::Mesh *ShaderRenderable::getMesh() {
	return _mesh;
}

void ShaderRenderable::setSurface(Shader::ShaderSurface *surface) {
	// TODO: check old surface for usage count decrement.
	if (_surface) {
		_surface->useDecrement();
	}
	_surface = surface;
	if (_surface) {
		_surface->useIncrement();
	}
	updateProgram();
}

void ShaderRenderable::setMaterial(Shader::ShaderMaterial *material) {
	if (_material) {
		_material->useDecrement();
	}
	_material = material;
	if (_material) {
		_material->useIncrement();
	}
	updateProgram();
}

void ShaderRenderable::setMesh(Mesh::Mesh *mesh) {
	if (_mesh) {
		_mesh->useDecrement();
	}
	_mesh = mesh;
	if (_mesh) {
		_mesh->useIncrement();
	}
}

void ShaderRenderable::copyRenderable(ShaderRenderable *src) {
	if (src) {
		setSurface(src->getSurface());
		setMaterial(src->getMaterial());
		setMesh(src->getMesh());
	} else {
		setSurface(0);
		setMaterial(0);
		setMesh(0);
	}
	updateProgram();
}

void ShaderRenderable::renderImmediate(const glm::mat4 &tform) {
	glUseProgram(_program->glid);
	_material->bindProgram(_program);
	_material->bindGLState();
	_surface->bindProgram(_program, &tform);
	_surface->bindGLState();

	_mesh->renderImmediate();

	_surface->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void ShaderRenderable::updateProgram() {
	if ((_surface != 0) && (_material != 0)) {
		_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	} else {
		_program = 0;
	}
}

} // End of namespace Shader

} // End of namespace Graphics
