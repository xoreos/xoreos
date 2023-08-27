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

#include "src/common/strutil.h"

#include "src/graphics/shader/shaderbuilder.h"

namespace Graphics {

namespace Shader {

ShaderDescriptor::ShaderDescriptor() {
}

ShaderDescriptor::~ShaderDescriptor() {
}

void ShaderDescriptor::declareInput(ShaderDescriptor::Input input) {
	_inputDescriptors.push_back(input);
}

void ShaderDescriptor::declareSampler(ShaderDescriptor::Sampler sampler, ShaderDescriptor::SamplerType type) {
	SamplerDescriptor descriptor = {};
	descriptor.sampler = sampler;
	descriptor.type = type;
	_samplerDescriptors.push_back(descriptor);
}

void ShaderDescriptor::declareUniform(ShaderDescriptor::Uniform uniform, int count) {
	UniformDescriptor descriptor = {};
	descriptor.uniform = uniform;
	descriptor.count = count;
	_uniformDescriptors.push_back(descriptor);
}

void ShaderDescriptor::connect(ShaderDescriptor::Sampler sampler, ShaderDescriptor::Input input, ShaderDescriptor::Action action) {
	Connector connector = {};
	connector.sampler = sampler;
	connector.input = input;
	connector.action = action;
	_connectors.push_back(connector);
}

void ShaderDescriptor::addPass(ShaderDescriptor::Action action, ShaderDescriptor::Blend blend) {
	Pass pass = {};
	pass.action = action;
	pass.blend = blend;
	_passes.push_back(pass);
}

void ShaderDescriptor::build(bool isGL3, Common::UString &v_string, Common::UString &f_string) {
	Common::UString v_header, f_header;
	Common::UString v_body, f_body;

	bool hasLighting = true;
	bool foundNormal0 = false;

	/**
	 * @todo Declare required inputs here. For vertex shaders this means the camera
	 * view and transform matrices, and the appropriate object transform matrix.
	 * Fragment shaders have an alpha value and fraggle/froggle for building the
	 * final colour output.
	 */
	if (isGL3) {
		v_header = "#version 150\n\n"
		           "uniform mat4 _objectModelviewMatrix;\n"
		           "uniform mat4 _projectionMatrix;\n"
		           "uniform mat4 _modelviewMatrix;\n";

		v_body =   "void main(void) {\n"
		           "	mat4 mo = (_modelviewMatrix * _objectModelviewMatrix);\n";


		f_header = "#version 150\n\n"
		           "precision highp float;\n\n"
		           "uniform float _alpha;\n";

		f_body =   "out vec4 outColor;\n"
		           "void main(void) {\n"
		           "	vec4 fraggle = vec4(1.0, 0.0, 0.0, 1.0);\n"
		           "	vec4 froggle = vec4(1.0, 0.0, 0.0, 1.0);\n";
	} else {
		v_header = "#version 120\n\n"
		           "uniform mat4 _objectModelviewMatrix;\n"
		           "uniform mat4 _projectionMatrix;\n"
		           "uniform mat4 _modelviewMatrix;\n";

		v_body =   "void main(void) {\n"
		           "	mat4 mo = (_modelviewMatrix * _objectModelviewMatrix);\n";

		f_header = "#version 120\n\n"
		           "uniform float _alpha;\n";

		f_body =   "void main(void) {\n"
		           "	vec4 fraggle = vec4(1.0, 0.0, 0.0, 1.0);\n"
		           "	vec4 froggle = vec4(1.0, 0.0, 0.0, 1.0);\n";
	}

	if (hasLighting) {
		/**
		 * Structs are not supported by the introspection code right now, so treat
		 * lighting as an array of vec4 basic type and index into it appropriately.
		 * It's not the best solution, but it will have to do until structs are
		 * better supported.
		 * struct LightParameters {
		 *     vec4 position;
		 *     vec4 ambient;
		 *     vec4 diffuse;
		 *     vec4 specular;
		 * };
		 * uniform LightParameters _lights[8];
		 */
		f_header += "\n"
		            "uniform vec4 _lights[32];\n"
		            "uniform int _activeLights;\n";

		/**
		 * Lights are basically a position, colour, and maybe strength (diffuse.w).
		 * When calculating lights attaching to a surface however, then surface properties
		 * need to be taken into account: albedo, specular, etc.
		 *
		 * To do this properly, the lighting will be some global variables. Lights are not modelnode
		 * specific normally, but instead apply to an entire area, and so it makes sense for them to
		 * be bound appropriately.
		 * Modelnode data (albedo, specular, etc) will likely be modelnode specific, and iterate
		 * over all enabled light sources.
		 *
		 * It makes most sense at the time of writing to apply lighting to fraggle just before it's
		 * assigned alpha values and handed over to the output framebuffer. So a lighting function
		 * might be best used.
		 */
	}


	int boneCount = 0;

	/**
	 * Extra uniform declarations. These will go into either vertex or fragment
	 * headers as appropriate.
	 */
	for (size_t i = 0; i < _uniformDescriptors.size(); ++i) {
		switch (_uniformDescriptors[i].uniform) {
		case UNIFORM_V_OBJECT_MODELVIEW_MATRIX: break;
		case UNIFORM_V_PROJECTION_MATRIX: break;
		case UNIFOM_V_MODELVIEW_MATRIX: break;
		case UNIFORM_V_BIND_POSE:
			v_header += "uniform mat4 _bindPose;\n";
			break;
		case UNIFORM_V_BONE_TRANSFORMS:
			v_header += "uniform mat4 _boneTransforms[" + Common::composeString(_uniformDescriptors[i].count) + "];\n";
			boneCount = _uniformDescriptors[i].count;
			break;
		case UNIFORM_F_ALPHA: break;
		case UNIFORM_F_COLOUR:
			f_header += "uniform vec4 _colour;\n";
			break;
		default: break;
		}
	}

	/**
	 * Vertex shader input declarations.
	 *
	 * First of all, the speceified inputs need declaring. If they're not actually used,
	 * then it doesn't matter - shader compiler will include the reference for binding
	 * purposes, but won't do anything with the data.
	 * Re-declarations will cause everything to fail, but that's the caller's problem.
	 *
	 * Vertex shaders have an easier time of it, as nothing complicated is really done.
	 * So a good deal of the vertex shader body can be added while going over the input
	 * descriptors now, rather than having to loop over them again later on.
	 *
	 * Below may need tweaking later. It would be better to be able to map an input to
	 * an action, such as texture transform, with the output from that action fed into
	 * an appropriate shader output.
	 * Of course, what's being written here is essentially a "null action" (map input
	 * directly to output), with some hard coded exceptions. Everything later can be
	 * an action:
	 *     input to view modification transform
	 *     input to texture transform
	 *     etc...
	 */
	for (size_t i = 0; i < _inputDescriptors.size(); ++i) {
		Common::UString input_desc_string;
		Common::UString output_desc_string;
		Common::UString body_desc_string;
		Common::UString f_desc_string;
		switch (_inputDescriptors[i]) {
		case INPUT_POSITION0:
			if (isGL3) {
				input_desc_string = "in vec3 inputPosition0;\n";
				output_desc_string = "out vec3 position0;\n";
				f_desc_string = "in vec3 position0;\n";
			} else {
				input_desc_string = "#define inputPosition0 gl_Vertex\n";
				output_desc_string = "varying vec3 position0;\n";
				f_desc_string = "varying vec3 position0;\n";
			}
			if (boneCount > 0) {
				body_desc_string = "vec4 iv = vec4(inputPosition0.xyz, 1.0f);\n"
				                   "vec4 _vertex = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
				                   "mat4 invBindPose = inverse(_bindPose);\n"
				                   "for (int i = 0; i < 4; ++i) {\n"
				                   "	int boneIndex = -1;\n"
				                   "	float boneWeight = 0.0f;\n"
				                   "	if      (i == 0) { boneIndex = int(inputBoneIndices.x); boneWeight = inputBoneWeights.x; }\n"
				                   "	else if (i == 1) { boneIndex = int(inputBoneIndices.y); boneWeight = inputBoneWeights.y; }\n"
				                   "	else if (i == 2) { boneIndex = int(inputBoneIndices.z); boneWeight = inputBoneWeights.z; }\n"
				                   "	else if (i == 3) { boneIndex = int(inputBoneIndices.w); boneWeight = inputBoneWeights.w; }\n"
				                   "	if (boneIndex != -1) {\n"
				                   "		vec4 tmp = (invBindPose * _boneTransforms[boneIndex] * _bindPose) * iv;\n"
				                   "		_vertex.x += boneWeight * tmp.x;\n"
				                   "		_vertex.y += boneWeight * tmp.y;\n"
				                   "		_vertex.z += boneWeight * tmp.z;\n"
				                   "	}\n"
				                   "}\n"
				                   "_vertex = mo * _vertex;"
				                   "gl_Position = _projectionMatrix * _vertex;\n"
				                   "position0 = vec3(_vertex);\n";
			} else {
				///< @note This is always going to be transform matrix modified.
				body_desc_string = "vec4 _vertex = mo * vec4(inputPosition0.xyz, 1.0f);\n"
				                   "gl_Position = _projectionMatrix * _vertex;\n"
				                   "position0 = vec3(_vertex);\n";
			}
			break;
		case INPUT_POSITION1:
			if (isGL3) {
				input_desc_string = "in vec3 inputPosition1;\n";
				output_desc_string = "out vec3 position1;\n";
				f_desc_string = "in vec3 position1;\n";
			} else {
				input_desc_string = "#define inputPosition1 gl_Vertex\n";
				output_desc_string = "varying vec3 position1;\n";
				f_desc_string = "varying vec3 position1;\n";
			}
			body_desc_string = "position1 = inputPosition1;\n";
			break;
		case INPUT_POSITION2:
			if (isGL3) {
				input_desc_string = "in vec3 inputPosition2;\n";
				output_desc_string = "out vec3 position2;\n";
				f_desc_string = "in vec3 position2;\n";
			} else {
				input_desc_string = "#define inputPosition2 gl_Vertex\n";
				output_desc_string = "varying vec3 position2;\n";
				f_desc_string = "varying vec3 position2;\n";
			}
			body_desc_string = "position2 = inputPosition2;\n";
			break;
		case INPUT_POSITION3:
			if (isGL3) {
				input_desc_string = "in vec3 inputPosition3;\n";
				output_desc_string = "out vec3 position3;\n";
				f_desc_string = "in vec3 position3;\n";
			} else {
				input_desc_string = "#define inputPosition3 gl_Vertex\n";
				output_desc_string = "varying vec3 position3;\n";
				f_desc_string = "varying vec3 position3;\n";
			}
			body_desc_string = "position3 = inputPosition3;\n";
			break;
		case INPUT_NORMAL0:
			if (isGL3) {
				input_desc_string = "in vec3 inputNormal0;\n";
				output_desc_string = "out vec3 normal0;\n";
				f_desc_string = "in vec3 normal0;\n";
			} else {
				input_desc_string = "#define inputNormal0 gl_Normal\n";
				output_desc_string = "varying vec3 normal0;\n";
				f_desc_string = "varying vec3 normal0;\n";
			}
			///< @todo This should be modified by the rotation matrix in mo
			body_desc_string = "normal0 = inputNormal0.xyz;\n";
			foundNormal0 = true;
			break;
		case INPUT_NORMAL1:
			if (isGL3) {
				input_desc_string = "in vec3 inputNormal1;\n";
				output_desc_string = "out vec3 normal1;\n";
				f_desc_string = "in vec3 normal1;\n";
			} else {
				input_desc_string = "#define inputNormal1 gl_Normal\n";
				output_desc_string = "varying vec3 normal1;\n";
				f_desc_string = "varying vec3 normal1;\n";
			}
			break;
		case INPUT_NORMAL2:
			if (isGL3) {
				input_desc_string = "in vec3 inputNormal2;\n";
				output_desc_string = "out vec3 normal2;\n";
				f_desc_string = "in vec3 normal2;\n";
			} else {
				input_desc_string = "#define inputNormal2 gl_Normal\n";
				output_desc_string = "varying vec3 normal2;\n";
				f_desc_string = "varying vec3 normal2;\n";
			}
			break;
		case INPUT_NORMAL3:
			if (isGL3) {
				input_desc_string = "in vec3 inputNormal3;\n";
				output_desc_string = "out vec3 normal3;\n";
				f_desc_string = "in vec3 normal3;\n";
			} else {
				input_desc_string = "#define inputNormal3 gl_Normal\n";
				output_desc_string = "varying vec3 normal3;\n";
				f_desc_string = "varying vec3 normal3;\n";
			}
			break;
		case INPUT_UV0:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV0;\n";
				output_desc_string = "out vec2 uv0;\n";
				f_desc_string = "in vec2 uv0;\n";
			} else {
				input_desc_string = "#define inputUV0 gl_MultiTexCoord0\n";
				output_desc_string = "varying vec2 uv0;\n";
				f_desc_string = "varying vec2 uv0;\n";
			}
			body_desc_string = "uv0 = inputUV0.xy;\n";
			break;
		case INPUT_UV1:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV1;\n";
				output_desc_string = "out vec2 uv1;\n";
				f_desc_string = "in vec2 uv1;\n";
			} else {
				input_desc_string = "#define inputUV1 gl_MultiTexCoord1\n";
				output_desc_string = "varying vec2 uv1;\n";
				f_desc_string = "varying vec2 uv1;\n";
			}
			body_desc_string = "uv1 = inputUV1.xy;\n";
			break;
		case INPUT_UV0_MATRIX:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV0;\n"
				                    "uniform mat4 _uv0Matrix;\n";
				output_desc_string = "out vec2 uv0;\n";
				f_desc_string = "in vec2 uv0;\n";
			} else {
				input_desc_string = "#define inputUV0 gl_MultiTexCoord0\n"
				                    "uniform mat4 _uv0Matrix;\n";
				output_desc_string = "varying vec2 uv0;\n";
				f_desc_string = "varying vec2 uv0;\n";
			}
			body_desc_string = "uv0 = (_uv0Matrix * vec4(inputUV0.xy, 0.0, 1.0)).xy;\n";
			break;
		case INPUT_UV1_MATRIX:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV1;\n"
				                    "uniform mat2 _uv1Matrix;\n";
				output_desc_string = "out vec2 uv1;\n";
				f_desc_string = "in vec2 uv1;\n";
			} else {
				input_desc_string = "#define inputUV1 gl_MultiTexCoord1\n"
				                    "uniform mat2 _uv1Matrix;\n";
				output_desc_string = "varying vec2 uv1;\n";
				f_desc_string = "varying vec2 uv1;\n";
			}
			body_desc_string = "uv1 = (_uv1Matrix * vec4(inputUV1.xy, 0.0, 1.0)).xy;\n";
			break;
		case INPUT_UV_CUBE:
			if (isGL3) {
				input_desc_string = "// UV Cube coordinates declared.\n";
				output_desc_string = "out vec3 uvCube;\n";
				f_desc_string = "in vec3 uvCube;\n";
			} else {
				input_desc_string = "// UV Cube coordinates declared.\n";
				output_desc_string = "varying vec3 uvCube;\n";
				f_desc_string = "varying vec3 uvCube;\n";
			}
			body_desc_string = "	vec3 ucube = normalize(position0);\n"
			                   "	vec3 ncube = normalize(normal0);\n"
			                   "	uvCube = reflect(ucube, ncube);\n";
			break;
		case INPUT_UV_SPHERE:
			if (isGL3) {
				input_desc_string = "// UV Sphere coordinates declared.\n";
				output_desc_string = "out vec2 uvSphere;\n";
				f_desc_string = "in vec2 uvSphere;\n";
			} else {
				input_desc_string = "// UV Sphere coordinates declared.\n";
				output_desc_string = "varying vec2 uvSphere;\n";
				f_desc_string = "varying vec2 uvSphere;\n";
			}
			body_desc_string = "	vec3 usphere = normalize(position0);\n"
			                   "	vec3 nsphere = normalize(normal0);\n"
			                   "	vec3 rsphere = reflect(usphere, nsphere);\n"
			                   "	float msphere = 2.0 * sqrt(rsphere.x * rsphere.x + rsphere.y * rsphere.y + (rsphere.z + 1.0) * (rsphere.z + 1.0));\n"
			                   "	uvSphere = vec2(rsphere.x / msphere + 0.5, rsphere.y / msphere + 0.5);\n";
			break;
		case INPUT_COLOUR:
			if (isGL3) {
				input_desc_string = "in vec4 inputColour;\n";
				output_desc_string = "out vec4 xColour;\n";
				f_desc_string = "in vec4 xColour;\n";
			} else {
				input_desc_string = "#define inputColour gl_Color\n";
				output_desc_string = "varying vec4 xColour;\n";
				f_desc_string = "varying vec4 xColour;\n";
			}
			body_desc_string = "xColour = inputColour;\n";
			break;
		case INPUT_BONE_INDICES:
			if (isGL3) {
				input_desc_string = "in vec4 inputBoneIndices;\n";
				output_desc_string = "out vec4 boneIndices;\n";
				f_desc_string = "in vec4 boneIndices;\n";
			} else {
				input_desc_string = "attribute vec4 inputBoneIndices;\n";
				output_desc_string = "varying vec4 boneIndices;\n";
				f_desc_string = "varying vec4 boneIndices;\n";
			}
			body_desc_string = "boneIndices = inputBoneIndices;\n";
			break;
		case INPUT_BONE_WEIGHTS:
			if (isGL3) {
				input_desc_string = "in vec4 inputBoneWeights;\n";
				output_desc_string = "out vec4 boneWeights;\n";
				f_desc_string = "in vec4 boneWeights;\n";
			} else {
				input_desc_string = "attribute vec4 inputBoneWeights;\n";
				output_desc_string = "varying vec4 boneWeights;\n";
				f_desc_string = "varying vec4 boneWeights;\n";
			}
			body_desc_string = "boneWeights = inputBoneWeights;\n";
			break;
		}
		v_header += input_desc_string;
		v_header += output_desc_string;
		v_body += body_desc_string;
		f_header += f_desc_string;
	}

	/**
	 * Fragment shader sampler declarations.
	 *
	 * Now declare samplers. Actually this needs to be done as part of the connection phase
	 * because the sampler type is attached there, and generic sampler declaration isn't supported.
	 * Instead, prepare a string for the declaration now, and prepend the appropriate type later.
	 */
	for (size_t i = 0; i < _samplerDescriptors.size(); ++i) {
		Common::UString sampler_descriptor_string = "uniform ";
		Common::UString sampler_type_string = "sampler";
		Common::UString sampler_id_string = "sampler_";
		switch (_samplerDescriptors[i].type) {
		case SAMPLER_1D:   sampler_type_string += "1D"; break;
		case SAMPLER_2D:   sampler_type_string += "2D"; break;
		case SAMPLER_3D:   sampler_type_string += "3D"; break;
		case SAMPLER_CUBE: sampler_type_string += "Cube"; break;
		default: break;
		}

		switch (_samplerDescriptors[i].sampler) {
		case SAMPLER_TEXTURE_0: sampler_id_string += "0"; break;
		case SAMPLER_TEXTURE_1: sampler_id_string += "1"; break;
		case SAMPLER_TEXTURE_2: sampler_id_string += "2"; break;
		case SAMPLER_TEXTURE_3: sampler_id_string += "3"; break;
		case SAMPLER_TEXTURE_4: sampler_id_string += "4"; break;
		case SAMPLER_TEXTURE_5: sampler_id_string += "5"; break;
		case SAMPLER_TEXTURE_6: sampler_id_string += "6"; break;
		case SAMPLER_TEXTURE_7: sampler_id_string += "7"; break;
		case SAMPLER_TEXTURE_NONE: sampler_id_string += "uv"; break;
		}
		sampler_id_string += "_id";

		sampler_descriptor_string += sampler_type_string;
		sampler_descriptor_string += " ";
		sampler_descriptor_string += sampler_id_string;
		sampler_descriptor_string += ";\n";

		f_header += sampler_descriptor_string;
	}

	if (!foundNormal0) {
		/**
		 * Normal0 wasn't defined as an input, however some later code might be relying on it,
		 * particularly lighting. The default GL value for a normal was (0,0,1), so it seems
		 * reasonable to copy that if not otherwise found.
		 */
		 f_header += "vec3 normal0 = vec3(0, 0, 1);\n";
	}

	/**
	 * Fragment shader sampler uv coordinate mapping.
	 *
	 * Inputs and sampler declared. Now they need to be connected. Essentially this next step
	 * just maps an input name to something the sampler can assume exists, by using some simple
	 * macro definitions. So sample_0_id will always use sample_0_coords for sampling, but that
	 * name can map to any input. The same input can be used for multiple samplers, but the same
	 * sampler cannot use multiple inputs (which makes sense if you think about it hard enough).
	 *
	 * Samplers (which have known macro names)
	 */
	for (size_t i = 0; i < _connectors.size(); ++i) {

		Common::UString f_sampler_id_string;
		switch (_connectors[i].sampler) {
		case SAMPLER_TEXTURE_0: f_sampler_id_string = "0"; break;
		case SAMPLER_TEXTURE_1: f_sampler_id_string = "1"; break;
		case SAMPLER_TEXTURE_2: f_sampler_id_string = "2"; break;
		case SAMPLER_TEXTURE_3: f_sampler_id_string = "3"; break;
		case SAMPLER_TEXTURE_4: f_sampler_id_string = "4"; break;
		case SAMPLER_TEXTURE_5: f_sampler_id_string = "5"; break;
		case SAMPLER_TEXTURE_6: f_sampler_id_string = "6"; break;
		case SAMPLER_TEXTURE_7: f_sampler_id_string = "7"; break;
		case SAMPLER_TEXTURE_NONE: f_sampler_id_string = "uv"; break;
		}

		Common::UString f_input_string;
		switch (_connectors[i].input) {
		case INPUT_POSITION0: f_input_string = "position0"; break;
		case INPUT_POSITION1: f_input_string = "position1"; break;
		case INPUT_POSITION2: f_input_string = "position2"; break;
		case INPUT_POSITION3: f_input_string = "position3"; break;
		case INPUT_NORMAL0: f_input_string = "normal0"; break;
		case INPUT_NORMAL1: f_input_string = "normal1"; break;
		case INPUT_NORMAL2: f_input_string = "normal2"; break;
		case INPUT_NORMAL3: f_input_string = "normal3"; break;
		case INPUT_UV0: f_input_string = "uv0"; break;
		case INPUT_UV1: f_input_string = "uv1"; break;
		case INPUT_UV0_MATRIX: f_input_string = "uv0"; break;
		case INPUT_UV1_MATRIX: f_input_string = "uv1"; break;
		case INPUT_UV_CUBE: f_input_string = "uvCube"; break;
		case INPUT_UV_SPHERE: f_input_string = "uvSphere"; break;
		case INPUT_COLOUR: f_input_string = "xColour"; break;
		default: break;
		}

		Common::UString f_action_string;
		switch (_connectors[i].action) {
		case ENV_CUBE: f_action_string = "env_cube"; break;
		case ENV_SPHERE: f_action_string = "env_sphere"; break;
		case COLOUR: f_action_string = "colour"; break;
		case X_COLOUR: f_action_string = "xcolour"; break;
		case TEXTURE_DIFFUSE: f_action_string = "diffuse"; break;
		case TEXTURE_LIGHTMAP: f_action_string = "lightmap"; break;
		case TEXTURE_BUMPMAP: f_action_string = "bumpmap"; break;
		case FORCE_OPAQUE: f_action_string = "force_opaque"; break;
		case NOOP: f_action_string = "noop"; break;
		}

		// Map an input to a sampler coordinate.
		Common::UString f_connector_input_sampler_string = "#define sampler_";
		f_connector_input_sampler_string += f_sampler_id_string;
		f_connector_input_sampler_string += "_coords ";
		f_connector_input_sampler_string += f_input_string;
		f_connector_input_sampler_string += "\n";
		f_header += f_connector_input_sampler_string;

		// Map a sampler to an action.
		Common::UString f_connector_sampler_action_string = "#define action_";
		f_connector_sampler_action_string += f_action_string;
		f_connector_sampler_action_string += "_sampler sampler_";
		f_connector_sampler_action_string += f_sampler_id_string;
		f_connector_sampler_action_string += "_id";
		f_connector_sampler_action_string += "\n";
		f_header += f_connector_sampler_action_string;

		// Map an input to an action (identical to sampler coordinate).
		Common::UString f_connector_input_action_string = "#define action_";
		f_connector_input_action_string += f_action_string;
		f_connector_input_action_string += "_coords ";
		f_connector_input_action_string += f_input_string;
		f_connector_input_action_string += "\n";
		f_header += f_connector_input_action_string;
	}


	/**
	 * All declarations should now be in place.
	 */
	for (size_t i = 0; i < _passes.size(); ++i) {
		Common::UString f_action_string;
		switch (_passes[i].action) {
		case ENV_CUBE:
			if (isGL3) {
				f_action_string = "froggle = texture(action_env_cube_sampler, action_env_cube_coords);\n";
			} else {
				f_action_string = "froggle = textureCube(action_env_cube_sampler, action_env_cube_coords);\n";
			}
			break;
		case ENV_SPHERE:
			if (isGL3) {
				f_action_string = "froggle = texture(action_env_sphere_sampler, action_env_sphere_coords);\n";
			} else {
				f_action_string = "froggle = texture2D(action_env_sphere_sampler, action_env_sphere_coords);\n";
			}
			break;
		case COLOUR:
			f_action_string = "froggle = _colour; // TODO: colour is not a sampler.\n";
			break;
		case X_COLOUR:
			f_action_string = "froggle = xColour; // TODO: colour is not a sampler.\n";
			break;
		case TEXTURE_DIFFUSE:
			if (isGL3) {
				f_action_string = "froggle = texture(action_diffuse_sampler, action_diffuse_coords);\n";
			} else {
				f_action_string = "froggle = texture2D(action_diffuse_sampler, action_diffuse_coords);\n";
			}
			break;
		case TEXTURE_LIGHTMAP:
			if (isGL3) {
				f_action_string = "froggle = vec4(texture(action_lightmap_sampler, action_lightmap_coords).xyz, 1.0);\n";
			} else {
				f_action_string = "froggle = vec4(texture2D(action_lightmap_sampler, action_lightmap_coords).xyz, 1.0);\n";
			}
			break;
		case TEXTURE_BUMPMAP:
			if (isGL3) {
				f_action_string = "froggle = texture(action_bumpmap_sampler, action_bumpmap_coords);\n";
			} else {
				f_action_string = "froggle = texture2D(action_bumpmap_sampler, action_bumpmap_coords);\n";
			}
			break;
		case FORCE_OPAQUE:
			f_action_string = "fraggle.a = 1.0f;\n";
			break;
		case NOOP:
			f_action_string = "// noop;\n";
			break;
		}

		Common::UString f_blend_string;
		switch (_passes[i].blend) {
		case BLEND_SRC_ALPHA:
			f_blend_string = "fraggle = mix(fraggle, froggle, froggle.a);\n";
			break;
		case BLEND_DST_ALPHA:
			f_blend_string = "fraggle += (froggle * (1.0f - fraggle.a));\n";
			break;
		case BLEND_ZERO:
			f_blend_string = "\n";
			break;
		case BLEND_ONE:
			f_blend_string = "fraggle = froggle;\n";
			break;
		case BLEND_MULTIPLY:
			f_blend_string = "fraggle *= froggle;\n";
			break;
		case BLEND_IGNORED:
			break;
		}

		f_body += f_action_string;
		f_body += f_blend_string;
	}

	if (hasLighting) {
		/**
		 * It's worth noting that lights always need a position to work on. This is assumed
		 * to be position0, and declared as an in to the fragment shader. That's probably
		 * a safe assumption to make; if it's not, then we'll find out pretty quickly because
		 * the shader won't compile and the program will crash.
		 *
		 * Normals a bit tricker. If a normal vector is supplied by the mesh data, then
		 * lighting still applies - just without the gradient associated with a normal.
		 * @todo: check to see if the shader descriptor has normals or not.
		 * @todo: the ambient hack is just until actual object ambient values are implemented.
		 */
		f_body +=
			"vec4 laggle = vec4(0.0, 0.0, 0.0, 0.0);\n"
			"vec3 _ambient = vec3(1.0, 1.0, 1.0);\n"
			"for (int i = 0; i < _activeLights; ++i) {\n"
			"    vec3 direction = normalize(_lights[4*i+3].xyz - position0);\n"
			"    float distance = length(_lights[4*i+3].xyz - position0);\n"
			"    float grad = max(dot(normal0, direction), 0.0);\n"
			"    float attenuation = 1.0 / distance;\n"
			"    vec4 diffuse = _lights[4*i+1] * grad * attenuation; // diffuse\n"
			"    vec4 ambient = _lights[4*i+0] * attenuation; // ambient\n"
			"    laggle += fraggle * ambient;\n"
			"    laggle += fraggle * diffuse;\n"
			"    _ambient *= 0.8;\n"
			"}\n"
			"laggle.a = 0.0;  // TODO: light components should be just rgb, no a.\n"
			"fraggle = (fraggle * vec4(_ambient, 1.0)) + laggle;\n"
			"\n";
	}

	/**
	 * Wrap up the shaders.
	 */
	if (isGL3) {
		v_body += "}\n";

		f_body += "fraggle.a = fraggle.a * _alpha;\n"
		          "outColor = fraggle;\n"
		          "}\n";
	} else {
		v_body += "}\n";

		f_body += "fraggle.a = fraggle.a * _alpha;\n"
		          "gl_FragColor = fraggle;\n"
		          "}\n";
	}

	/**
	 * So now there are strings for vertex and fragment shaders, header and body.
	 * Shaders, transform and rollout.
	 */

	Common::UString v_shader_string = v_header + v_body;
	Common::UString f_shader_string = f_header + f_body;

	v_string = v_header + v_body;
	f_string = f_header + f_body;
}

