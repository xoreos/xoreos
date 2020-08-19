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
 *  The global shader manager.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "src/common/util.h"

#include "src/graphics/graphics.h"

#include "src/graphics/shader/shader.h"
#include "src/graphics/shader/shaderbuilder.h"

#include "src/graphics/aurora/texture.h"

/*--------------------------------------------------------------------*/

DECLARE_SINGLETON(Graphics::Shader::ShaderManager)

namespace Graphics {

namespace Shader {

/*--------------------------------------------------------------------*/
ShaderObject::ShaderObject() : GLContainer() {
}

void ShaderObject::doRebuild() {
}

void ShaderObject::doDestroy() {
	if (this->glid != 0) {
		glDeleteShader(this->glid);
		this->glid = 0;
	}
}
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
ShaderProgram::ShaderProgram() : GLContainer() {
}

void ShaderProgram::doRebuild() {
	ShaderMan.genGLProgram(this);
}

void ShaderProgram::doDestroy() {
	if (this->glid != 0) {
		glDeleteProgram(this->glid);
		this->glid = 0;
	}
}
/*--------------------------------------------------------------------*/


ShaderManager::ShaderManager() : _counterVID(1), _counterFID(1) {
}

ShaderManager::~ShaderManager() {
	deinit();
}

void ShaderManager::init() {
	//_isGL3 = isGL3;  // pull this from GfxMan
	status("Initialising shaders...");

	ShaderObject *vObj;
	ShaderObject *fObj;

	Graphics::Shader::ShaderDescriptor cripter;
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_POSITION0);
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV0);
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_COLOUR);
	cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
	cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                Graphics::Shader::ShaderDescriptor::INPUT_UV0,
	                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);
	cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
	                Graphics::Shader::ShaderDescriptor::BLEND_ONE);
	cripter.addPass(Graphics::Shader::ShaderDescriptor::X_COLOUR,
	                Graphics::Shader::ShaderDescriptor::BLEND_MULTIPLY);

	Common::UString vertexStringFinal;
	Common::UString fragmentStringFinal;
	cripter.build(GfxMan.isGL3(), vertexStringFinal, fragmentStringFinal);
	vObj = ShaderMan.getShaderObject("default/text.vert", vertexStringFinal, Shader::SHADER_VERTEX);
	fObj = ShaderMan.getShaderObject("default/text.frag", fragmentStringFinal, Shader::SHADER_FRAGMENT);
	registerShaderProgram(vObj, fObj);

	cripter.clear();
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_POSITION0);
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV0);
	cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
	cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                Graphics::Shader::ShaderDescriptor::INPUT_UV0,
	                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);
	cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
	                Graphics::Shader::ShaderDescriptor::BLEND_ONE);

	vertexStringFinal.clear();
	fragmentStringFinal.clear();
	cripter.build(GfxMan.isGL3(), vertexStringFinal, fragmentStringFinal);
	vObj = ShaderMan.getShaderObject("default/texture.vert", vertexStringFinal, Shader::SHADER_VERTEX);
	fObj = ShaderMan.getShaderObject("default/texture.frag", fragmentStringFinal, Shader::SHADER_FRAGMENT);
	registerShaderProgram(vObj, fObj);

	cripter.clear();
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_POSITION0);
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV0_MATRIX);
	cripter.declareSampler(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                       Graphics::Shader::ShaderDescriptor::SAMPLER_2D);
	cripter.connect(Graphics::Shader::ShaderDescriptor::SAMPLER_TEXTURE_0,
	                Graphics::Shader::ShaderDescriptor::INPUT_UV0_MATRIX,
	                Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE);
	cripter.addPass(Graphics::Shader::ShaderDescriptor::TEXTURE_DIFFUSE,
	                Graphics::Shader::ShaderDescriptor::BLEND_ONE);

	vertexStringFinal.clear();
	fragmentStringFinal.clear();
	cripter.build(GfxMan.isGL3(), vertexStringFinal, fragmentStringFinal);
	vObj = ShaderMan.getShaderObject("default/textureMatrix.vert", vertexStringFinal, Shader::SHADER_VERTEX);
	fObj = ShaderMan.getShaderObject("default/texture.frag", Shader::SHADER_FRAGMENT);
	registerShaderProgram(vObj, fObj);

	cripter.clear();
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_POSITION0);
	cripter.declareInput(Graphics::Shader::ShaderDescriptor::INPUT_UV0);
	cripter.declareUniform(Graphics::Shader::ShaderDescriptor::UNIFORM_F_COLOUR);
	cripter.addPass(Graphics::Shader::ShaderDescriptor::COLOUR,
	                Graphics::Shader::ShaderDescriptor::BLEND_ONE);

	vertexStringFinal.clear();
	fragmentStringFinal.clear();
	cripter.build(GfxMan.isGL3(), vertexStringFinal, fragmentStringFinal);
	vObj = ShaderMan.getShaderObject("default/default.vert", vertexStringFinal, Shader::SHADER_VERTEX);
	fObj = ShaderMan.getShaderObject("default/colour.frag", fragmentStringFinal, Shader::SHADER_FRAGMENT);
	registerShaderProgram(vObj, fObj);
}

