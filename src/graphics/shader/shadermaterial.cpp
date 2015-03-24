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

#include "src/graphics/shader/shadermaterial.h"

namespace Graphics {

namespace Shader {

#define SHADER_MATERIAL_VARIABLE_OWNED (0x00000001)

static const GLenum ShaderMaterialBlendfuncArray[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_SRC_ALPHA_SATURATE,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_COLOR,
	GL_SRC1_ALPHA,
	GL_ONE_MINUS_SRC1_ALPHA
};

ShaderMaterial::ShaderMaterial(Shader::ShaderObject *fragShader, const Common::UString &name) : _variableData(), _fragShader(fragShader), _flags(0), _name(name), _usageCount(0) {
	fragShader->usageCount++;

	uint32 varCount = fragShader->variablesCombined.size();
	_variableData.resize(varCount);
	for (uint32 i = 0; i < varCount; ++i) {
		_variableData[i].flags = 0;
		genMaterialVar(i);
	}
}

ShaderMaterial::~ShaderMaterial() {
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		delMaterialVar(i);
	}
}

const Common::UString &ShaderMaterial::getName() const {
	return _name;
}

uint32 ShaderMaterial::getFlags() const {
	return _flags;
}

void ShaderMaterial::setFlags(uint32 flags) {
	_flags = flags;
}

Shader::ShaderObject *ShaderMaterial::getFragmentShader() const {
	return _fragShader;
}

uint32 ShaderMaterial::getVariableCount() const {
	return _fragShader->variablesCombined.size();
	// return _variableData.size(); // Should be equal to the frag shader variable count.
}

Shader::ShaderVariableType ShaderMaterial::getVariableType(uint32 index) const {
	return _fragShader->variablesCombined[index].type;
}

void *ShaderMaterial::getVariableData(uint32 index) const {
	return _variableData[index].data;
}

void *ShaderMaterial::getVariableData(const Common::UString &name) const {
	void *rval = 0;

	for (uint32 i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			rval = _variableData[i].data;
			break;
		}
	}

	return rval;
}

const Common::UString &ShaderMaterial::getVariableName(uint32 index) const {
	return _fragShader->variablesCombined[index].name;
}

uint32 ShaderMaterial::getVariableFlags(uint32 index) const {
	return _variableData[index].flags;
}

void ShaderMaterial::setVariableExternal(uint32 index, void *loc, bool textureUnitRecalc) {
	delMaterialVar(index);
	_variableData[index].data = loc;
	if (textureUnitRecalc) {
		recalcTextureUnits();
	}
}

void ShaderMaterial::setVariableExternal(const Common::UString &name, void *loc, bool textureUnitRecalc) {
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			delMaterialVar(i);
			_variableData[i].data = loc;
			break;
		}
	}

	if (textureUnitRecalc) {
		recalcTextureUnits();
	}
}

void ShaderMaterial::setVariableInternal(uint32 index, bool textureUnitRecalc) {
	genMaterialVar(index);

	if (textureUnitRecalc) {
		recalcTextureUnits();
	}
}

void ShaderMaterial::setVariableInternal(const Common::UString &name, bool textureUnitRecalc) {
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			genMaterialVar(i);
			break;
		}
	}

	if (textureUnitRecalc) {
		recalcTextureUnits();
	}
}

void ShaderMaterial::recalcTextureUnits() {
	uint32 unit = 0;

	// First iterate through and find all external variables - they have master control over texture unit assignment.
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		switch (_fragShader->variablesCombined[i].type) {
			case SHADER_SAMPLER1D:
			case SHADER_SAMPLER2D:
			case SHADER_SAMPLER3D:
			case SHADER_SAMPLERCUBE:
				if (!(_variableData[i].flags & SHADER_MATERIAL_VARIABLE_OWNED)) {
					unit |= (1 << (static_cast<Shader::ShaderSampler *>(_variableData[i].data)->unit));
				}
			break;
			default: break;
		}
	}

	// Now fill in the gaps.
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		switch (_fragShader->variablesCombined[i].type) {
			case SHADER_SAMPLER1D:
			case SHADER_SAMPLER2D:
			case SHADER_SAMPLER3D:
			case SHADER_SAMPLERCUBE:
				if (_variableData[i].flags & SHADER_MATERIAL_VARIABLE_OWNED) {
					uint32 textureUnit = 0;
					for (uint32 j = 1; j < 0x80000000; j<<=1, ++textureUnit)
					{
						if (!(unit & j)) {
							unit |= j;
							break;
						}
					}
					static_cast<Shader::ShaderSampler *>(_variableData[i].data)->unit = textureUnit;
				}
				break;
			default: break;
		}
	}
}