void ShaderDescriptor::clear() {
	_inputDescriptors.clear();
	_samplerDescriptors.clear();
	_uniformDescriptors.clear();
	_connectors.clear();
	_passes.clear();
}

void ShaderDescriptor::genName(Common::UString &n_string) {
	for (size_t i = 0; i < _uniformDescriptors.size(); ++i) {
		n_string += "__";
		switch (_uniformDescriptors[i].uniform) {
		case UNIFORM_V_BIND_POSE: n_string += "uniform_bindpose"; break;
		case UNIFORM_V_BONE_TRANSFORMS: n_string += "uniform_bonetransforms" + Common::composeString(_uniformDescriptors[i].count); break;
		default: break;
		}
	}

	for (size_t i = 0; i < _inputDescriptors.size(); ++i) {
		n_string += "__";
		switch (_inputDescriptors[i]) {
		case INPUT_POSITION0: n_string += "input_position0"; break;
		case INPUT_POSITION1: n_string += "input_position1"; break;
		case INPUT_POSITION2: n_string += "input_position2"; break;
		case INPUT_POSITION3: n_string += "input_position3"; break;
		case INPUT_NORMAL0: n_string += "input_normal0"; break;
		case INPUT_NORMAL1: n_string += "input_normal1"; break;
		case INPUT_NORMAL2: n_string += "input_normal2"; break;
		case INPUT_NORMAL3: n_string += "input_normal3"; break;
		case INPUT_UV0: n_string += "input_uv0"; break;
		case INPUT_UV1: n_string += "input_uv1"; break;
		case INPUT_UV0_MATRIX: n_string += "input_uv0"; break;
		case INPUT_UV1_MATRIX: n_string += "input_uv1"; break;
		case INPUT_UV_CUBE: n_string += "input_uv_cube"; break;
		case INPUT_UV_SPHERE: n_string += "input_uv_sphere"; break;
		case INPUT_COLOUR: n_string += "input_colour"; break;
		case INPUT_BONE_INDICES: n_string += "input_boneindices"; break;
		case INPUT_BONE_WEIGHTS: n_string += "input_boneweights"; break;
		}
	}

	for (size_t i = 0; i < _samplerDescriptors.size(); ++i) {
		n_string += "__";
		switch (_samplerDescriptors[i].type) {
		case SAMPLER_1D: n_string += "sampler_1d"; break;
		case SAMPLER_2D: n_string += "sampler_2d"; break;
		case SAMPLER_3D: n_string += "sampler_3d"; break;
		case SAMPLER_CUBE: n_string += "sampler_cube"; break;
		default: break;
		}

		n_string += "-";
		switch (_samplerDescriptors[i].sampler) {
		case SAMPLER_TEXTURE_0: n_string += "texture0"; break;
		case SAMPLER_TEXTURE_1: n_string += "texture1"; break;
		case SAMPLER_TEXTURE_2: n_string += "texture2"; break;
		case SAMPLER_TEXTURE_3: n_string += "texture3"; break;
		case SAMPLER_TEXTURE_4: n_string += "texture4"; break;
		case SAMPLER_TEXTURE_5: n_string += "texture5"; break;
		case SAMPLER_TEXTURE_6: n_string += "texture6"; break;
		case SAMPLER_TEXTURE_7: n_string += "texture7"; break;
		case SAMPLER_TEXTURE_NONE: n_string += "textureUV"; break;
		}
	}

	for (size_t i = 0; i < _connectors.size(); ++i) {
		n_string += "__";
		switch (_connectors[i].sampler) {
		case SAMPLER_TEXTURE_0: n_string += "texture0"; break;
		case SAMPLER_TEXTURE_1: n_string += "texture1"; break;
		case SAMPLER_TEXTURE_2: n_string += "texture2"; break;
		case SAMPLER_TEXTURE_3: n_string += "texture3"; break;
		case SAMPLER_TEXTURE_4: n_string += "texture4"; break;
		case SAMPLER_TEXTURE_5: n_string += "texture5"; break;
		case SAMPLER_TEXTURE_6: n_string += "texture6"; break;
		case SAMPLER_TEXTURE_7: n_string += "texture7"; break;
		case SAMPLER_TEXTURE_NONE: n_string += "textureUV"; break;
		}

		n_string += "-";
		switch (_connectors[i].input) {
		case INPUT_POSITION0: n_string += "input_position0"; break;
		case INPUT_POSITION1: n_string += "input_position1"; break;
		case INPUT_POSITION2: n_string += "input_position2"; break;
		case INPUT_POSITION3: n_string += "input_position3"; break;
		case INPUT_NORMAL0: n_string += "input_normal0"; break;
		case INPUT_NORMAL1: n_string += "input_normal1"; break;
		case INPUT_NORMAL2: n_string += "input_normal2"; break;
		case INPUT_NORMAL3: n_string += "input_normal3"; break;
		case INPUT_UV0: n_string += "input_uv0"; break;
		case INPUT_UV1: n_string += "input_uv1"; break;
		case INPUT_UV0_MATRIX: n_string += "input_uv0"; break;
		case INPUT_UV1_MATRIX: n_string += "input_uv1"; break;
		case INPUT_UV_CUBE: n_string += "input_uv_cube"; break;
		case INPUT_UV_SPHERE: n_string += "input_uv_sphere"; break;
		case INPUT_COLOUR: n_string += "input_colour"; break;
		case INPUT_BONE_INDICES: n_string += "input_boneindices"; break;
		case INPUT_BONE_WEIGHTS: n_string += "input_boneweights"; break;
		}

		n_string += "-";
		switch (_connectors[i].action) {
		case ENV_CUBE: n_string += "env_cube"; break;
		case ENV_SPHERE: n_string += "env_sphere"; break;
		case COLOUR: n_string += "colour"; break;
		case X_COLOUR: n_string += "xcolour"; break;
		case TEXTURE_DIFFUSE: n_string += "diffuse"; break;
		case TEXTURE_LIGHTMAP: n_string += "lightmap"; break;
		case TEXTURE_BUMPMAP: n_string += "bumpmap"; break;
		case FORCE_OPAQUE: n_string += "force_opaque"; break;
		case NOOP: n_string += "noop"; break;
		}
	}

	for (size_t i = 0; i < _passes.size(); ++i) {
		n_string += "__";
		switch (_passes[i].action) {
		case ENV_CUBE: n_string += "env_cube"; break;
		case ENV_SPHERE: n_string += "env_sphere"; break;
		case COLOUR: n_string += "colour"; break;
		case X_COLOUR: n_string += "xcolour"; break;
		case TEXTURE_DIFFUSE: n_string += "diffuse"; break;
		case TEXTURE_LIGHTMAP: n_string += "lightmap"; break;
		case TEXTURE_BUMPMAP: n_string += "bumpmap"; break;
		case FORCE_OPAQUE: n_string += "force_opaque"; break;
		case NOOP: n_string += "noop"; break;
		}

		n_string += "-";
		switch (_passes[i].blend) {
		case BLEND_SRC_ALPHA: n_string += "blend_src_alpha"; break;
		case BLEND_DST_ALPHA: n_string += "blend_dst_alpha"; break;
		case BLEND_ZERO: n_string += "blend_zero"; break;
		case BLEND_ONE: n_string += "blend_one"; break;
		case BLEND_MULTIPLY: n_string += "blend_multiply"; break;
		case BLEND_IGNORED: n_string += "blend_ignored"; break;
		}
	}
}

} // End of namespace Shader

} // End of namespace Graphics