void ShaderManager::deinit() {
	status("Cleaning up shaders...");
	glUseProgram(0);

	for (uint32_t i = 0; i < _shaderProgramArray.size(); ++i) {
		glDeleteProgram(_shaderProgramArray[i]->glid);
		delete _shaderProgramArray[i];
	}
	_shaderProgramArray.clear();

	for (std::map<Common::UString, Shader::ShaderObject *>::iterator iter = _shaderObjectMap.begin(); iter != _shaderObjectMap.end(); ++iter) {
		if (iter->second->glid) {
			glDeleteShader(iter->second->glid);
		}
		delete iter->second;
	}
	_shaderObjectMap.clear();
}

ShaderObject *ShaderManager::getShaderObject(const Common::UString &name, ShaderType UNUSED(type)) {
	// In future, this should use Common::ReadFile to load file "name" into a string, and compile a shader object from there.
	Common::UString shaderString;
	//ShaderObject *shaderObject = 0;
	//ShaderObject *shaderObject = (ShaderObject *)(_shaderObjectMap[filename.c_str()]);
	std::map<Common::UString, Shader::ShaderObject *>::iterator it = _shaderObjectMap.find(name);
	if (it != _shaderObjectMap.end()) {
		return it->second;
	} else {
		return 0;
	}
}

ShaderObject *ShaderManager::getShaderObject(const Common::UString &name, const Common::UString &source, ShaderType type) {
	_shaderMutex.lock();
	ShaderObject *shaderObject = 0;
	//ShaderObject *shaderObject = (ShaderObject *)(_shaderObjectMap[filename.c_str()]);
	std::map<Common::UString, Shader::ShaderObject *>::iterator it = _shaderObjectMap.find(name);
	if (it != _shaderObjectMap.end()) {
		_shaderMutex.unlock();
		return it->second;
	}

	shaderObject = new ShaderObject;
	shaderObject->type = type;
	shaderObject->glid = 0;
	shaderObject->shaderString = source;

	status("shader %s loaded", name.c_str());

	_shaderObjectMap.insert(std::pair<Common::UString, ShaderObject *>(name, shaderObject));
	_shaderMutex.unlock();

	parseShaderVariables(source, shaderObject->variablesSelf);
	genShaderVariableList(shaderObject, shaderObject->variablesCombined);
	if (shaderObject->type == SHADER_VERTEX) {
		shaderObject->id = _counterVID++; // Post decrement intentional.
	} else {
		shaderObject->id = _counterFID++; // Post decrement intentional.
	}

	return shaderObject;
}

