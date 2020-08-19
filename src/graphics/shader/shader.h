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
 *
 *  Developer's Notes:
 *  This form of shader handling is absolute overkill for xoreos. It's based
 *  upon code developed for a different project, then plugged into xoreos to
 *  save some wheel reinvention. It is, however, rather flexible and should
 *  be able to cleanly handle any quirks from any of the particular engines.
 *
 *  The basic idea:
 *    * Parse shader text, looking for uniform variables.
 *    * Put all uniform names into an array.
 *    * A compiled program will copy the lists of uniforms from vertex and fragment shaders.
 *    * The compiled program then matches the full list of uniforms with shader program locations.
 *    * A shader program instance (shaderInstance) adds its own pointer information to match the program uniform list.
 *    * Binding a shader program instance (shaderInstance) takes the pointer, and can then bind it directly to a location.
 *  So this is a bit complicated, but easily allows switching the data pointers
 *  within a shader program instance. Textures can be swapped out, colors
 *  modified, etc, and it's all automatically bound correctly. Assigning data
 *  to a variable within a shader program instance is generally done by using
 *  the variable's name directly. This avoids having to know the location of
 *  the variable in the compiled shader, and avoids having to know the index
 *  of the variable within the collected shader uniform list.
 *
 *  Uniform Buffer Objects
 *  UBO's are parsed out and treated as a single variable, similar to a
 *  texture sampler. The user themselves must specify both the UBO id to
 *  attach, and the attachment location.
 *  Index binding within the shader is possible, but discouraged here. It's
 *  an OpenGL >=4.2 feature, and while it would cut down on a little bit of
 *  overhead, the target version for xoreos is OpenGL 3.2 (maybe 3.3).
 *
 *  To anyone thinking that program introspection would replace much of this, well yes and no.
 *  Again, the original idea behind this code involved knowing what uniforms would be
 *  available prior to the program linker stage. Uniforms stripped out during the linker
 *  phase are pretty much ignored when binding a shader here, so that's not a worry.
 *  The methods here also allow a common shader object instance (e.g fragment shader) to
 *  be bound across multiple programs (this is where unused uniforms being ignored comes in).
 */

#ifndef GRAPHICS_SHADER_SHADER_H
#define GRAPHICS_SHADER_SHADER_H

#include <vector>
#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/graphics/texture.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/graphics/shader/shaderbuilder.h"

namespace Graphics {

namespace Shader {

/** Enum of all supported shader variable parsing and automatic binding.
 *  In theory, appropriate gl enums could be used, but they're not as
 *  good for the switch statements used internally.
 */
enum ShaderVariableType {
	SHADER_FLOAT = 0,
	SHADER_VEC2,
	SHADER_VEC3,
	SHADER_VEC4,
	SHADER_INT,
	SHADER_IVEC2,
	SHADER_IVEC3,
	SHADER_IVEC4,
	SHADER_UINT,
	SHADER_UVEC2,
	SHADER_UVEC3,
	SHADER_UVEC4,
	SHADER_BOOL,
	SHADER_BVEC2,
	SHADER_BVEC3,
	SHADER_BVEC4,
	SHADER_MAT2,
	SHADER_MAT2X2,
	SHADER_MAT2X3,
	SHADER_MAT2X4,
	SHADER_MAT3,
	SHADER_MAT3X2,
	SHADER_MAT3X3,
	SHADER_MAT3X4,
	SHADER_MAT4,
	SHADER_MAT4X2,
	SHADER_MAT4X3,
	SHADER_MAT4X4,
	SHADER_SAMPLER1D,
	SHADER_SAMPLER2D,
	SHADER_SAMPLER3D,
	SHADER_SAMPLERCUBE,
	SHADER_SAMPLER1DSHADOW,
	SHADER_SAMPLER2DSHADOW,
	SHADER_SAMPLER1DARRAY,
	SHADER_SAMPLER2DARRAY,
	SHADER_SAMPLER1DARRAYSHADOW,
	SHADER_SAMPLER2DARRAYSHADOW,
	SHADER_SAMPLERBUFFER,
	SHADER_ISAMPLER1D,
	SHADER_ISAMPLER2D,
	SHADER_ISAMPLER3D,
	SHADER_ISAMPLERCUBE,
	SHADER_ISAMPLER1DARRAY,
	SHADER_ISAMPLER2DARRAY,
	SHADER_USAMPLER1D,
	SHADER_USAMPLER2D,
	SHADER_USAMPLER3D,
	SHADER_USAMPLERCUBE,
	SHADER_USAMPLER1DARRAY,
	SHADER_USAMPLER2DARRAY,
	SHADER_UNIFORM_BUFFER,
	SHADER_INVALID
};

enum ShaderType {
	SHADER_VERTEX = 0,
	SHADER_FRAGMENT
};

enum ShaderVertexAttrib {
	VERTEX_LOCATION    = 0,
	VERTEX_NORMAL      = 1,
	VERTEX_COLOR       = 2,
	VERTEX_BONEINDICES = 3,
	VERTEX_BONEWEIGHTS = 4,
	VERTEX_TEXCOORD0   = 5,
	VERTEX_TEXCOORD1   = 6
};

enum ShaderUBOIndex {
	UBO_VIEW_MATRICES = 0,
	UBO_BONE_MATRICES = 1,
	UBO_SCREEN_INFO   = 2
};


struct ShaderUBO {
	GLuint index;
	GLuint glid;
	ShaderUBO() : index(0), glid(0) {}
	ShaderUBO(const ShaderUBO &src) : index(src.index), glid(src.glid) {}
	ShaderUBO(GLuint idx, GLuint gid) : index(idx), glid(gid) {}
	inline const ShaderUBO &operator=(const ShaderUBO &src) { index = src.index; glid = src.glid; return *this; }
};


/** Shader sampler is used to bind a texture to a texture unit. The user
 *  must specify the unit to use.
 */
struct ShaderSampler {
	Texture *texture;
	Graphics::Aurora::TextureHandle handle;
	uint32_t unit;
	ShaderSampler() : texture(0), unit(0) {}
	ShaderSampler(Texture *t, uint32_t u) : texture(t), unit(u) {}
};


class ShaderObject : public GLContainer {
public:
	struct ShaderObjectVariable {
		ShaderVariableType type;
		uint32_t count;  // Number of variables (normally 1, but higher if an array is defined).
		Common::UString name;

