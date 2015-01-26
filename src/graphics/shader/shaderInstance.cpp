#include "graphics/shader/shaderInstance.h"

namespace Graphics {

shaderInstance::shaderInstance(Shader::ShaderObject *fragShader, Shader::ShaderObject *vertShader) : _vertexVariableData(), _fragmentVariableData(), _sourceProgram(NULL) {
	Shader::ShaderProgram *program = ShaderMan.getShaderProgram(vertShader, fragShader);
	if(!program) {
		ShaderMan.registerShaderProgram(vertShader, fragShader);
		program = ShaderMan.getShaderProgram(vertShader, fragShader);
	}

	if(program) {
		_sourceProgram = program;
		_sourceProgram->usageCount++;
		for(uint32_t i = 0; i < program->vertexObject->variablesCombined.size(); ++i)
			_vertexVariableData.push_back(NULL);
		_vertexVariableData.resize(_vertexVariableData.size());

		for(uint32_t i = 0; i < program->fragmentObject->variablesCombined.size(); ++i)
			_fragmentVariableData.push_back(NULL);
		_fragmentVariableData.resize(_fragmentVariableData.size());
	}
}

shaderInstance::shaderInstance(const std::string &vertName, const std::string &fragName) : _vertexVariableData(), _fragmentVariableData(), _sourceProgram(NULL) {
	Shader::ShaderObject *vertShader = ShaderMan.getShaderObject(vertName, Shader::SHADER_VERTEX);
	Shader::ShaderObject *fragShader = ShaderMan.getShaderObject(fragName, Shader::SHADER_FRAGMENT);
	Shader::ShaderProgram *program = ShaderMan.getShaderProgram(vertShader, fragShader);
	if(!program) {
		ShaderMan.registerShaderProgram(vertShader, fragShader);
		program = ShaderMan.getShaderProgram(vertShader, fragShader);
	}

	if(program) {
		_sourceProgram = program;
		_sourceProgram->usageCount++;
		for(uint32_t i = 0; i < program->vertexObject->variablesCombined.size(); ++i) {
			_vertexVariableData.push_back(NULL);
		}
		_vertexVariableData.resize(_vertexVariableData.size());

		for(uint32_t i = 0; i < program->fragmentObject->variablesCombined.size(); ++i) {
			_fragmentVariableData.push_back(NULL);
		}
		_fragmentVariableData.resize(_fragmentVariableData.size());
	}
}

void shaderInstance::setVertexVariable(const std::string &name, const void *location)
{
	for(uint32_t i = 0; i < _sourceProgram->vertexObject->variablesCombined.size(); i++) {
		if(_sourceProgram->vertexObject->variablesCombined[i].name == name) {
			_vertexVariableData[i] = location;
			break;
		}
	}
}

void shaderInstance::setFragmentVariable(const std::string &name, const void *location)
{
	for(uint32_t i = 0; i < _sourceProgram->fragmentObject->variablesCombined.size(); i++) {
		if(_sourceProgram->fragmentObject->variablesCombined[i].name == name) {
			_fragmentVariableData[i] = location;
			break;
		}
	}
}

void shaderInstance::bind() {
	ShaderMan.bindShaderInstance(_sourceProgram, &(_vertexVariableData[0]), &(_fragmentVariableData[0]));
}

void shaderInstance::unbind() {
	glUseProgram(0);
}

void shaderInstance::rebindVertexVariable(uint32_t index) {
	ShaderMan.bindShaderVariable(_sourceProgram->vertexObject->variablesCombined[index], _sourceProgram->vertexVariableLocations[index], _vertexVariableData[index]);
}

void shaderInstance::rebindFragmentVariable(uint32_t index) {
	ShaderMan.bindShaderVariable(_sourceProgram->fragmentObject->variablesCombined[index], _sourceProgram->fragmentVariableLocations[index], _fragmentVariableData[index]);
}


shaderInstance::~shaderInstance()
{
	_sourceProgram->usageCount--;
}

} // namespace invictus