void ShaderManager::bindShaderVariable(ShaderObject::ShaderObjectVariable &var, GLint loc, const void *data) {
	switch (var.type) {
		case SHADER_FLOAT: glUniform1fv(loc, var.count, static_cast<const float *>(data)); break;
		case SHADER_VEC2:  glUniform2fv(loc, var.count, static_cast<const float *>(data)); break;
		case SHADER_VEC3:  glUniform3fv(loc, var.count, static_cast<const float *>(data)); break;
		case SHADER_VEC4:  glUniform4fv(loc, var.count, static_cast<const float *>(data)); break;
		case SHADER_INT:   glUniform1iv(loc, var.count, static_cast<const GLint *>(data)); break;
		case SHADER_IVEC2: glUniform2iv(loc, var.count, static_cast<const GLint *>(data)); break;
		case SHADER_IVEC3: glUniform3iv(loc, var.count, static_cast<const GLint *>(data)); break;
		case SHADER_IVEC4: glUniform4iv(loc, var.count, static_cast<const GLint *>(data)); break;
// Commented out those not really supported by older GLSL versions.
//		case SHADER_UINT:  glUniform1uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_UVEC2: glUniform2uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_UVEC3: glUniform3uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_UVEC4: glUniform4uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_BOOL:  glUniform1uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_BVEC2: glUniform2uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_BVEC3: glUniform3uiv(loc, var.count, static_cast<GLuint *>(data)); break;
//		case SHADER_BVEC4: glUniform4uiv(loc, var.count, static_cast<GLuint *>(data)); break;
		case SHADER_MAT2:  glUniformMatrix2fv(loc, var.count, 0, static_cast<const float *>(data)); break;
		case SHADER_MAT3:  glUniformMatrix3fv(loc, var.count, 0, static_cast<const float *>(data)); break;
		case SHADER_MAT4:  glUniformMatrix4fv(loc, var.count, 0, static_cast<const float *>(data)); break;
		case SHADER_SAMPLER1D:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_1D, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER2D:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_2D, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER3D:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_3D, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLERCUBE:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER1DSHADOW:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_1D_ARRAY, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER2DSHADOW:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_2D, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER1DARRAY:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_1D_ARRAY, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER2DARRAY:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER1DARRAYSHADOW:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_1D_ARRAY, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLER2DARRAYSHADOW:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
		case SHADER_SAMPLERBUFFER:
			glUniform1i(loc, static_cast<const ShaderSampler *>(data)->unit);
			glActiveTexture(GL_TEXTURE0 + static_cast<const ShaderSampler *>(data)->unit);
			glBindTexture(GL_TEXTURE_BUFFER, static_cast<const ShaderSampler *>(data)->handle.getTexture().getID());
			break;
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
		case SHADER_UNIFORM_BUFFER:
			glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<const ShaderUBO *>(data)->index, static_cast<const ShaderUBO *>(data)->glid);
			break;
		default: break;
	}
}

void ShaderManager::bindShaderInstance(ShaderProgram *prog, const void **vertexVariables, const void **fragmentVariables) {
	glUseProgram(prog->glid);
	for (uint32_t i = 0; i < prog->vertexObject->variablesCombined.size(); ++i) {
		bindShaderVariable(prog->vertexObject->variablesCombined[i], prog->vertexVariableLocations[i], vertexVariables[i]);
	}

	for (uint32_t i = 0; i < prog->fragmentObject->variablesCombined.size(); ++i) {
		bindShaderVariable(prog->fragmentObject->variablesCombined[i], prog->fragmentVariableLocations[i], fragmentVariables[i]);
	}
}

ShaderProgram *ShaderManager::getShaderProgram(ShaderObject *vertexObject, ShaderObject *fragmentObject) {
	_programMutex.lock();
	if (_shaderProgramArray.size() == 0) {
		_programMutex.unlock();
		return NULL;
	}

	uint32_t search_high = _shaderProgramArray.size() - 1;  // -1 is ok here, as a default shader will exist.
	uint32_t search_low = 0;
	uint32_t search_mid = _shaderProgramArray.size() >> 1;
	uint64_t key = (((uint64_t)(vertexObject->id)) << 32) | (uint64_t)(fragmentObject->id);
	ShaderProgram *rvalue = 0;  // Default to nothing found.

	while (search_high >= search_low) {
		if (_shaderProgramArray[search_mid]->id == key) {
			// Found the program.
			search_low = search_high+1;  // Break the loop.
			rvalue = _shaderProgramArray[search_mid];
		} else if (_shaderProgramArray[search_mid]->id < key) {
			search_low = search_mid+1;
			search_mid = (search_high + search_low) >> 1;  // This will allow search_mid to gravitate towards the highest point.
		} else {
			// The key is higher than the mid point, so search higher in the array.
			search_high = search_mid-1;
			search_mid = (search_high + search_low) >> 1;  // The will help search_mid to gravitate towards the lowest point.
		}
	}
	_programMutex.unlock();
	return rvalue;
}

void ShaderManager::registerShaderAttachment(GLuint progid, ShaderObject *obj) {
	glAttachShader(progid, obj->glid);
	for (uint32_t i = 0; i < obj->subObjects.size(); ++i) {
		registerShaderAttachment(progid, obj->subObjects[i]);
	}
}