		ShaderObjectVariable() : type(SHADER_INVALID), count(0), name() {}
		ShaderObjectVariable(const ShaderObjectVariable &src) : type(src.type), count(src.count), name(src.name) {}
		ShaderObjectVariable(ShaderVariableType t, uint32_t cnt, const Common::UString &n) : type(t), count(cnt), name(n) {}

		inline const ShaderObjectVariable &operator=(const ShaderObjectVariable &src) { type = src.type; count = src.count; name = src.name; return *this; }
	};

	ShaderObject();

	uint32_t usageCount { 0 };
	uint32_t id { 0 };  // ID unique to a shader within its type (i.e unique across vertex shaders, or unique across fragment shaders). 0 is an invalid id.
	GLuint glid { 0 };  // Defaults to zero if not compiled.
	ShaderType type { SHADER_VERTEX };  // Type of shader (typically vertex or fragment).
	Common::UString shaderString;

	std::vector<ShaderObject::ShaderObjectVariable> variablesSelf;
	std::vector<ShaderObject::ShaderObjectVariable> variablesCombined;
	std::vector<ShaderObject *> subObjects;

protected:
	void doRebuild();
	void doDestroy();
};

class ShaderProgram : public GLContainer {
public:
	ShaderProgram();

	ShaderObject *vertexObject { nullptr };  // Vertex shader.
	ShaderObject *fragmentObject { nullptr };  // Fragment shader.
	std::vector<GLint> vertexVariableLocations;
	std::vector<GLint> fragmentVariableLocations;
	uint64_t id { 0 };  // Set to (vertex.id << 32) | fragment.id
	GLuint glid { 0 };
	uint32_t usageCount { 0 };

	void bindAttribute(ShaderVertexAttrib attrib, const Common::UString &name) {
		glBindAttribLocation(glid, (GLuint)(attrib), name.c_str());
	}

	void queue() {
		addToQueue(kQueueNewShader);
	}

protected:
	void doRebuild();
	void doDestroy();
};

/** The shader manager. */
class ShaderManager : public Common::Singleton<ShaderManager> {
public:
	ShaderManager();
	~ShaderManager();

	/** Initialise shader management, including default shader set creation. */
	void init();
	/** Deinitialise shader subsystem. */
	void deinit();

	ShaderObject *getShaderObject(const Common::UString &name, ShaderType type);
	ShaderObject *getShaderObject(const Common::UString &name, const Common::UString &source, ShaderType type);

	void bindShaderVariable(ShaderObject::ShaderObjectVariable &var, GLint loc, const void *data);
	void bindShaderInstance(ShaderProgram *program, const void **vertexVariables, const void **fragmentVariables);

	ShaderProgram *getShaderProgram(ShaderObject *vertexObject, ShaderObject *fragmentObject);
	ShaderProgram *registerShaderProgram(ShaderObject *vertexObject, ShaderObject *fragmentObject);

	void genShaderVariableList(ShaderObject *obj, std::vector<ShaderObject::ShaderObjectVariable> &vars);

	// Takes a string, and returns the appropriate enum representing that type (e.g "vec4" => SHADER_VEC4).
	ShaderVariableType shaderstringToEnum(const Common::UString &stype);

private:
	/** Recursively attaches shader objects to a given program. Called prior to linking. */
	void registerShaderAttachment(GLuint progid, ShaderObject *obj);

	/** Parses a given string, representing a GLSL shader, and extracts uniform variable information from it. */
	void parseShaderVariables(const Common::UString &shaderString, std::vector<ShaderObject::ShaderObjectVariable> &variableList);

public:
	/** Generate GL ids for, and compile a shader object. */
	void genGLShader(ShaderObject *object);

	/** Generate GL id for, and link, a shader program. */
	void genGLProgram(ShaderProgram *program);

private:
	uint32_t _counterVID;
	uint32_t _counterFID;
	std::map<Common::UString, Shader::ShaderObject *> _shaderObjectMap;
	std::vector<Shader::ShaderProgram *> _shaderProgramArray;

	std::recursive_mutex _shaderMutex;
	std::recursive_mutex _programMutex;
};

} // End of namespace Shader

} // End of namespace Graphics

/** Shortcut for accessing the shader manager. */
#define ShaderMan Graphics::Shader::ShaderManager::instance()

#endif // GRAPHICS_SHADER_SHADER_H
