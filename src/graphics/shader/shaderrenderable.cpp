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

#include "external/glm/gtc/type_ptr.hpp"

#include "src/graphics/shader/shaderrenderable.h"

namespace Graphics {

namespace Shader {

ShaderRenderable::ShaderRenderable() : _surface(0), _material(0), _program(0), _mesh(0) {
}

ShaderRenderable::ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh) : _surface(surface), _material(material), _program(0), _mesh(mesh) {
	//_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	_mesh->useIncrement();
	updateProgram();
}

ShaderRenderable::ShaderRenderable(Shader::ShaderRenderable *src) : _surface(0), _material(0), _program(0), _mesh(0) {
	if (src) {
		setSurface(src->_surface, false);
		setMaterial(src->_material, false);
		setMesh(src->_mesh);
		updateProgram();
	}
}

ShaderRenderable::ShaderRenderable(const Shader::ShaderRenderable &src) : _surface(src._surface), _material(src._material), _program(src._program), _mesh(src._mesh) {
	if (_mesh) {
		_mesh->useIncrement();
	}
}

const ShaderRenderable &ShaderRenderable::operator=(const Shader::ShaderRenderable &src) {
	setSurface(src._surface, false);
	setMaterial(src._material, false);
	setMesh(src._mesh);
	_program = src._program;
	return *this;
}

ShaderRenderable::~ShaderRenderable() {
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

void ShaderRenderable::setSurface(Shader::ShaderSurface *surface, bool rebuildProgram) {
	_surface = surface;
	if (rebuildProgram) {
		updateProgram();
	}
}

void ShaderRenderable::setMaterial(Shader::ShaderMaterial *material, bool rebuildProgram) {
	_material = material;
	if (rebuildProgram) {
		updateProgram();
	}
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
		setSurface(src->getSurface(), false);
		setMaterial(src->getMaterial(), false);
		setMesh(src->getMesh());
	} else {
		setSurface(0, false);
		setMaterial(0, false);
		setMesh(0);
	}
	updateProgram();
}

void ShaderRenderable::renderImmediate(const glm::mat4 &tform, float alpha) {
	if (!_program->glid)
		return;
	glUseProgram(_program->glid);
	_material->bindProgram(_program, alpha);
	_material->bindGLState();
	_surface->bindProgram(_program, tform);
	_surface->bindGLState();

	_mesh->renderImmediate();

	_surface->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void ShaderRenderable::updateProgram() {
	if ((_surface != 0) && (_material != 0)) {
		ShaderMan.registerShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
		_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	} else {
		_program = 0;
	}
}

} // End of namespace Shader

} // End of namespace Graphics