ShaderProgram *ShaderManager::registerShaderProgram(ShaderObject *vertexObject, ShaderObject *fragmentObject) {
	ShaderProgram *program = getShaderProgram(vertexObject, fragmentObject);
	if (program != 0)
		return program;  // Shader already exists.
	else if (vertexObject->id == 0 || fragmentObject->id == 0)
		return 0;  // One id or the other is invalid - shader program can not be created.

	vertexObject->usageCount++;
	fragmentObject->usageCount++;

	_programMutex.lock();
	uint64_t key = (((uint64_t)(vertexObject->id)) << 32) | (uint64_t)(fragmentObject->id);
	uint32_t searchIndex = 0;
	while ((searchIndex < _shaderProgramArray.size()) && (_shaderProgramArray[searchIndex]->id < key)) {
		++searchIndex;
	}

	program = new ShaderProgram;
	_shaderProgramArray.push_back(program);

	for (unsigned int i = (_shaderProgramArray.size() - 1); i > searchIndex; i--) {
		_shaderProgramArray[i] = _shaderProgramArray[i - 1];
	}
	_shaderProgramArray[searchIndex] = program;
	_programMutex.unlock();

	program->id = key;
	program->glid = 0;
	program->vertexObject = vertexObject;
	program->fragmentObject = fragmentObject;

	program->queue();
	return program;
}

void ShaderManager::genShaderVariableList(ShaderObject *obj, std::vector<ShaderObject::ShaderObjectVariable> &vars) {
	if (!obj) {
		return;
	}

	std::vector<ShaderObject *> shaderObjectStack;
	std::map<Common::UString, uint32_t> shaderNameMap;

	do {
		for (uint32_t i = 0; i < obj->subObjects.size(); ++i) {
			shaderObjectStack.push_back(obj->subObjects[i]);
		}

		for (uint32_t i = 0; i < obj->variablesSelf.size(); ++i) {
			if (shaderNameMap.find(obj->variablesSelf[i].name) == shaderNameMap.end()) {
				shaderNameMap.insert(std::pair<Common::UString, uint32_t>(obj->variablesSelf[i].name, i));
				vars.push_back(obj->variablesSelf[i]);
			}
		}
		if (shaderObjectStack.size() > 0) {
			obj = shaderObjectStack.back();
			shaderObjectStack.pop_back();
		} else {
			obj = 0;
		}
	} while (obj);

	vars.resize(vars.size());  // C++11 function is vars.shrink_to_fit().
}

