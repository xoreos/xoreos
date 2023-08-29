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

#include <limits>

#include "src/graphics/lightman.h"

#include "src/graphics/shader/shadermaterial.h"

namespace Graphics {

namespace Shader {

ShaderMaterial::ShaderMaterial(Shader::ShaderObject *fragShader, const Common::UString &name) :
		_variableData(),
		_fragShader(fragShader),
		_flags(0),
		_blendEquationRGB(GL_FUNC_ADD),
		_blendEquationAlpha(GL_FUNC_ADD),
		_blendSrcRGB(GL_SRC_ALPHA),
		_blendSrcAlpha(GL_SRC_ALPHA),
		_blendDstRGB(GL_ONE_MINUS_SRC_ALPHA),
		_blendDstAlpha(GL_ONE_MINUS_SRC_ALPHA),
		_alpha(1.0f),
		_name(name) {
	fragShader->usageCount++;

	/**
	 * For the sake of convenience, having samplers handled externally is a bit of a  pain.
	 * It's much more friendly to just bind a texture handle directly - and so far at least
	 * there's been no need to worry about the sampler unit. So to make a lot of other
	 * code that much more manageable, deal with the samplers in the material and intercept
	 * setting variables if the appropriate type is detected.
	 */
	size_t samplerCount = 0;

	uint32_t varCount = fragShader->variablesCombined.size();
	_variableData.resize(varCount);
	for (uint32_t i = 0; i < varCount; ++i) {
		// First see if some default material variables can be assigned.
		if (fragShader->variablesCombined[i].name == "_lights") {
			this->setVariable(i, LightMan.getLightData());
		} else if (fragShader->variablesCombined[i].name == "_activeLights") {
			this->setVariable(i, LightMan.getActiveLightsData());
		} else if (fragShader->variablesCombined[i].name == "_alpha") {
			this->setVariable(i, &_alpha);
		}

		switch (_fragShader->variablesCombined[i].type) {
			case SHADER_SAMPLER1D:
			case SHADER_SAMPLER2D:
			case SHADER_SAMPLER3D:
			case SHADER_SAMPLERCUBE:
				samplerCount++;
			break;
			default: break;
		}
	}

	_samplers.resize(samplerCount);
	for (size_t i = 0; i < samplerCount; ++i) {
		_samplers[i].unit = i;
	}
}
#if 0
ShaderMaterial::ShaderMaterial(const ShaderMaterial &src) :
	_variableData(),
	_fragShader(src._fragShader),
	_flags(src._flags),
	_blendEquationRGB(src._blendEquationRGB),
	_blendEquationAlpha(src._blendEquationAlpha),
	_blendSrcRGB(src._blendSrcRGB),
	_blendSrcAlpha(src._blendSrcAlpha),
	_blendDstRGB(src._blendDstRGB),
	_blendDstAlpha(src._blendDstAlpha),
	_alpha(src._alpha),
	_name(src._name) {

	_fragShader->usageCount++;

	size_t samplerCount = 0;
	_samplers.resize(src._samplers.size());
	for (size_t i = 0; i < _samplers.size(); ++i) {
		_samplers[i].unit = src._samplers[i].unit;
		_samplers[i].handle = src._samplers[i].handle;
	}

	uint32_t varCount = _fragShader->variablesCombined.size();
	_variableData.resize(varCount);
	for (uint32_t i = 0; i < varCount; ++i) {
		_variableData[i] = src._variableData[i];
		if (_fragShader->variablesCombined[i].name == "_alpha") {
			if (_variableData[i] == &(src._alpha)) {
				this->setVariable(i, &_alpha);
			}
		} else {
			switch (_fragShader->variablesCombined[i].type) {
			case SHADER_SAMPLER1D:
			case SHADER_SAMPLER2D:
			case SHADER_SAMPLER3D:
			case SHADER_SAMPLERCUBE:
				_variableData[i] = &_samplers[samplerCount++];
			break;
			default: break;
			}
		}
	}
}
#endif
ShaderMaterial::~ShaderMaterial() {
	_fragShader->usageCount--;
}

const Common::UString &ShaderMaterial::getName() const {
	return _name;
}

uint32_t ShaderMaterial::getFlags() const {
	return _flags;
}

void ShaderMaterial::setFlags(uint32_t flags) {
	_flags = flags;
}

void ShaderMaterial::setBlendEquationRGB(GLenum equation) {
	_blendEquationRGB = equation;
}

void ShaderMaterial::setBlendEquationAlpha(GLenum equation) {
	_blendEquationAlpha = equation;
}

void ShaderMaterial::setBlendSrcRGB(GLenum mode) {
	_blendSrcRGB = mode;
}

void ShaderMaterial::setBlendSrcAlpha(GLenum mode) {
	_blendSrcAlpha = mode;
}

void ShaderMaterial::setBlendDstRGB(GLenum mode) {
	_blendDstRGB = mode;
}

void ShaderMaterial::setBlendDstAlpha(GLenum mode) {
	_blendDstAlpha = mode;
}

Shader::ShaderObject *ShaderMaterial::getFragmentShader() const {
	return _fragShader;
}

uint32_t ShaderMaterial::getVariableCount() const {
	return _fragShader->variablesCombined.size();
	// return _variableData.size(); // Should be equal to the frag shader variable count.
}

Shader::ShaderVariableType ShaderMaterial::getVariableType(uint32_t index) const {
	return _fragShader->variablesCombined[index].type;
}

const void *ShaderMaterial::getVariableData(uint32_t index) const {
	return _variableData[index];
}

const void *ShaderMaterial::getVariableData(const Common::UString &name) const {
	const void *rval = 0;

	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			rval = _variableData[i];
			break;
		}
	}

