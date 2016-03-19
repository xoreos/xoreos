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
 *  Shader runtime builder.
 *
 *  This file is used for construction of shaders based upon an
 *  input configuration. The purpose is to use one large source
 *  string, but only enable those components selected by the
 *  parameter configuration.
 */

#ifndef GRAPHICS_SHADER_SHADERBUILDER_H
#define GRAPHICS_SHADER_SHADERBUILDER_H

#include <vector>
#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"
#include "src/common/matrix.h"

namespace Graphics {

namespace Shader {

class ShaderBuilder {
public:

	enum {
		ENV_CUBE_PRE    = 0x0001,
		ENV_SPHERE_PRE  = 0x0002,
		COLOUR          = 0x0004,
		TEXTURE         = 0x0008,
		ENV_CUBE_POST   = 0x0010,
		ENV_SPHERE_POST = 0x0020
	};

	/**
	 * @brief ShaderBuilder constructor.
	 *
	 * Constructs a shader builder object, which can be done at any time. There
	 * is no reliance upon any other class.
	 */
	ShaderBuilder();

	/**
	 * @brief ShaderBuilder destructor.
	 */
	~ShaderBuilder();

	/**
	 * @brief Generate a vertex shader string based upon the input flags.
	 * @param flags   Input flags to indicated pre-built components to include.
	 * @return  Output of final shader string.
	 */
	Common::UString genVertexShader(uint32_t flags, bool isGL3);

	/**
	 * @brief Generate a name to use for the (vertex) shader, based upon input flags.
	 *
	 * A name is generated predictably based upon the flags used. This allows
	 * for string based naming to determine if a shader has been loaded already
	 * or not. A vertex shader will have ".vert" appended to the end.
	 *
	 * @param flags   Input flags used to generate a name.
	 * @return  Generated name.
	 */
	Common::UString genVertexShaderName(uint32_t flags);

	/**
	 * @brief Generate a fragment shader string based upon the input flags.
	 * @param flags   Input flags to indicated pre-built components to include.
	 * @return  Output of final shader string.
	 */
	Common::UString genFragmentShader(uint32_t flags, bool isGL3);

	/**
	 * @brief Generate a name to use for the (fragment) shader, based upon input flags.
	 *
	 * A name is generated predictably based upon the flags used. This allows
	 * for string based naming to determine if a shader has been loaded already
	 * or not. A fragment shader will have ".frag" appended to the end.
	 *
	 * @param flags   Input flags used to generate a name.
	 * @return  Generated name.
	 */
	Common::UString genFragmentShaderName(uint32_t flags);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERBUILDER_H