void ShaderManager::parseShaderVariables(const Common::UString &shaderString, std::vector<ShaderObject::ShaderObjectVariable> &varList) {
	// todo: Better source parsing is required, rather than relying on newline / whitespace delimiting.
	// #version in shader source needs a newline, but after that does not, so perhaps parse on ';' instead.

	std::string searchString = shaderString.c_str();
	std::string sword, snum;
	std::string::size_type pos, posEnd;
	std::string::size_type posi, posEndi;
	unsigned int parseState = 0;
	unsigned int num;  // Used if an array subscript is found.
	ShaderVariableType varIdentityCrisis = SHADER_INVALID;

	pos = searchString.find_first_not_of("\t\r\n\f\v ", 0);
	posEnd = searchString.find_first_of("\t\r\n\f\v ", pos);
	while (pos != std::string::npos && posEnd != std::string::npos) {
		sword = searchString.substr(pos, posEnd - pos);
		switch (parseState) {
			case 0:
			if (sword.find("//") != std::string::npos) {
				// Move to the end of the line, and continue.
				posEnd = searchString.find_first_of("\n", pos);
			} else if (sword.find("/*") != std::string::npos) {
				// Find the next "*/" after this.
				posEnd = searchString.find("*/", pos);
				if (posEnd != std::string::npos)
					posEnd += 2;  // Go to after the comment end.
			} else if (sword == "uniform") {
				// Found the start of a uniform declaration. Go to the next parsing state.
				parseState = 1;
			}
			pos = searchString.find_first_not_of("\t\r\n\f\v ", posEnd);
			posEnd = searchString.find_first_of("\t\r\n\f\v ", pos);
			break;

			case 1:
			// sword should be the uniform type.
			varIdentityCrisis = shaderstringToEnum(Common::UString(sword));
			//printf("%s (%u) ", sword.c_str(), varIdentityCrisis);
			if (varIdentityCrisis == SHADER_INVALID) {
				// Uncomment below if uniform buffers are to be included. Right now, the shader should use the binding layout instead.
				/*
				varIdentityCrisis = SHADER_UNIFORM_BUFFER;

				posEndi = sword.find_first_of("{");
				if (posEndi != std::string::npos)
					sword.erase(sword.length(), posEndi - sword.length());
				sword.erase(sword.find_last_not_of(whitespace)+1);

				status("unknown shader variable type encountered, assuming uniform buffer of name: %s\n", sword.c_str());

				new_var = new ShaderVariable;
				new_var->count = 1;
				new_var->type = SHADER_UNIFORM_BUFFER;
				new_var->location = 0;
				strncpy(new_var->name, sword.c_str(), 51);
				var_list->append(new_var);
				*/
				parseState = 0;
			} else {
				parseState = 2;
				pos = searchString.find_first_not_of("\t\r\n\f\v ", posEnd);
				posEnd = searchString.find_first_of(",;", pos);  // Only check for , or ; to determine the next variable.
			}
			break;

			case 2:
			posEndi = sword.length();
			posi = sword.find('=');
			if (posi != std::string::npos) {
				sword.erase(posi, posEndi - posi);  // Check for and remove any initialisers.
			}

			num = 1;  // Default to a single element.
			posi = sword.find('[');
			if (posi != std::string::npos) {
				posEndi = sword.find(']');  // Assume for now that it exists.
				snum = sword.substr(posi+1, posEndi - posi - 1);
				num	= atoi(snum.c_str());//std::stoi(snum);//atoi(snum.c_str());
				sword.erase(posi, sword.length() - posi);
			}
			// sword may contain trailing whitespaces, so remove them.
			sword.erase(sword.find_last_not_of("\t\r\n\f\v ")+1);

			// sword should now be the name of a variable, of type varIdentityCrisis, numbering num.
			varList.push_back(ShaderObject::ShaderObjectVariable(varIdentityCrisis, num, sword));

			if (searchString.at(posEnd) == ';') {
				++posEnd;  // As the first non-whitespace character is looked for next, increment posEnd to avoid picking up on the ';'.
				// End of current uniform, reset parse state.
				parseState = 0;
				pos = searchString.find_first_not_of("\t\r\n\f\v ", posEnd);
				posEnd = searchString.find_first_of("\t\r\n\f\v ", pos);
			} else {
				++posEnd;  // As the first non-whitespace character is looked for next, increment posEnd to avoid picking up on the ','.
				pos = searchString.find_first_not_of("\t\r\n\f\v ", posEnd);
				posEnd = searchString.find_first_of("\t\r\n\v\f ,;", pos);
			}
			break;

			default: parseState = 0; break;
		}
	}

	varList.resize(varList.size());
}

