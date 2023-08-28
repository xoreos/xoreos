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

#ifndef GRAPHICS_SHADER_SHADERSURFACE_H
#define GRAPHICS_SHADER_SHADERSURFACE_H

#include "external/glm/mat4x4.hpp"

#include "src/graphics/shader/shader.h"

namespace Graphics {

namespace Shader {

#define SHADER_SURFACE_WIREFRAME            (0x00000001)  // Surface should be rendered in wireframe mode.
#define SHADER_SURFACE_INSTANCED            (0x00000002)  // Surface is used for mesh instancing.
#define SHADER_SURFACE_NOCULL               (0x00000004)  // No face culling is applied.

class ShaderSurface {
public:
	ShaderSurface(Shader::ShaderObject *vertShader, const Common::UString &name = "unnamed");
	~ShaderSurface();

	const Common::UString &getName() const;

	Shader::ShaderObject *getVertexShader() const;

	uint32_t getFlags() const;
	void setFlags(uint32_t flags);

	uint32_t getVariableCount() const;

	Shader::ShaderVariableType getVariableType(uint32_t index) const;

	const void *getVariableData(uint32_t index) const;
	const void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32_t index) const;

	// Do not use this function to set sampler data. Instead, get the the variable data and modify
	// it directly; the texture unit associated with the texture id might be incorrect otherwise.
	// (Note: at the time of writing, vertex shader samplers are not supported).
	void setVariable(uint32_t index, const void *loc);
	void setVariable(const Common::UString &name, const void *loc);

	// UBOs (Uniform Buffer Objects) are only for >= GL3.x.
	void addUBO(uint32_t index, GLuint glid);

	void bindProgram(Shader::ShaderProgram *program);
	void bindProgram(Shader::ShaderProgram *program, const glm::mat4 &t);
#if 0
	/*
	void bindObjectModelview(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindTextureView(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindBindPose(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindBoneTransforms(Shader::ShaderProgram *program, const float *t);
	*/
#endif
	void bindGLState();
	void unbindGLState();
	void restoreGLState();

private:
	std::vector<const void *> _variableData;
	std::vector<Shader::ShaderUBO> _uboArray;
	Shader::ShaderObject *_vertShader;
	uint32_t _flags;

	glm::mat4 _objectModelviewMatrix;
	Common::UString _name;  // @TODO: name isn't required.
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERSURFACE_H
