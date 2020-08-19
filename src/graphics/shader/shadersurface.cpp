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

#include "external/glm/gtc/type_ptr.hpp"

#include "src/graphics/graphics.h"

#include "src/graphics/shader/shadersurface.h"

namespace Graphics {

namespace Shader {

#define SHADER_SURFACE_VARIABLE_OWNED (0x00000001)

ShaderSurface::ShaderSurface(Shader::ShaderObject *vertShader, const Common::UString &name) :
		_variableData(),
		_vertShader(vertShader),
		_flags(0),
		_name(name),
		_usageCount(0),
		_objectModelviewIndex(0xFFFFFFFF),
		_textureViewIndex(0xFFFFFFFF),
		_bindPoseIndex(0xFFFFFFFF),
		_boneTransformsIndex(0xFFFFFFFF) {

	vertShader->usageCount++;

	uint32_t varCount = vertShader->variablesCombined.size();
	_variableData.resize(varCount);
	for (uint32_t i = 0; i < varCount; ++i) {
		_variableData[i].flags = 0;
		genSurfaceVar(i);

		if (vertShader->variablesCombined[i].name == "_objectModelviewMatrix") {
			_objectModelviewIndex = i;
		} else if (vertShader->variablesCombined[i].name == "_textureViewMatrix") {
			_textureViewIndex = i;
		} else if (vertShader->variablesCombined[i].name == "_projectionMatrix") {
			setVariableExternal(i, &(GfxMan.getProjectionMatrix()));
		} else if (vertShader->variablesCombined[i].name == "_modelviewMatrix") {
			setVariableExternal(i, &(GfxMan.getModelviewMatrix()));
		} else if (vertShader->variablesCombined[i].name == "_bindPose") {
			_bindPoseIndex = i;
		} else if (vertShader->variablesCombined[i].name == "_boneTransforms") {
			_boneTransformsIndex = i;
		}
	}
}

ShaderSurface::~ShaderSurface() {
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		delSurfaceVar(i);
	}
	// The surface doesn't own UBO's in the _uboArray, and so does not delete them.
}

const Common::UString &ShaderSurface::getName() const {
	return _name;
}

Shader::ShaderObject *ShaderSurface::getVertexShader() const {
	return _vertShader;
}

uint32_t ShaderSurface::getFlags() const {
	return _flags;
}

void ShaderSurface::setFlags(uint32_t flags) {
	_flags = flags;
}

uint32_t ShaderSurface::getVariableCount() const {
	return _vertShader->variablesCombined.size();
	// return _variableData.size(); // Should be equal to the frag shader variable count.
}

Shader::ShaderVariableType ShaderSurface::getVariableType(uint32_t index) const {
	return _vertShader->variablesCombined[index].type;
}

void *ShaderSurface::getVariableData(uint32_t index) const {
	return _variableData[index].data;
}

void *ShaderSurface::getVariableData(const Common::UString &name) const {
	void *rval = 0;

	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			rval = _variableData[i].data;
			break;
		}
	}

	return rval;
}

const Common::UString &ShaderSurface::getVariableName(uint32_t index) const {
	return _vertShader->variablesCombined[index].name;
}

uint32_t ShaderSurface::getVariableFlags(uint32_t index) const {
	return _variableData[index].flags;
}

void ShaderSurface::setVariableExternal(uint32_t index, void *loc) {
	delSurfaceVar(index);
	_variableData[index].data = loc;
}

void ShaderSurface::setVariableExternal(const Common::UString &name, void *loc) {
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			delSurfaceVar(i);
			_variableData[i].data = loc;
			break;
		}
	}
}

void ShaderSurface::setVariableInternal(uint32_t index) {
	genSurfaceVar(index);
}

void ShaderSurface::setVariableInternal(const Common::UString &name) {
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			genSurfaceVar(i);
			break;
		}
	}
}

bool ShaderSurface::isVariableOwned(uint32_t index) const {
	return (_variableData[index].flags & SHADER_SURFACE_VARIABLE_OWNED) ? true : false;
}

bool ShaderSurface::isVariableOwned(const Common::UString &name) const {
	bool rval = false;
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_vertShader->variablesCombined[i].name == name) {
			rval = (_variableData[i].flags & SHADER_SURFACE_VARIABLE_OWNED) ? true : false;
			break;
		}
	}

	return rval;
}

