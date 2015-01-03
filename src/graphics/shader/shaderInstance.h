#ifndef SHADER_INSTANCE_H
#define SHADER_INSTANCE_H

#include "graphics/shader/shader.h"

namespace Graphics {

class shaderInstance {
public:
	shaderInstance(Shader::ShaderObject *vertShader, Shader::ShaderObject *fragShader);
	shaderInstance(const std::string &vertName, const std::string &fragName);

	void setVertexVariable(const std::string &name, const void *location);
	void setFragmentVariable(const std::string &name, const void *location);

	void bind();
	void unbind();

	void rebindVertexVariable(uint32_t index);
	void rebindFragmentVariable(uint32_t index);

	~shaderInstance();

private:
	std::vector<const void *> _vertexVariableData;
	std::vector<const void *> _fragmentVariableData;
	Shader::ShaderProgram *_sourceProgram;
};

} // namespace Graphics

#endif