bool ShaderMaterial::isVariableOwned(uint32 index) const {
	return (_variableData[index].flags & SHADER_MATERIAL_VARIABLE_OWNED) ? true : false;
}

bool ShaderMaterial::isVariableOwned(const Common::UString &name) const {
	bool rval = false;
	for (uint32 i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			rval = (_variableData[i].flags & SHADER_MATERIAL_VARIABLE_OWNED) ? true : false;
			break;
		}
	}

	return rval;
}

void ShaderMaterial::bindProgram(Shader::ShaderProgram *program) {
	for (uint32 i = 0; i < _variableData.size(); i++) {
		ShaderMan.bindShaderVariable(program->fragmentObject->variablesCombined[i], program->fragmentVariableLocations[i], _variableData[i].data);
	}
}

void ShaderMaterial::bindGLState() {
	if (_flags & SHADER_MATERIAL_TRANSPARENT) {
		glBlendFunc(ShaderMaterialBlendfuncArray[(_flags >> SHADER_MATERIAL_TRANSPARENT_SRC_SHIFT) & SHADER_MATERIAL_TRANSPARENT_SHIFT_MASK],
		            ShaderMaterialBlendfuncArray[(_flags >> SHADER_MATERIAL_TRANSPARENT_DST_SHIFT) & SHADER_MATERIAL_TRANSPARENT_SHIFT_MASK]);
		glEnable(GL_BLEND);
	}

	if (_flags & SHADER_MATERIAL_NOCULLFACE) {
		glDisable(GL_CULL_FACE);
	}

	if (_flags & SHADER_MATERIAL_NODEPTHTEST) {
		glDisable(GL_DEPTH_TEST);
	}

	if (_flags & SHADER_MATERIAL_NODEPTHMASK) {
		glDepthMask(GL_FALSE);
	}
}

void ShaderMaterial::unbindGLState() {
	if (_flags & SHADER_MATERIAL_TRANSPARENT) {
		glDisable(GL_BLEND);
	}

	if (_flags & SHADER_MATERIAL_NOCULLFACE) {
		glEnable(GL_CULL_FACE);
	}

	if (_flags & SHADER_MATERIAL_NODEPTHTEST) {
		glEnable(GL_DEPTH_TEST);
	}

	if (_flags & SHADER_MATERIAL_NODEPTHMASK) {
		glDepthMask(GL_TRUE);
	}
}

