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

#ifndef GRAPHICS_SHADER_SHADERINSTANCE_H
#define GRAPHICS_SHADER_SHADERINSTANCE_H

#include "src/graphics/shader/shader.h"

namespace Graphics {

namespace Shader {

class ShaderInstance {
public:
	ShaderInstance(Shader::ShaderObject *vertShader, Shader::ShaderObject *fragShader);
	ShaderInstance(const std::string &vertName, const std::string &fragName);
	~ShaderInstance();

	void setVertexVariable(const std::string &name, const void *location);
	void setFragmentVariable(const std::string &name, const void *location);

	void bind();
	void unbind();

	void rebindVertexVariable(uint32_t index);
	void rebindFragmentVariable(uint32_t index);

private:
	std::vector<const void *> _vertexVariableData;
	std::vector<const void *> _fragmentVariableData;
	Shader::ShaderProgram *_sourceProgram;
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERINSTANCE_H