	return rval;
}

const Common::UString &ShaderMaterial::getVariableName(uint32_t index) const {
	return _fragShader->variablesCombined[index].name;
}


void ShaderMaterial::setVariable(uint32_t index, const void *loc) {
	_variableData[index] = loc;
}

void ShaderMaterial::setVariable(const Common::UString &name, const void *loc) {
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			_variableData[i] = loc;
			break;
		}
	}
}

void ShaderMaterial::setTexture(const Common::UString &name, Graphics::Aurora::TextureHandle handle) {
	uint32_t sindex = 0;
	for (uint32_t i = 0; i < _variableData.size(); ++i) {
		if (_fragShader->variablesCombined[i].name == name) {
			_samplers[sindex].handle = handle;
			_variableData[i] = &_samplers[sindex];
			break;
		} else {
			switch (_fragShader->variablesCombined[i].type) {
			case SHADER_SAMPLER1D:
			case SHADER_SAMPLER2D:
			case SHADER_SAMPLER3D:
			case SHADER_SAMPLERCUBE:
				sindex++;
				break;
			default: break;
			}
		}
	}
}


void ShaderMaterial::bindProgram(Shader::ShaderProgram *program) {
	for (uint32_t i = 0; i < _variableData.size(); i++) {
		ShaderMan.bindShaderVariable(program->fragmentObject->variablesCombined[i], program->fragmentVariableLocations[i], _variableData[i]);
	}
}

void ShaderMaterial::bindProgram(Shader::ShaderProgram *program, float alpha) {
	_alpha = alpha;
	this->bindProgram(program);
}


void ShaderMaterial::bindGLState() {
	if (_flags & ShaderMaterial::MATERIAL_CUSTOM_BLEND) {
		glBlendEquationSeparate(_blendEquationRGB, _blendEquationAlpha);
		glBlendFuncSeparate(_blendSrcRGB, _blendDstRGB, _blendSrcAlpha, _blendDstAlpha);
	}
}

void ShaderMaterial::unbindGLState() {
	if (_flags & ShaderMaterial::MATERIAL_CUSTOM_BLEND) {
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void ShaderMaterial::restoreGLState() {
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

} // End of namespace Shader

} // End of namespace Graphics
