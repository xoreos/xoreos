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

#include "glm/mat4x4.hpp"

#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

namespace Graphics {

namespace Shader {

class ShaderDescriptor
{
public:
	///< Vertex shader input data. Each should have a corresponding output.
	enum Input {
		INPUT_POSITION0,
		INPUT_POSITION1,
		INPUT_POSITION2,
		INPUT_POSITION3,
		INPUT_NORMAL0,
		INPUT_NORMAL1,
		INPUT_NORMAL2,
		INPUT_NORMAL3,
		INPUT_UV0,
		INPUT_UV1,
		INPUT_UV0_MATRIX, ///< Same as INPUT_UV0, but specifies input matrix too.
		INPUT_UV1_MATRIX, ///< Same as INPUT_UV1, but specifies input matrix too.
		INPUT_UV_CUBE,    ///< Not strictly speaking an input, but generated for an output.
		INPUT_UV_SPHERE,  ///< Not strictly speaking an input, but generated for an output.
		INPUT_COLOUR
	};

	///< Sampler definitions. Limit to the OpenGL minimum requirement.
	enum Sampler {
		SAMPLER_TEXTURE_0,
		SAMPLER_TEXTURE_1,
		SAMPLER_TEXTURE_2,
		SAMPLER_TEXTURE_3,
		SAMPLER_TEXTURE_4,
		SAMPLER_TEXTURE_5,
		SAMPLER_TEXTURE_6,
		SAMPLER_TEXTURE_7,
		SAMPLER_TEXTURE_NONE
	};

	///< Type of sampler. Obviously.
	enum SamplerType {
		SAMPLER_1D,
		SAMPLER_2D,
		SAMPLER_3D,
		SAMPLER_CUBE
	};

	///< Uniforms included in the final shader.
	enum Uniform {
		UNIFORM_V_OBJECT_MODELVIEW_MATRIX,
		UNIFORM_V_PROJECTION_MATRIX,
		UNIFOM_V_MODELVIEW_MATRIX,
		UNIFORM_F_ALPHA,
		UNIFORM_F_COLOUR
	};

	///< Fragment shader action. Multiple actions can be used in a single shader.
	enum Action {
		ENV_CUBE,
		ENV_SPHERE,
		COLOUR,           ///< Uniform variable colour information.
		X_COLOUR,         ///< Per-vertex colour information.
		TEXTURE_DIFFUSE,
		TEXTURE_LIGHTMAP,
		TEXTURE_BUMPMAP,
		FORCE_OPAQUE,
		NOOP
	};

	///< Blending applied to each Action.
	enum Blend {
		BLEND_SRC_ALPHA,
		BLEND_DST_ALPHA,
		BLEND_ZERO,
		BLEND_ONE,
		BLEND_MULTIPLY,   ///< Not really blending, but component-wise multiply.
		BLEND_IGNORED     ///< Blending not applicable to the component.
	};

	ShaderDescriptor();

	~ShaderDescriptor();

	void declareInput(ShaderDescriptor::Input input);

	void declareSampler(ShaderDescriptor::Sampler sampler, ShaderDescriptor::SamplerType type);

	void declareUniform(ShaderDescriptor::Uniform uniform);

	/**
	 * @brief Connect an input to a sampler and an action.
	 * @param sampler  Sampler to associate with the input and the action. Use SAMPLER_TEXTURE_NONE if this is to be ignored.
	 * @param input    Associate a sampler with an input to be used for UVW coordinates.
	 * @param action   Associate the connection to an action to be performed with them.
	 */
	void connect(ShaderDescriptor::Sampler sampler,
	             ShaderDescriptor::Input input,
	             ShaderDescriptor::Action action);

	void addPass(ShaderDescriptor::Action action, ShaderDescriptor::Blend blend);

	void build(bool isGL3, Common::UString &v_string, Common::UString &f_string);

	/**
	 * @brief Clear shader descriptor information. Reset everything to default state.
	 */
	void clear();

	/**
	 * @brief Generate a name to asscoiate with the current description. Does not require building first.
	 * @param n_string String name of description.
	 */
	void genName(Common::UString &n_string);

private:
	// Input descriptors.
	// Sampler descriptors.
	// sampler -> uv coord descriptors (from inputs)
	// render passes
	struct InputDescriptor {
		Input input;
	};

	struct SamplerDescriptor {
		Sampler sampler;
		SamplerType type;
	};

	struct UniformDescriptor {
		Uniform uniform;
	};

	struct Connector {
		Sampler sampler;
		Input input;
		Action action;
	};

	struct Pass {
		Action action;
		Blend blend;
	};

	std::vector<Input> _inputDescriptors;
	std::vector<SamplerDescriptor> _samplerDescriptors;
	std::vector<UniformDescriptor> _uniformDescriptors;
	std::vector<Connector> _connectors;
	std::vector<Pass> _passes;
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERBUILDER_H
