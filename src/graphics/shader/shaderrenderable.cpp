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

#include "src/graphics/shader/shaderrenderable.h"

namespace Graphics {

namespace Shader {

ShaderRenderable::ShaderRenderable() : _surface(0), _material(0), _program(0), _mesh(0) {
}

ShaderRenderable::ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh) : _surface(surface), _material(material), _program(0), _mesh(mesh) {
	_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
}

ShaderRenderable::~ShaderRenderable() {
	// TODO: one less surface in use, one less material, one less program usage count, one less mesh pointer.
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
	_surface = surface;
	updateProgram();
}

void ShaderRenderable::setMaterial(Shader::ShaderMaterial *material) {
	// TODO: check old surface for usage count decrement.
	_material = material;
	updateProgram();
}

void ShaderRenderable::setMesh(Mesh::Mesh *mesh) {
	_mesh = mesh;
}

void ShaderRenderable::renderImmediate(const Common::Matrix4x4 &tform) {
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
