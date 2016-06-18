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

static const GLenum ShaderRenderableBlendfuncArray[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_SRC_ALPHA_SATURATE,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_COLOR,
	GL_SRC1_ALPHA,
	GL_ONE_MINUS_SRC1_ALPHA
};
#define BLEND_FUNC_COUNT (19)

ShaderRenderable::ShaderRenderable() : _surface(0), _material(0), _program(0), _mesh(0), _stateFlags(0) {
}

ShaderRenderable::ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh, uint32 stateflags) : _surface(surface), _material(material), _program(0), _mesh(mesh), _stateFlags(stateflags) {
	_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	_surface->useIncrement();
	_material->useIncrement();
	_mesh->useIncrement();
}

ShaderRenderable::ShaderRenderable(Shader::ShaderRenderable *src) : _surface(0), _material(0), _program(0), _mesh(0), _stateFlags(0) {
	if (src) {
		setSurface(src->_surface, false);
		setMaterial(src->_material, false);
		setMesh(src->_mesh);
		setStateFlags(src->_stateFlags);
		updateProgram();
	}
}

ShaderRenderable::ShaderRenderable(const Shader::ShaderRenderable &src) : _surface(src._surface), _material(src._material), _program(src._program), _mesh(src._mesh), _stateFlags(src._stateFlags) {
	if (_surface) {
		_surface->useIncrement();
	}
	if (_material) {
		_material->useIncrement();
	}
	if (_mesh) {
		_mesh->useIncrement();
	}
}

const ShaderRenderable &ShaderRenderable::operator=(const Shader::ShaderRenderable &src) {
	setSurface(src._surface, false);
	setMaterial(src._material, false);
	setMesh(src._mesh);
	_program = src._program;
	_stateFlags = src._stateFlags;
	return *this;
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

uint32 ShaderRenderable::getStateFlags() {
	return _stateFlags;
}

void ShaderRenderable::setSurface(Shader::ShaderSurface *surface, bool rebuildProgram) {
	// TODO: check old surface for usage count decrement.
	if (_surface) {
		_surface->useDecrement();
	}
	_surface = surface;
	if (_surface) {
		_surface->useIncrement();
	}
	if (rebuildProgram) {
		updateProgram();
	}
}

void ShaderRenderable::setMaterial(Shader::ShaderMaterial *material, bool rebuildProgram) {
	if (_material) {
		_material->useDecrement();
	}
	_material = material;
	if (_material) {
		_material->useIncrement();
	}
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

void ShaderRenderable::setStateFlags(uint32 flags) {
	_stateFlags = flags;
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

void ShaderRenderable::bindState() {
	if (_stateFlags & SHADER_RENDER_TRANSPARENT) {
		glBlendFunc(ShaderRenderableBlendfuncArray[(_stateFlags >> SHADER_RENDER_TRANSPARENT_SRC_SHIFT) & SHADER_RENDER_TRANSPARENT_SHIFT_MASK],
					ShaderRenderableBlendfuncArray[(_stateFlags >> SHADER_RENDER_TRANSPARENT_DST_SHIFT) & SHADER_RENDER_TRANSPARENT_SHIFT_MASK]);
		//glEnable(GL_BLEND);  // Blending is never disabled. It's just how the games were written.
	}

	if (_stateFlags & SHADER_RENDER_NOCULLFACE) {
		glDisable(GL_CULL_FACE);
	}

	if (_stateFlags & SHADER_RENDER_NODEPTHTEST) {
		glDisable(GL_DEPTH_TEST);
	}

	if (_stateFlags & SHADER_RENDER_NODEPTHMASK) {
		glDepthMask(GL_FALSE);
	}

	if (_stateFlags & SHADER_RENDER_NOALPHATEST) {
		glDisable(GL_ALPHA_TEST);
	}
}

void ShaderRenderable::unbindState() {
	if (_stateFlags & SHADER_RENDER_TRANSPARENT) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_BLEND);  // Blending is never disabled. It's just how the games were written.
	}

	if (_stateFlags & SHADER_RENDER_NOCULLFACE) {
		glEnable(GL_CULL_FACE);
	}

	if (_stateFlags & SHADER_RENDER_NODEPTHTEST) {
		glEnable(GL_DEPTH_TEST);
	}

	if (_stateFlags & SHADER_RENDER_NODEPTHMASK) {
		glDepthMask(GL_TRUE);
	}

	if (_stateFlags & SHADER_RENDER_NOALPHATEST) {
		glEnable(GL_ALPHA_TEST);
	}
}

void ShaderRenderable::renderImmediate(const glm::mat4 &tform, float alpha) {
	this->bindState();
	glUseProgram(_program->glid);
	_material->bindProgram(_program, alpha);
	_material->bindGLState();
	_surface->bindProgram(_program, &tform);
	_surface->bindGLState();

	_mesh->renderImmediate();

	_surface->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
	this->unbindState();
}

void ShaderRenderable::updateProgram() {
	if ((_surface != 0) && (_material != 0)) {
		ShaderMan.registerShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
		_program = ShaderMan.getShaderProgram(_surface->getVertexShader(), _material->getFragmentShader());
	} else {
		_program = 0;
	}
}

uint32 ShaderRenderable::genBlendFlags(GLenum src, GLenum dst) {
	uint32 flags = 0;

	// First figure out the src flags.
	for (uint32 i = 0; i < BLEND_FUNC_COUNT; i++) {
		if (src == ShaderRenderableBlendfuncArray[i]) {
			flags |= i << SHADER_RENDER_TRANSPARENT_SRC_SHIFT;
			break;
		}
	}

	// Now get the dst flags.
	for (uint32 i = 0; i < BLEND_FUNC_COUNT; i++) {
		if (dst == ShaderRenderableBlendfuncArray[i]) {
			flags |= i << SHADER_RENDER_TRANSPARENT_DST_SHIFT;
			break;
		}
	}

	return flags;
}

} // End of namespace Shader

} // End of namespace Graphics
