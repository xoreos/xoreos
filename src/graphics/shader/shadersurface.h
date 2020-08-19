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

	void *getVariableData(uint32_t index) const;
	void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32_t index) const;

	uint32_t getVariableFlags(uint32_t index) const;

	// Do not use this function to set sampler data. Instead, get the the variable data and modify
	// it directly; the texture unit associated with the texture id might be incorrect otherwise.
	// (Note: at the time of writing, vertex shader samplers are not supported).
	void setVariableExternal(uint32_t index, void *loc);
	void setVariableExternal(const Common::UString &name, void *loc);
	void setVariableInternal(uint32_t index);
	void setVariableInternal(const Common::UString &name);

	bool isVariableOwned(uint32_t index) const;
	bool isVariableOwned(const Common::UString &name) const;

	// UBOs (Uniform Buffer Objects) are only for >= GL3.x.
	void addUBO(uint32_t index, GLuint glid);

	void bindProgram(Shader::ShaderProgram *program);
	void bindProgram(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindObjectModelview(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindTextureView(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindBindPose(Shader::ShaderProgram *program, const glm::mat4 *t);
	void bindBoneTransforms(Shader::ShaderProgram *program, const float *t);

	void bindGLState();
	void unbindGLState();
	void restoreGLState();

	void useIncrement();
	void useDecrement();
	uint32_t useCount() const;

private:
	struct ShaderSurfaceVariable {
		void *data;
		uint32_t flags;  // Full flags may or may not be required here.
	};

	std::vector<ShaderSurfaceVariable> _variableData;
	std::vector<Shader::ShaderUBO> _uboArray;
	Shader::ShaderObject *_vertShader;
	uint32_t _flags;

	Common::UString _name;
	uint32_t _usageCount;

	uint32_t _objectModelviewIndex;
	uint32_t _textureViewIndex;
	uint32_t _bindPoseIndex;
	uint32_t _boneTransformsIndex;

	void *genSurfaceVar(uint32_t index);
	void delSurfaceVar(uint32_t index);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERSURFACE_H
