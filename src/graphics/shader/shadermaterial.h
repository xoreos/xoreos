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

	void *getVariableData(uint32_t index) const;
	void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32_t index) const;

	uint32_t getVariableFlags(uint32_t index) const;

	// Do not use this function to set sampler data. Instead, get the the variable data and modify
	// it directly; the texture unit associated with the texture id might be incorrect otherwise.
	void setVariableExternal(uint32_t index, void *loc, bool textureUnitRecalc = true);
	void setVariableExternal(const Common::UString &name, void *loc, bool textureUnitRecalc = true);
	void setVariableInternal(uint32_t index, bool textureUnitRecalc = true);
	void setVariableInternal(const Common::UString &name, bool textureUnitRecalc = true);

	void recalcTextureUnits();

	bool isVariableOwned(uint32_t index) const;
	bool isVariableOwned(const Common::UString &name) const;

	void bindProgram(Shader::ShaderProgram *program);
	void bindProgramNoFade(Shader::ShaderProgram *program);
	void bindProgram(Shader::ShaderProgram *program, float fade);
	void bindFade(Shader::ShaderProgram *program, float fade);

	void bindGLState();
	void unbindGLState();
	void restoreGLState();

	void useIncrement();
	void useDecrement();
	uint32_t useCount() const;

private:
	struct ShaderMaterialVariable {
		void *data;
		uint32_t flags;  // Full flags may or may not be required here.
	};

	std::vector<ShaderMaterialVariable> _variableData;
	Shader::ShaderObject *_fragShader;
	uint32_t _flags;
	GLenum _blendEquationRGB;
	GLenum _blendEquationAlpha;
	GLenum _blendSrcRGB;
	GLenum _blendSrcAlpha;
	GLenum _blendDstRGB;
	GLenum _blendDstAlpha;

	Common::UString _name;
	uint32_t _usageCount;

	uint32_t _alphaIndex;

	void *genMaterialVar(uint32_t index);
	void delMaterialVar(uint32_t index);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERMATERIAL_H
