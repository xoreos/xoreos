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
 *  Shader instancing, allowing for easier shader resource sharing.
 */

#include "src/graphics/shader/shaderinstance.h"

namespace Graphics {

namespace Shader {

ShaderInstance::ShaderInstance(Shader::ShaderObject *fragShader, Shader::ShaderObject *vertShader) : _vertexVariableData(), _fragmentVariableData(), _sourceProgram(0) {
	Shader::ShaderProgram *program = ShaderMan.getShaderProgram(vertShader, fragShader);
	if (!program) {
		ShaderMan.registerShaderProgram(vertShader, fragShader);
		program = ShaderMan.getShaderProgram(vertShader, fragShader);
	}

	if (program) {
		_sourceProgram = program;
		_sourceProgram->usageCount++;
		for (uint32_t i = 0; i < program->vertexObject->variablesCombined.size(); ++i)
			_vertexVariableData.push_back(0);
		_vertexVariableData.resize(_vertexVariableData.size());

		for (uint32_t i = 0; i < program->fragmentObject->variablesCombined.size(); ++i)
			_fragmentVariableData.push_back(0);
		_fragmentVariableData.resize(_fragmentVariableData.size());
	}
}

ShaderInstance::ShaderInstance(const std::string &vertName, const std::string &fragName) : _vertexVariableData(), _fragmentVariableData(), _sourceProgram(0) {
	Shader::ShaderObject *vertShader = ShaderMan.getShaderObject(vertName, Shader::SHADER_VERTEX);
	Shader::ShaderObject *fragShader = ShaderMan.getShaderObject(fragName, Shader::SHADER_FRAGMENT);
	Shader::ShaderProgram *program = ShaderMan.getShaderProgram(vertShader, fragShader);
	if (!program) {
		ShaderMan.registerShaderProgram(vertShader, fragShader);
		program = ShaderMan.getShaderProgram(vertShader, fragShader);
	}

	if (program) {
		_sourceProgram = program;
		_sourceProgram->usageCount++;
		for (uint32_t i = 0; i < program->vertexObject->variablesCombined.size(); ++i) {
			_vertexVariableData.push_back(0);
		}
		_vertexVariableData.resize(_vertexVariableData.size());

		for (uint32_t i = 0; i < program->fragmentObject->variablesCombined.size(); ++i) {
			_fragmentVariableData.push_back(0);
		}
		_fragmentVariableData.resize(_fragmentVariableData.size());
	}
}

ShaderInstance::~ShaderInstance() {
	_sourceProgram->usageCount--;
}

void ShaderInstance::setVertexVariable(const std::string &name, const void *location) {
	for (uint32_t i = 0; i < _sourceProgram->vertexObject->variablesCombined.size(); ++i) {
		if (_sourceProgram->vertexObject->variablesCombined[i].name == name) {
			_vertexVariableData[i] = location;
			break;
		}
	}
}

void ShaderInstance::setFragmentVariable(const std::string &name, const void *location) {
	for (uint32_t i = 0; i < _sourceProgram->fragmentObject->variablesCombined.size(); ++i) {
		if (_sourceProgram->fragmentObject->variablesCombined[i].name == name) {
			_fragmentVariableData[i] = location;
			break;
		}
	}
}

void ShaderInstance::bind() {
	ShaderMan.bindShaderInstance(_sourceProgram, &(_vertexVariableData[0]), &(_fragmentVariableData[0]));
}

void ShaderInstance::unbind() {
	glUseProgram(0);
}

void ShaderInstance::rebindVertexVariable(uint32_t index) {
	ShaderMan.bindShaderVariable(_sourceProgram->vertexObject->variablesCombined[index], _sourceProgram->vertexVariableLocations[index], _vertexVariableData[index]);
}

void ShaderInstance::rebindFragmentVariable(uint32_t index) {
	ShaderMan.bindShaderVariable(_sourceProgram->fragmentObject->variablesCombined[index], _sourceProgram->fragmentVariableLocations[index], _fragmentVariableData[index]);
}

} // End of namespace Shader

} // End of namespace Graphics
