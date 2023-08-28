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
 *  Shader material, responsible for tracking data relating to a fragment shader.
 */

#ifndef GRAPHICS_SHADER_SHADERMATERIAL_H
#define GRAPHICS_SHADER_SHADERMATERIAL_H

#include "src/graphics/shader/shader.h"

namespace Graphics {

namespace Shader {

class ShaderMaterial {
public:
	enum {
		MATERIAL_OPAQUE        = 0x0001,  ///< Render hint; material has no transparency.
		MATERIAL_TRANSPARENT   = 0x0002,  ///< Material definitely has transparency.
		MATERIAL_TRANSPARENT_B = 0x0004,  ///< Material definitely has transparency.
		MATERIAL_DECAL         = 0x0008,  ///< Material is a decal.
		MATERIAL_CUSTOM_BLEND  = 0x0010,  ///< Different blending is being used, see _blend* variables.
	};

	ShaderMaterial(Shader::ShaderObject *fragShader, const Common::UString &name = "unnamed");
	~ShaderMaterial();

	const Common::UString &getName() const;

	uint32_t getFlags() const;
	void setFlags(uint32_t flags);

	void setBlendEquationRGB(GLenum equation);
	void setBlendEquationAlpha(GLenum equation);
	void setBlendSrcRGB(GLenum mode);
	void setBlendSrcAlpha(GLenum mode);
	void setBlendDstRGB(GLenum mode);
	void setBlendDstAlpha(GLenum mode);

	Shader::ShaderObject *getFragmentShader() const;

	uint32_t getVariableCount() const;

	Shader::ShaderVariableType getVariableType(uint32_t index) const;

	const void *getVariableData(uint32_t index) const;
	const void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32_t index) const;

	void setVariable(uint32_t index, const void *loc);
	void setVariable(const Common::UString &name, const void *loc);
	void setTexture(const Common::UString &name, Graphics::Aurora::TextureHandle handle);

	void bindProgram(Shader::ShaderProgram *program);
	void bindProgram(Shader::ShaderProgram *program, float alpha);

	void bindGLState();
	void unbindGLState();
	void restoreGLState();

private:

	std::vector<const void *> _variableData;
	std::vector<Shader::ShaderSampler> _samplers;
	Shader::ShaderObject *_fragShader;
	uint32_t _flags;
	GLenum _blendEquationRGB;
	GLenum _blendEquationAlpha;
	GLenum _blendSrcRGB;
	GLenum _blendSrcAlpha;
	GLenum _blendDstRGB;
	GLenum _blendDstAlpha;

	float _alpha;  ///< Give the material an alpha variable for convenience.
	Common::UString _name;
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERMATERIAL_H