void ShaderSurface::addUBO(uint32_t index, GLuint glid) {
	_uboArray.push_back(Shader::ShaderUBO(index, glid));
}

void ShaderSurface::bindProgram(Shader::ShaderProgram *program) {
	for (uint32_t i = 0; i < _variableData.size(); i++) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[i], program->vertexVariableLocations[i], _variableData[i].data);
	}
}

void ShaderSurface::bindProgram(Shader::ShaderProgram *program, const glm::mat4 *t) {
	for (uint32_t i = 0; i < _variableData.size(); i++) {
		if (_objectModelviewIndex == i) {
			ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[i], program->vertexVariableLocations[i], t);
		} else {
			ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[i], program->vertexVariableLocations[i], _variableData[i].data);
		}
	}
}

void ShaderSurface::bindObjectModelview(Shader::ShaderProgram *program, const glm::mat4 *t) {
	if (_objectModelviewIndex != 0xFFFFFFFF) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[_objectModelviewIndex], program->vertexVariableLocations[_objectModelviewIndex], t);
	}
}

void ShaderSurface::bindTextureView(Shader::ShaderProgram *program, const glm::mat4 *t) {
	if (_textureViewIndex != 0xFFFFFFFF) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[_textureViewIndex], program->vertexVariableLocations[_objectModelviewIndex], t);
	}
}

void ShaderSurface::bindBindPose(Shader::ShaderProgram *program, const glm::mat4 *t) {
	if (_bindPoseIndex != 0xFFFFFFFF) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[_bindPoseIndex], program->vertexVariableLocations[_bindPoseIndex], glm::value_ptr(*t));
	}
}

void ShaderSurface::bindBoneTransforms(Shader::ShaderProgram *program, const float *t) {
	if (_boneTransformsIndex != 0xFFFFFFFF) {
		ShaderMan.bindShaderVariable(program->vertexObject->variablesCombined[_boneTransformsIndex], program->vertexVariableLocations[_boneTransformsIndex], t);
	}
}

void ShaderSurface::bindGLState() {
	if (_flags & SHADER_SURFACE_NOCULL) {
		glDisable(GL_CULL_FACE);
	}

	for (uint32_t i = 0; i < _uboArray.size(); ++i) {
		glBindBufferBase(GL_UNIFORM_BUFFER, _uboArray[i].index, _uboArray[i].glid);
	}
}

void ShaderSurface::unbindGLState() {
	// No need to unbind the uniform buffers really. If a shader uses them, new ones should be bound.

	if (_flags & SHADER_SURFACE_NOCULL) {
		glEnable(GL_CULL_FACE);
	}
}

void ShaderSurface::restoreGLState() {
}

void *ShaderSurface::genSurfaceVar(uint32_t index) {
	if (_variableData[index].flags & SHADER_SURFACE_VARIABLE_OWNED)
		return 0;

	void *rval = 0;
	uint32_t count = _vertShader->variablesCombined[index].count;

	switch (_vertShader->variablesCombined[index].type) {
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
		case SHADER_SAMPLERCUBE: break;
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

	_variableData[index].flags |= SHADER_SURFACE_VARIABLE_OWNED;
	_variableData[index].data = rval;

	return rval;
}

void ShaderSurface::delSurfaceVar(uint32_t index)
{
	if (!(_variableData[index].flags & SHADER_SURFACE_VARIABLE_OWNED))
		return;

	_variableData[index].flags &= ~(SHADER_SURFACE_VARIABLE_OWNED);

	void *data = _variableData[index].data;
	switch (_vertShader->variablesCombined[index].type) {
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
		case SHADER_SAMPLER1D: break;
		case SHADER_SAMPLER2D: break;
		case SHADER_SAMPLER3D: break;
		case SHADER_SAMPLERCUBE: break;
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

void ShaderSurface::useIncrement() {
	++_usageCount;
}

void ShaderSurface::useDecrement() {
	if (_usageCount) {
		--_usageCount;
	}
}

uint32_t ShaderSurface::useCount() const {
	return _usageCount;
}

} // End of namespace Shader

} // End of namespace Graphics
