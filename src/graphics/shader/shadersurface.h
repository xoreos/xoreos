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

#include "src/common/matrix4x4.h"

#include "src/graphics/shader/shader.h"

namespace Graphics {

namespace Shader {

#define SHADER_SURFACE_WIREFRAME            (0x00000001)  // Surface should be rendered in wireframe mode.
#define SHADER_SURFACE_INSTANCED            (0x00000002)  // Surface is used for mesh instancing.

class ShaderSurface {
public:
	ShaderSurface(Shader::ShaderObject *vertShader, const Common::UString &name = "unnamed");
	~ShaderSurface();

	const Common::UString &getName() const;

	Shader::ShaderObject *getVertexShader() const;

	uint32 getVariableCount() const;

	Shader::ShaderVariableType getVariableType(uint32 index) const;

	void *getVariableData(uint32 index) const;
	void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32 index) const;

	uint32 getVariableFlags(uint32 index) const;

	// Do not use this function to set sampler data. Instead, get the the variable data and modify
	// it directly; the texture unit associated with the texture id might be incorrect otherwise.
	// (Note: at the time of writing, vertex shader samplers are not supported).
	void setVariableExternal(uint32 index, void *loc);
	void setVariableExternal(const Common::UString &name, void *loc);
	void setVariableInternal(uint32 index);
	void setVariableInternal(const Common::UString &name);

	bool isVariableOwned(uint32 index) const;
	bool isVariableOwned(const Common::UString &name) const;

	// UBOs (Uniform Buffer Objects) are only for >= GL3.x.
	void addUBO(uint32 index, GLuint glid);

	void bindProgram(Shader::ShaderProgram *program);
	void bindProgram(Shader::ShaderProgram *program, const Common::Matrix4x4 *t);
	void bindObjectModelview(Shader::ShaderProgram *program, const Common::Matrix4x4 *t);

	void bindGLState();
	void unbindGLState();
	void restoreGLState();

	void useIncrement();
	void useDecrement();
	uint32 useCount() const;

private:
	struct ShaderSurfaceVariable {
		void *data;
		uint32 flags;  // Full flags may or may not be required here.
	};

	std::vector<ShaderSurfaceVariable> _variableData;
	std::vector<Shader::ShaderUBO> _uboArray;
	Shader::ShaderObject *_vertShader;
	uint32 _flags;

	Common::UString _name;
	uint32 _usageCount;

	uint32 _objectModelviewIndex;

	void *genSurfaceVar(uint32 index);
	void delSurfaceVar(uint32 index);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERSURFACE_H
