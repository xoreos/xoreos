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

class ShaderBuilder : public Common::Singleton<ShaderBuilder> {
public:

	enum {
		ENV_CUBE,
		ENV_SPHERE,
		COLOUR,
		TEXTURE,
		TEXTURE_LIGHTMAP,
		TEXTURE_BUMPMAP,
		TEXTURE_LIGHTMAP_BUMPMAP,
		FORCE_OPAQUE
	};

	enum {
		BLEND_SRC_ALPHA,
		BLEND_DST_ALPHA,
		BLEND_ZERO,
		BLEND_ONE,
		BLEND_MULTIPLY,   ///< Not really blending, but component-wise multiply.
		BLEND_IGNORED     ///< Blending not applicable to the component.
	};

	struct BuildPass {
		uint32 pass;  ///< Which type of pass (env map, texture, etc).
		uint32 blend; ///< Any type of blending to apply.

		// Definitions below allow this to be used with std::vector
		BuildPass() : pass(0), blend(0) {}
		BuildPass(uint32 p, uint32 b) : pass(p), blend(b) {}
		BuildPass(const BuildPass &bp) : pass(bp.pass), blend(bp.blend) {}
		inline const BuildPass &operator=(const BuildPass &bp) {
			pass = bp.pass;
			blend = bp.blend;
			return *this;
		}
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
	Common::UString genVertexShader(BuildPass *passes, uint32 count, bool isGL3);

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
	Common::UString genVertexShaderName(BuildPass *passes, uint32 count);

	/**
	 * @brief Generate a fragment shader string based upon the input flags.
	 * @param flags   Input flags to indicated pre-built components to include.
	 * @return  Output of final shader string.
	 */
	Common::UString genFragmentShader(BuildPass *passes, uint32 count, bool isGL3);

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
	Common::UString genFragmentShaderName(BuildPass *passes, uint32 count);

	/**
	 * @brief Initialise a name used for a shader object.
	 * @param name  String used to store the target name.
	 */
	void initShaderName(Common::UString &name);
	void addShaderName(Common::UString &name, uint32 passType, uint32 blendType);
	void finaliseShaderNameVertex(Common::UString &name);
	void finaliseShaderNameFragment(Common::UString &name);

	void initVertexShaderString(Common::UString &header, Common::UString &body, bool isGL3);
	void addVertexShaderString(Common::UString &header, Common::UString &body, uint32 passType, uint32 blendType, bool isGL3);
	void finaliseVertexShaderString(Common::UString &combined, Common::UString &header, Common::UString &body, bool isGL3);

	void initFragmentShaderString(Common::UString &header, Common::UString &body, bool isGL3);
	void addFragmentShaderString(Common::UString &header, Common::UString &body, uint32 passType, uint32 blendType, bool isGL3);
	void finaliseFragmentShaderString(Common::UString &combined, Common::UString &header, Common::UString &body, bool isGL3);
};

} // End of namespace Shader

} // End of namespace Graphics

/** Shortcut for accessing the shader manager. */
#define ShaderBuild Graphics::Shader::ShaderBuilder::instance()

#endif // GRAPHICS_SHADER_SHADERBUILDER_H
