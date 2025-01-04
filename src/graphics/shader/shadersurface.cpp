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
 *  Shader surface, responsible for tracking data relating to a vertex shader.
 */
#include "src/graphics/graphics.h"

#include "src/graphics/shader/shadersurface.h"

namespace Graphics {

namespace Shader {

#define SHADER_SURFACE_VARIABLE_OWNED (0x00000001)

ShaderSurface::ShaderSurface(Shader::ShaderObject *vertShader, const Common::UString &name) :
		_variableData(),
		_vertShader(vertShader),
		_flags(0),
		_name(name) {

	vertShader->usageCount++;

	uint32_t varCount = vertShader->variablesCombined.size();
	_variableData.resize(varCount);

	for (uint32_t i = 0; i < varCount; ++i) {
		if (vertShader->variablesCombined[i].name == "_objectModelviewMatrix") {
			setVariable(i, &_objectModelviewMatrix);
		} else if (vertShader->variablesCombined[i].name == "_projectionMatrix") {
			setVariable(i, &(GfxMan.getProjectionMatrix()));
		} else if (vertShader->variablesCombined[i].name == "_modelviewMatrix") {
			setVariable(i, &(GfxMan.getModelviewMatrix()));
		}
	}
}
#if 0
ShaderSurface::ShaderSurface(const ShaderSurface &src) :
	_variableData(),
	_vertShader(src._vertShader),
	_flags(src._flags),
	_objectModelviewMatrix(src._objectModelviewMatrix),
	_name(src._name) {

	_vertShader->usageCount++;
	uint32_t varCount = _vertShader->variablesCombined.size();
	_variableData.resize(varCount);
	for (uint32_t i = 0; i < varCount; ++i) {
		_variableData[i] = src._variableData[i];
		if (_vertShader->variablesCombined[i].name == "_objectModelviewMatrix") {
			if (src._variableData[i] == &(src._objectModelviewMatrix)) {
				setVariable(i, &_objectModelviewMatrix);
			}
		}
	}
}
#endif
ShaderSurface::~ShaderSurface() {
	if (_vertShader) {
		_vertShader->usageCount--;
	}
}

const Common::UString &ShaderSurface::getName() const {
	return _name;
}

Shader::ShaderObject *ShaderSurface::getVertexShader() const {
	return _vertShader;
}

uint32_t ShaderSurface::getFlags() const {
	return _flags;
}

void ShaderSurface::setFlags(uint32_t flags) {
	_flags = flags;
}

uint32_t ShaderSurface::getVariableCount() const {
	return _vertShader->variablesCombined.size();
	// return _variableData.size(); // Should be equal to the frag shader variable count.
}

Shader::ShaderVariableType ShaderSurface::getVariableType(uint32_t index) const {
	return _vertShader->variablesCombined[index].type;
}

const void *ShaderSurface::getVariableData(uint32_t index) const {
	return _variableData[index];
}

const void *ShaderSurface::getVariableData(const Common::UString &name) const {
	const void *rval = nullptr;

	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			rval = _variableData[i];
			break;
		}
	}

	return rval;
}

const Common::UString &ShaderSurface::getVariableName(uint32_t index) const {
	return _vertShader->variablesCombined[index].name;
}

void ShaderSurface::setVariable(uint32_t index, const void *loc) {
	_variableData[index] = loc;
}

void ShaderSurface::setVariable(const Common::UString &name, const void *loc) {
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			_variableData[i] = loc;
			break;
		}
	}
}

void ShaderSurface::addUBO(uint32_t index, GLuint glid) {
	_uboArray.push_back(Shader::ShaderUBO(index, glid));
}

void ShaderSurface::bindProgram(Shader::ShaderProgram *program) {
	for (uint32_t i = 0; i < _variableData.size(); i++) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[i], program->vertexVariableLocations[i], _variableData[i]);
	}
}

void ShaderSurface::bindProgram(Shader::ShaderProgram *program, const glm::mat4 &t) {
	_objectModelviewMatrix = t;
	this->bindProgram(program);
}


void ShaderSurface::bindGLState() {
	if (_flags & SHADER_SURFACE_NOCULL) {
		glDisable(GL_CULL_FACE);
	}

	for (uint32_t i = 0; i < _uboArray.size(); ++i) {
		glBindBufferBase(GL_UNIFORM_BUFFER, _uboArray[i].index, _uboArray[i].glid);
	}
}

void ShaderSurface::unbindGLState() {
	// No need to unbind the uniform buffers really. If a shader uses them, new ones should be bound.

	if (_flags & SHADER_SURFACE_NOCULL) {
		glEnable(GL_CULL_FACE);
	}
}

void ShaderSurface::restoreGLState() {
}

} // End of namespace Shader

} // End of namespace Graphics
