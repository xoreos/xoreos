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

#define SHADER_MATERIAL_TRANSPARENT            (0x00000001)  // Transparency is applied.
#define SHADER_MATERIAL_NOCULLFACE             (0x00000002)  // No face culling is used - typically only used with transparency enabled.
#define SHADER_MATERIAL_NODEPTHTEST            (0x00000004)  // Depth tests disabled (this is always rendered).
#define SHADER_MATERIAL_NODEPTHMASK            (0x00000008)  // Depth mask writes disabled. This is normal for transparent objects.

// Transparency information (if any).
#define SHADER_MATERIAL_TRANSPARENT_SRC_MASK   (0x000001F0)
#define SHADER_MATERIAL_TRANSPARENT_DST_MASK   (0x00003E00)
#define SHADER_MATERIAL_TRANSPARENT_SHIFT_MASK (0x001F)
#define SHADER_MATERIAL_TRANSPARENT_SRC_SHIFT  (4)
#define SHADER_MATERIAL_TRANSPARENT_DST_SHIFT  (9)

// Hints as to which render pass this material should be used on.
#define SHADER_MATERIAL_HINT_MASK              (0xF0000000)
#define SHADER_MATERIAL_HINT_COLOR             (0x00000000)
#define SHADER_MATERIAL_HINT_SHADOW            (0x10000000)
#define SHADER_MATERIAL_HINT_LIGHT             (0x20000000)
#define SHADER_MATERIAL_HINT_POST              (0x30000000)

class ShaderMaterial {
public:
	ShaderMaterial(Shader::ShaderObject *fragShader, const Common::UString &name = "unnamed");
	~ShaderMaterial();

	const Common::UString &getName() const;

	uint32 getFlags() const;
	void setFlags(uint32 flags);

	Shader::ShaderObject *getFragmentShader() const;

	uint32 getVariableCount() const;

	Shader::ShaderVariableType getVariableType(uint32 index) const;

	void *getVariableData(uint32 index) const;
	void *getVariableData(const Common::UString &name) const;

	const Common::UString &getVariableName(uint32 index) const;

	uint32 getVariableFlags(uint32 index) const;

	// Do not use this function to set sampler data. Instead, get the the variable data and modify
	// it directly; the texture unit associated with the texture id might be incorrect otherwise.
	void setVariableExternal(uint32 index, void *loc, bool textureUnitRecalc = true);
	void setVariableExternal(const Common::UString &name, void *loc, bool textureUnitRecalc = true);
	void setVariableInternal(uint32 index, bool textureUnitRecalc = true);
	void setVariableInternal(const Common::UString &name, bool textureUnitRecalc = true);

	void recalcTextureUnits();

	bool isVariableOwned(uint32 index) const;
	bool isVariableOwned(const Common::UString &name) const;

	void bindProgram(Shader::ShaderProgram *program);

	void bindGLState();
	void unbindGLState();
	void restoreGLState();

	void useIncrement();
	void useDecrement();
	uint32 useCount() const;

private:
	struct ShaderMaterialVariable {
		void *data;
		uint32 flags;  // Full flags may or may not be required here.
	};

	std::vector<ShaderMaterialVariable> _variableData;
	Shader::ShaderObject *_fragShader;
	uint32 _flags;

	Common::UString _name;
	uint32 _usageCount;

	void *genMaterialVar(uint32 index);
	void delMaterialVar(uint32 index);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERMATERIAL_H