void ShaderMaterial::restoreGLState() {
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void *ShaderMaterial::genMaterialVar(uint32 index) {
	if (_variableData[index].flags & SHADER_MATERIAL_VARIABLE_OWNED)
		return 0;

	void *rval = 0;
	uint32 count = _fragShader->variablesCombined[index].count;

	switch (_fragShader->variablesCombined[index].type) {
		case SHADER_FLOAT: rval = new float[count];     break;
		case SHADER_VEC2:  rval = new float[2 * count]; break;
		case SHADER_VEC3:  rval = new float[3 * count]; break;
		case SHADER_VEC4:  rval = new float[4 * count]; break;
		case SHADER_INT:   rval = new int[count];       break;
		case SHADER_IVEC2: rval = new int[2 * count];   break;
		case SHADER_IVEC3: rval = new int[3 * count];   break;
		case SHADER_IVEC4: rval = new int[4 * count];   break;
//		case SHADER_UINT:            break;
//		case SHADER_UVEC2:           break;
//		case SHADER_UVEC3:           break;
//		case SHADER_UVEC4:           break;
//		case SHADER_BOOL:            break;
//		case SHADER_BVEC2:           break;
//		case SHADER_BVEC3:           break;
//		case SHADER_BVEC4:           break;
		case SHADER_MAT2: rval = new float[4 * count];  break;
		case SHADER_MAT3: rval = new float[9 * count];  break;
		case SHADER_MAT4: rval = new float[16 * count]; break;
		case SHADER_SAMPLER1D:
		case SHADER_SAMPLER2D:
		case SHADER_SAMPLER3D:
		case SHADER_SAMPLERCUBE: rval = new Shader::ShaderSampler(); break;
		case SHADER_SAMPLER1DSHADOW: break;
		case SHADER_SAMPLER2DSHADOW: break;
		case SHADER_SAMPLER1DARRAY:  break;
		case SHADER_SAMPLER2DARRAY:  break;
		case SHADER_SAMPLER1DARRAYSHADOW: break;
		case SHADER_SAMPLER2DARRAYSHADOW: break;
		case SHADER_SAMPLERBUFFER:   break;
		case SHADER_ISAMPLER1D:      break;
		case SHADER_ISAMPLER2D:      break;
		case SHADER_ISAMPLER3D:      break;
		case SHADER_ISAMPLERCUBE:    break;
		case SHADER_ISAMPLER1DARRAY: break;
		case SHADER_ISAMPLER2DARRAY: break;
		case SHADER_USAMPLER1D:      break;
		case SHADER_USAMPLER2D:      break;
		case SHADER_USAMPLER3D:      break;
		case SHADER_USAMPLERCUBE:    break;
		case SHADER_USAMPLER1DARRAY: break;
		case SHADER_USAMPLER2DARRAY: break;
		case SHADER_UNIFORM_BUFFER:  rval = new Shader::ShaderUBO; break;
		default: break;
	}

	_variableData[index].flags |= SHADER_MATERIAL_VARIABLE_OWNED;
	_variableData[index].data = rval;

	return rval;
}

void ShaderMaterial::delMaterialVar(uint32 index)
{
	if (!(_variableData[index].flags & SHADER_MATERIAL_VARIABLE_OWNED))
		return;

	_variableData[index].flags &= ~(SHADER_MATERIAL_VARIABLE_OWNED);

	void *data = _variableData[index].data;
	switch (_fragShader->variablesCombined[index].type) {
		case SHADER_FLOAT:
		case SHADER_VEC2:
		case SHADER_VEC3:
		case SHADER_VEC4: delete [] (static_cast<float *>(data)); break;
		case SHADER_INT:
		case SHADER_IVEC2:
		case SHADER_IVEC3:
		case SHADER_IVEC4: delete [] (static_cast<int *>(data)); break;
//		case SHADER_UINT: break;
//		case SHADER_UVEC2: break;
//		case SHADER_UVEC3: break;
//		case SHADER_UVEC4: break;
//		case SHADER_BOOL: break;
//		case SHADER_BVEC2: break;
//		case SHADER_BVEC3: break;
//		case SHADER_BVEC4: break;
		case SHADER_MAT2:
		case SHADER_MAT3:
		case SHADER_MAT4: delete [] (static_cast<float *>(data)); break;
		case SHADER_SAMPLER1D:
		case SHADER_SAMPLER2D:
		case SHADER_SAMPLER3D:
		case SHADER_SAMPLERCUBE:
			// todo: link in texture usage count properly here.
			//if(static_cast<ShaderMaterial::ShaderMaterialSampler *>(data)->texture != 0)
			//	static_cast<ShaderMaterial::ShaderMaterialSampler *>(data)->texture->_usageCount--;
			delete (static_cast<Shader::ShaderSampler *>(data));
		break;
		case SHADER_SAMPLER1DSHADOW: break;
		case SHADER_SAMPLER2DSHADOW: break;
		case SHADER_SAMPLER1DARRAY: break;
		case SHADER_SAMPLER2DARRAY: break;
		case SHADER_SAMPLER1DARRAYSHADOW: break;
		case SHADER_SAMPLER2DARRAYSHADOW: break;
		case SHADER_SAMPLERBUFFER: break;
		case SHADER_ISAMPLER1D: break;
		case SHADER_ISAMPLER2D: break;
		case SHADER_ISAMPLER3D: break;
		case SHADER_ISAMPLERCUBE: break;
		case SHADER_ISAMPLER1DARRAY: break;
		case SHADER_ISAMPLER2DARRAY: break;
		case SHADER_USAMPLER1D: break;
		case SHADER_USAMPLER2D: break;
		case SHADER_USAMPLER3D: break;
		case SHADER_USAMPLERCUBE: break;
		case SHADER_USAMPLER1DARRAY: break;
		case SHADER_USAMPLER2DARRAY: break;
		case SHADER_UNIFORM_BUFFER: delete static_cast<ShaderUBO *>(data); break;
		default: break;
	}

	_variableData[index].data = 0;
}

void ShaderMaterial::useIncrement() {
	++_usageCount;
}

void ShaderMaterial::useDecrement() {
	if (_usageCount) {
		--_usageCount;
	}
}

uint32 ShaderMaterial::useCount() const {
	return _usageCount;
}

} // End of namespace Shader

} // End of namespace Graphics