void ShaderManager::genGLShader(ShaderObject *object) {
	if (object->glid != 0) {
		return;  // Shader object already initialised.
	}

	object->glid = glCreateShader(object->type == SHADER_VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	const char *text = object->shaderString.c_str();
	glShaderSource(object->glid, 1, (const GLchar **)(&text), 0);
	glCompileShader(object->glid);

	GLint gl_status;
	glGetShaderiv(object->glid, GL_COMPILE_STATUS, &gl_status);
	if (gl_status == GL_FALSE) {
		GLsizei logolength;
		char logorama[4096];
		glGetShaderInfoLog(object->glid, 4095, &logolength, logorama);
		error("Shader compile failure! Driver output: %s", logorama);
	}
}

void ShaderManager::genGLProgram(ShaderProgram *program) {
	if (program->glid != 0) {
		return;
	}

	GLuint glid = glCreateProgram();
	ShaderObject *vertexObject = program->vertexObject;
	ShaderObject *fragmentObject = program->fragmentObject;

	// These functions will do nothing if the object has already been initialised.
	genGLShader(vertexObject);
	genGLShader(fragmentObject);

	registerShaderAttachment(glid, vertexObject);
	registerShaderAttachment(glid, fragmentObject);

	// GL2.1 supports attribute binding, but apparently nvidia drivers can have a problem with aliasing to inbuilt functions.
	// So only bind attribute locations for >= GL3.x, with 2.x using builtin.
	if (GfxMan.isGL3()) {
		// Attempt to bind some common attributes. Note that it doesn't hurt if the attributes don't exist in the shader program.
		glBindAttribLocation(glid, (GLuint)(VERTEX_LOCATION), "inputPosition0");
		glBindAttribLocation(glid, (GLuint)(VERTEX_TEXCOORD0), "inputUV0");
		glBindAttribLocation(glid, (GLuint)(VERTEX_NORMAL), "inputNormal0");
		glBindAttribLocation(glid, (GLuint)(VERTEX_TEXCOORD1), "inputUV1");
		glBindAttribLocation(glid, (GLuint)(VERTEX_COLOR), "inputColour");
	}

	glBindAttribLocation(glid, (GLuint)(VERTEX_BONEINDICES), "inputBoneIndices");
	glBindAttribLocation(glid, (GLuint)(VERTEX_BONEWEIGHTS), "inputBoneWeights");

	glLinkProgram(glid);

	GLint linkStatus;
	glGetProgramiv(glid, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		GLsizei logolength;
		char logorama[4096];
		glGetProgramInfoLog(glid, 4095, &logolength, logorama);
		error("Shader link failure! Driver output: %s", logorama);

		glDeleteProgram(glid);

		return;
	}

	program->glid = glid;

	for (uint32_t i = 0; i < vertexObject->variablesCombined.size(); ++i) {
		GLint location;
		if (vertexObject->variablesCombined[i].type != SHADER_UNIFORM_BUFFER)
			location = glGetUniformLocation(glid, vertexObject->variablesCombined[i].name.c_str());
		else
			location = glGetUniformBlockIndex(glid, vertexObject->variablesCombined[i].name.c_str());
		program->vertexVariableLocations.push_back(location);
	}

	for (uint32_t i = 0; i < fragmentObject->variablesCombined.size(); ++i) {
		GLint location;
		if (fragmentObject->variablesCombined[i].type != SHADER_UNIFORM_BUFFER)
			location = glGetUniformLocation(glid, fragmentObject->variablesCombined[i].name.c_str());
		else
			location = glGetUniformBlockIndex(glid, fragmentObject->variablesCombined[i].name.c_str());
		program->fragmentVariableLocations.push_back(location);
	}

	if (GfxMan.isGL3()) {
		// Bind some common UBO points.
		GLuint ubo_index;
		ubo_index = glGetUniformBlockIndex(glid, "engine_ubo_view");
		if (ubo_index != GL_INVALID_INDEX) {
			glUniformBlockBinding(glid, ubo_index, UBO_VIEW_MATRICES);
		}
		ubo_index = glGetUniformBlockIndex(glid, "engine_ubo_screen");
		if (ubo_index != GL_INVALID_INDEX) {
			glUniformBlockBinding(glid, ubo_index, UBO_SCREEN_INFO);
		}
		ubo_index = glGetUniformBlockIndex(glid, "boneBlock");
		if (ubo_index != GL_INVALID_INDEX) {
			glUniformBlockBinding(glid, ubo_index, UBO_BONE_MATRICES);
		}
	}

	glUseProgram(0);
}

static const char *shaderTypeChararray[] = {
	"float",
	"vec2",
	"vec3",
	"vec4",
	"int",
	"ivec2",
	"ivec3",
	"ivec4",
	"uint",
	"uvec2",
	"uvec3",
	"uvec4",
	"bool",
	"bvec2",
	"bvec3",
	"bvec4",
	"mat2",
	"mat2x2",
	"mat2x3",
	"mat2x4",
	"mat3",
	"mat3x2",
	"mat3x3",
	"mat3x4",
	"mat4",
	"mat4x2",
	"mat4x3",
	"mat4x4",
	"sampler1D",
	"sampler2D",
	"sampler3D",
	"samplerCube",
	"sampler1DShadow",
	"sampler2DShadow",
	"sampler1DArray",
	"sampler2DArray",
	"sampler1DArrayShadow",
	"sampler2DArrayShadow",
	"samplerBuffer",
	"isampler1D",
	"isampler2D",
	"isampler3D",
	"isamplerCube",
	"isampler1DArray",
	"isampler2DArray",
	"usampler1D",
	"usampler2D",
	"usampler3D",
	"usamplerCube",
	"usampler1DArray",
	"usampler2DArray",
	"uniformBufferInvalid"
};

ShaderVariableType ShaderManager::shaderstringToEnum(const Common::UString &stringType) {
	// Yes, using a hash table would make this an awful lot faster.
	ShaderVariableType varType = SHADER_INVALID;
	for (unsigned int i = 0; i < SHADER_INVALID; ++i) {
		if (stringType == shaderTypeChararray[i]) {
			varType = (ShaderVariableType)i;
			break;
		}
	}
	return varType;
}

} // End of namespace Shader

} // End of namespace Graphics
