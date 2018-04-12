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

#include "src/graphics/shader/shaderbuilder.h"

DECLARE_SINGLETON(Graphics::Shader::ShaderBuilder)

namespace Graphics {

namespace Shader {

// ---------------------------------------------------------
//OpenGL 3.2 (GLSL 150) shader strings.
// ---------------------------------------------------------
static const Common::UString header_default_3vert =
"#version 150\n\n"
"in vec3 inPosition;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord0;\n"
"out vec3 _normal;\n"
"out vec3 _position;\n"
"out vec2 _texCoords;\n"
"uniform mat4 _objectModelviewMatrix;\n"
"uniform mat4 _projectionMatrix;\n"
"uniform mat4 _modelviewMatrix;\n";

static const Common::UString header_envcube_3vert =
"out vec3 _cubeCoords;\n";

static const Common::UString header_envsphere_3vert =
"out vec2 _sphereCoords;\n";

static const Common::UString header_colour_3vert =
"\n";

static const Common::UString header_texture_3vert =
"\n";

static const Common::UString header_lightmap_3vert =
"in vec2 inTexCoord1;\n"
"out vec2 _lightmapCoords;\n";

static const Common::UString body_default_start_3vert =
"void main(void) {\n"
"	mat4 mo = (_modelviewMatrix * _objectModelviewMatrix);\n"
"	vec4 vertex = mo * vec4(inPosition, 1.0);\n"
"	gl_Position = _projectionMatrix * vertex;\n"
"	_normal = mat3(mo) * inNormal;\n"
"	_position = vec3(vertex);\n"
"	_texCoords = vec2(inTexCoord0);\n";

static const Common::UString body_colour_3vert =
"\n";

static const Common::UString body_texture_3vert =
"\n";

static const Common::UString body_envcube_3vert =
"	vec3 ucube = normalize(_position);\n"
"	vec3 ncube = normalize(_normal);\n"
"	_cubeCoords = reflect(ucube, ncube);\n";

static const Common::UString body_envsphere_3vert =
"	vec3 usphere = normalize(_position);\n"
"	vec3 nsphere = normalize(_normal);\n"
"	vec3 rsphere = reflect(usphere, nsphere);\n"
"	float msphere = 2.0 * sqrt(rsphere.x * rsphere.x + rsphere.y * rsphere.y + (rsphere.z + 1.0) * (rsphere.z + 1.0));\n"
"	_sphereCoords = vec2(rsphere.x / msphere + 0.5, rsphere.y / msphere + 0.5);\n";

static const Common::UString body_lightmap_3vert =
"	_lightmapCoords = vec2(inTexCoord1);\n";

static const Common::UString body_default_end_3vert =
"}\n";



static const Common::UString header_default_3frag =
"#version 150\n\n"
"precision highp float;\n\n"
"uniform float _alpha;\n";

static const Common::UString header_colour_3frag =
"uniform vec4 _color;\n";

static const Common::UString header_texture_3frag =
"uniform sampler2D _texture0;\n";

static const Common::UString header_envcube_3frag =
"in vec3 _cubeCoords;\n"
"uniform samplerCube _textureCube0;\n";

static const Common::UString header_envsphere_3frag =
"in vec2 _sphereCoords;\n"
"uniform sampler2D _textureSphere0;\n";

static const Common::UString header_lightmap_3frag =
"in vec2 _lightmapCoords;\n"
"uniform sampler2D _lightmap;\n";

static const Common::UString body_default_start_3frag =
"in vec3 _normal;\n"
"in vec3 _position;\n"
"in vec2 _texCoords;\n\n"
"out vec4 outColor;\n"
"void main(void) {\n"
"	vec4 fraggle = vec4(0.0, 0.0, 0.0, 0.0);\n"
"	vec4 froggle = vec4(0.0, 0.0, 0.0, 0.0);\n";

static const Common::UString body_default_end_3frag =
"	fraggle.a = fraggle.a * _alpha;\n"
"	outColor = fraggle;\n"
"}\n";

static const Common::UString body_colour_3frag =
"	froggle = _color;\n";

static const Common::UString body_texture_3frag =
"	froggle = texture(_texture0, _texCoords);\n";

static const Common::UString body_envcube_3frag =
"	froggle = texture(_textureCube0, _cubeCoords);\n";

static const Common::UString body_envsphere_3frag =
"	froggle = texture(_textureSphere0, _sphereCoords);\n";

static const Common::UString body_lightmap_3frag =
"	froggle = texture(_lightmap, _lightmapCoords);\n";
// ---------------------------------------------------------
static const Common::UString body_blend_src_alpha_3frag =
"	fraggle = mix(fraggle, froggle, froggle.a);\n";

static const Common::UString body_blend_dst_alpha_3frag =
"	fraggle += (froggle * (1.0f - fraggle.a));\n";

static const Common::UString body_blend_zero_3frag =
"\n";

static const Common::UString body_blend_one_3frag =
"	fraggle = froggle;\n";

static const Common::UString body_blend_multiply_3frag =
"	fraggle *= froggle;\n";
// ---------------------------------------------------------
static const Common::UString body_force_opaque_3frag =
"	fraggle.a = 1.0f;\n";
// ---------------------------------------------------------


// ---------------------------------------------------------
//OpenGL 2.1 (GLSL 120) shader strings.
// ---------------------------------------------------------
static const Common::UString header_default_2vert =
"#version 120\n"
"varying vec3 _normal;\n"
"varying vec3 _position;\n"
"varying vec2 _texCoords;\n"
"uniform mat4 _objectModelviewMatrix;\n"
"uniform mat4 _projectionMatrix;\n"
"uniform mat4 _modelviewMatrix;\n";

static const Common::UString header_envcube_2vert =
"varying vec3 _cubeCoords;\n";

static const Common::UString header_envsphere_2vert =
"varying vec2 _sphereCoords;\n";

static const Common::UString header_colour_2vert =
"\n";

static const Common::UString header_texture_2vert =
"\n";

static const Common::UString header_lightmap_2vert =
"varying vec2 _lightmapCoords;\n";

static const Common::UString body_default_start_2vert =
"void main(void) {\n"
"	mat4 mo = (_modelviewMatrix * _objectModelviewMatrix);\n"
"	vec4 vertex = mo * gl_Vertex;\n"
"	gl_Position = _projectionMatrix * vertex;\n"
"	_normal = mat3(mo) * vec3(gl_Normal);\n"
"	_position = vec3(vertex);\n"
"	_texCoords = vec2(gl_MultiTexCoord0);\n";

static const Common::UString body_envcube_2vert =
"	vec3 ucube = normalize(_position);\n"
"	vec3 ncube = normalize(_normal);\n"
"	_cubeCoords = reflect(ucube, ncube);\n";

static const Common::UString body_envsphere_2vert =
"	vec3 usphere = normalize(_position);\n"
"	vec3 nsphere = normalize(_normal);\n"
"	vec3 rsphere = reflect(usphere, nsphere);\n"
"	float msphere = 2.0 * sqrt(rsphere.x * rsphere.x + rsphere.y * rsphere.y + (rsphere.z + 1.0) * (rsphere.z + 1.0));\n"
"	_sphereCoords = vec2(rsphere.x / msphere + 0.5, rsphere.y / msphere + 0.5);\n";

static const Common::UString body_colour_2vert =
"\n";

static const Common::UString body_texture_2vert =
"\n";

static const Common::UString body_lightmap_2vert =
"	_lightmapCoords = vec2(gl_MultiTexCoord1);\n";

static const Common::UString body_default_end_2vert =
"}\n";



static const Common::UString header_default_2frag =
"#version 120\n\n"
"uniform float _alpha;\n";

static const Common::UString header_colour_2frag =
"uniform vec4 _color;\n";

static const Common::UString header_texture_2frag =
"uniform sampler2D _texture0;\n";

static const Common::UString header_envcube_2frag =
"varying vec3 _cubeCoords;\n"
"uniform samplerCube _textureCube0;\n";

static const Common::UString header_envsphere_2frag =
"varying vec2 _sphereCoords;\n"
"uniform sampler2D _textureSphere0;\n";

static const Common::UString header_lightmap_2frag =
"varying vec2 _lightmapCoords;\n"
"uniform sampler2D _lightmap;\n";

static const Common::UString body_default_start_2frag =
"varying vec3 _normal;\n"
"varying vec3 _position;\n"
"varying vec2 _texCoords;\n\n"
"void main(void) {\n"
"	vec4 fraggle = vec4(0.0, 0.0, 0.0, 0.0);\n"
"	vec4 froggle;";

static const Common::UString body_default_end_2frag =
"	fraggle.a = fraggle.a * _alpha;\n"
"	gl_FragColor = fraggle;\n"
"}\n";

static const Common::UString body_colour_2frag =
"	froggle = _color;\n";

static const Common::UString body_texture_2frag =
"	froggle = texture2D(_texture0, _texCoords);\n";

static const Common::UString body_envcube_2frag =
"	froggle = textureCube(_textureCube0, _cubeCoords);\n";

static const Common::UString body_envsphere_2frag =
"	froggle = texture2D(_textureSphere0, _sphereCoords);\n";

static const Common::UString body_lightmap_2frag =
"	froggle = texture2D(_lightmap, _lightmapCoords);\n";
// ---------------------------------------------------------
static const Common::UString body_blend_src_alpha_2frag =
"	fraggle = mix(fraggle, froggle, froggle.a);\n";

static const Common::UString body_blend_dst_alpha_2frag =
"	fraggle += (froggle * (1.0f - fraggle.a));\n";

static const Common::UString body_blend_zero_2frag =
"\n";

static const Common::UString body_blend_one_2frag =
"	fraggle = froggle;\n";

static const Common::UString body_blend_multiply_2frag =
"	fraggle *= froggle;\n";
// ---------------------------------------------------------
static const Common::UString body_force_opaque_2frag =
"	fraggle.a = 1.0f;\n";
// ---------------------------------------------------------

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
			///< @note This is always going to be transform matrix modified.
			body_desc_string = "vec4 _vertex = mo * vec4(inputPosition0.xyz, 1.0f);\n"
			                   "gl_Position = _projectionMatrix * _vertex;\n"
			                   "position0 = vec3(_vertex);\n";
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
			///< @todo This is always modified by something special.
			body_desc_string = "normal0 = inputNormal0.xyz;\n";
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
		case INPUT_UV2:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV2;\n";
				output_desc_string = "out vec2 uv2;\n";
				f_desc_string = "in vec2 uv2;\n";
			} else {
				input_desc_string = "#define inputUV2 gl_MultiTexCoord2\n";
				output_desc_string = "varying vec2 uv2;\n";
				f_desc_string = "varying vec2 uv2;\n";
			}
			body_desc_string = "uv2 = inputUV2.xy;\n";
			break;
		case INPUT_UV3:
			if (isGL3) {
				input_desc_string = "in vec2 inputUV3;\n";
				output_desc_string = "out vec2 uv3;\n";
				f_desc_string = "in vec2 uv3;\n";
			} else {
				input_desc_string = "#define inputUV3 gl_MultiTexCoord3\n";
				output_desc_string = "varying vec2 uv3;\n";
				f_desc_string = "varying vec2 uv3;\n";
			}
			body_desc_string = "uv3 = inputUV3.xy;\n";
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
		case INPUT_UV2: f_input_string = "uv2"; break;
		case INPUT_UV3: f_input_string = "uv3"; break;
		case INPUT_UV_CUBE: f_input_string = "uvCube"; break;
		case INPUT_UV_SPHERE: f_input_string = "uvSphere"; break;
		case INPUT_COLOUR: f_input_string = "xColour"; break;
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
			f_action_string = "froggle = vec4(1.0, 1.0, 1.0, 1.0); // TODO: colour is not a sampler.\n";
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

	//printf("Vertex Shader:\n%s\n", v_shader_string.c_str());
	//printf("Fragment Shader:\n%s\n", f_shader_string.c_str());
}

void ShaderDescriptor::genName(Common::UString &n_string) {
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
		case INPUT_UV2: n_string += "input_uv2"; break;
		case INPUT_UV3: n_string += "input_uv3"; break;
		case INPUT_UV_CUBE: n_string += "input_uv_cube"; break;
		case INPUT_UV_SPHERE: n_string += "input_uv_sphere"; break;
		case INPUT_COLOUR: n_string += "input_colour"; break;
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
		case INPUT_UV2: n_string += "input_uv2"; break;
		case INPUT_UV3: n_string += "input_uv3"; break;
		case INPUT_UV_CUBE: n_string += "input_uv_cube"; break;
		case INPUT_UV_SPHERE: n_string += "input_uv_sphere"; break;
		case INPUT_COLOUR: n_string += "input_colour"; break;
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


ShaderBuilder::ShaderBuilder() {
}

ShaderBuilder::~ShaderBuilder() {
}

Common::UString ShaderBuilder::genVertexShader(BuildPass *passes, uint32 count, bool isGL3) {
	Common::UString header;
	Common::UString body;
	Common::UString combined;

	initVertexShaderString(header, body, isGL3);
	for (uint32 i = 0; i < count; ++i) {
		addVertexShaderString(header, body, passes[i].pass, passes[i].blend, isGL3);
	}
	finaliseVertexShaderString(combined, header, body, isGL3);
	return combined;
}

Common::UString ShaderBuilder::genVertexShaderName(BuildPass *passes, uint32 count) {
	Common::UString name;
	initShaderName(name);
	for (uint32 i = 0; i < count; ++i) {
		addShaderName(name, passes[i].pass, passes[i].blend);
	}
	finaliseShaderNameVertex(name);
	return name;
}

Common::UString ShaderBuilder::genFragmentShader(BuildPass *passes, uint32 count, bool isGL3) {
	Common::UString header;
	Common::UString body;
	Common::UString combined;

	initFragmentShaderString(header, body, isGL3);
	for (uint32 i = 0; i < count; ++i) {
		addFragmentShaderString(header, body, passes[i].pass, passes[i].blend, isGL3);
	}
	finaliseFragmentShaderString(combined, header, body, isGL3);
	return combined;
}

Common::UString ShaderBuilder::genFragmentShaderName(BuildPass *passes, uint32 count) {
	Common::UString name;
	initShaderName(name);
	for (uint32 i = 0; i < count; ++i) {
		addShaderName(name, passes[i].pass, passes[i].blend);
	}
	finaliseShaderNameFragment(name);
	return name;
}

void ShaderBuilder::initShaderName(Common::UString &name) {
	name = "xoreos.";
}

void ShaderBuilder::addShaderName(Common::UString &name, uint32 passType, uint32 blendType) {
	switch (passType) {
	case ShaderBuilder::ENV_CUBE:
		name += "env_cube.";
		break;
	case ShaderBuilder::ENV_SPHERE:
		name += "env_sphere.";
		break;
	case ShaderBuilder::COLOUR:
		name += "colour.";
		break;
	case ShaderBuilder::TEXTURE:
		name += "texture.";
		break;
	case ShaderBuilder::TEXTURE_LIGHTMAP:
		name += "lightmap.";
		break;
	case ShaderBuilder::TEXTURE_BUMPMAP:
		name += "texture_bumpmap.";
		break;
	case ShaderBuilder::TEXTURE_LIGHTMAP_BUMPMAP:
		name += "texture_lightmap_bumpmap.";
		break;
	case ShaderBuilder::FORCE_OPAQUE:
		name += "force_opaque.";
		break;
	default: break;
	}

	switch (blendType) {
	case ShaderBuilder::BLEND_SRC_ALPHA:
		name += "blend_src_alpha.";
		break;
	case ShaderBuilder::BLEND_DST_ALPHA:
		name += "blend_dst_alpha.";
		break;
	case ShaderBuilder::BLEND_ZERO:
		name += "blend_zero.";
		break;
	case ShaderBuilder::BLEND_ONE:
		name += "blend_one.";
		break;
	case ShaderBuilder::BLEND_IGNORED:
		name += "blend_ignored.";
		break;
	default: break;
	}
}

void ShaderBuilder::finaliseShaderNameVertex(Common::UString &name) {
	name += "vert";
}

void ShaderBuilder::finaliseShaderNameFragment(Common::UString &name) {
	name += "frag";
}

void ShaderBuilder::initVertexShaderString(Common::UString &header, Common::UString &body, bool isGL3) {
	if (isGL3) {
		header = header_default_3vert;
		body = body_default_start_3vert;
	} else {
		header = header_default_2vert;
		body = body_default_start_2vert;
	}
}

void ShaderBuilder::addVertexShaderString(Common::UString &header, Common::UString &body, uint32 passType, uint32 UNUSED(blendType), bool isGL3) {
	if (isGL3) {
		switch (passType) {
		case ShaderBuilder::ENV_CUBE:
			header += header_envcube_3vert;
			body += body_envcube_3vert;
			break;
		case ShaderBuilder::ENV_SPHERE:
			header += header_envsphere_3vert;
			body += body_envsphere_3vert;
			break;
		case ShaderBuilder::COLOUR:
			header += header_colour_3vert;
			body += body_colour_3vert;
			break;
		case ShaderBuilder::TEXTURE:
			header += header_texture_3vert;
			body += body_texture_3vert;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP:
//			header += header_texture_3vert;
			header += header_lightmap_3vert;
//			body += body_texture_3vert;
			body += body_lightmap_3vert;
			break;
		case ShaderBuilder::TEXTURE_BUMPMAP:
			header += header_lightmap_3vert;
			body += body_lightmap_3vert;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP_BUMPMAP:
			header += header_lightmap_3vert;
			body += body_lightmap_3vert;
			break;
		case ShaderBuilder::FORCE_OPAQUE:
			break;
		default: break;
		}
	} else {
		switch (passType) {
		case ShaderBuilder::ENV_CUBE:
			header += header_envcube_2vert;
			body += body_envcube_2vert;
			break;
		case ShaderBuilder::ENV_SPHERE:
			header += header_envsphere_2vert;
			body += body_envsphere_2vert;
			break;
		case ShaderBuilder::COLOUR:
			header += header_colour_2vert;
			body += body_colour_2vert;
			break;
		case ShaderBuilder::TEXTURE:
			header += header_texture_2vert;
			body += body_texture_2vert;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP:
//			header += header_texture_2vert;
			header += header_lightmap_2vert;
//			body += body_texture_2vert;
			body += body_lightmap_2vert;
			break;
		case ShaderBuilder::TEXTURE_BUMPMAP:
			header += header_texture_2vert;
			header += header_lightmap_2vert;
			body += body_texture_2vert;
			body += body_lightmap_2vert;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP_BUMPMAP:
			header += header_lightmap_2vert;
			body += body_lightmap_2vert;
			break;
		case ShaderBuilder::FORCE_OPAQUE:
			break;
		default: break;
		}
	}
}

void ShaderBuilder::finaliseVertexShaderString(Common::UString &combined, Common::UString &header, Common::UString &body, bool isGL3) {
	if (isGL3) {
		body += body_default_end_3vert;

	} else {
		body += body_default_end_2vert;
	}

	combined = header + body;
}

void ShaderBuilder::initFragmentShaderString(Common::UString &header, Common::UString &body, bool isGL3) {
	if (isGL3) {
		header = header_default_3frag;
		body = body_default_start_3frag;
	} else {
		header = header_default_2frag;
		body = body_default_start_2frag;
	}
}

void ShaderBuilder::addFragmentShaderString(Common::UString &header, Common::UString &body, uint32 passType, uint32 blendType, bool isGL3) {
	if (isGL3) {
		switch (passType) {
		case ShaderBuilder::ENV_CUBE:
			header += header_envcube_3frag;
			body += body_envcube_3frag;
			break;
		case ShaderBuilder::ENV_SPHERE:
			header += header_envsphere_3frag;
			body += body_envsphere_3frag;
			break;
		case ShaderBuilder::COLOUR:
			header += header_colour_3frag;
			body += body_colour_3frag;
			break;
		case ShaderBuilder::TEXTURE:
			header += header_texture_3frag;
			body += body_texture_3frag;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP:
//			header += header_texture_3frag;
			header += header_lightmap_3frag;
//			body += body_texture_3frag;
			body += body_lightmap_3frag;
			break;
		case ShaderBuilder::TEXTURE_BUMPMAP:
			header += header_lightmap_3frag;
			body += body_lightmap_3frag;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP_BUMPMAP:
			header += header_lightmap_3frag;
			body += body_lightmap_3frag;
			break;
		case ShaderBuilder::FORCE_OPAQUE:
			body += body_force_opaque_3frag;
			break;
		default: break;
		}

		switch (blendType) {
		case ShaderBuilder::BLEND_SRC_ALPHA:
			body += body_blend_src_alpha_3frag;
			break;
		case ShaderBuilder::BLEND_DST_ALPHA:
			body += body_blend_dst_alpha_3frag;
			break;
		case ShaderBuilder::BLEND_ZERO:
			body += body_blend_zero_3frag;
			break;
		case ShaderBuilder::BLEND_ONE:
			body += body_blend_one_3frag;
			break;
		case ShaderBuilder::BLEND_MULTIPLY:
			body += body_blend_multiply_3frag;
			break;
		case ShaderBuilder::BLEND_IGNORED:
			break;
		default: break;
		}

	} else {
		switch (passType) {
		case ShaderBuilder::ENV_CUBE:
			header += header_envcube_2frag;
			body += body_envcube_2frag;
			break;
		case ShaderBuilder::ENV_SPHERE:
			header += header_envsphere_2frag;
			body += body_envsphere_2frag;
			break;
		case ShaderBuilder::COLOUR:
			header += header_colour_2frag;
			body += body_colour_2frag;
			break;
		case ShaderBuilder::TEXTURE:
			header += header_texture_2frag;
			body += body_texture_2frag;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP:
//			header += header_texture_2frag;
			header += header_lightmap_2frag;
//			body += body_texture_2frag;
			body += body_lightmap_2frag;
			break;
		case ShaderBuilder::TEXTURE_BUMPMAP:
			header += header_texture_2frag;
			header += header_lightmap_2frag;
			body += body_texture_2frag;
			body += body_lightmap_2frag;
			break;
		case ShaderBuilder::TEXTURE_LIGHTMAP_BUMPMAP:
			header += header_lightmap_2frag;
			body += body_lightmap_2frag;
			break;
		case ShaderBuilder::FORCE_OPAQUE:
			body += body_force_opaque_2frag;
			break;
		default: break;
		}

		switch (blendType) {
		case ShaderBuilder::BLEND_SRC_ALPHA:
			body += body_blend_src_alpha_2frag;
			break;
		case ShaderBuilder::BLEND_DST_ALPHA:
			body += body_blend_dst_alpha_2frag;
			break;
		case ShaderBuilder::BLEND_ZERO:
			body += body_blend_zero_2frag;
			break;
		case ShaderBuilder::BLEND_ONE:
			body += body_blend_one_2frag;
			break;
		case ShaderBuilder::BLEND_MULTIPLY:
			body += body_blend_multiply_2frag;
			break;
		case ShaderBuilder::BLEND_IGNORED:
			break;
		default: break;
		}
	}
}

void ShaderBuilder::finaliseFragmentShaderString(Common::UString &combined, Common::UString &header, Common::UString &body, bool isGL3) {
	if (isGL3) {
		body += body_default_end_3frag;

	} else {
		body += body_default_end_2frag;
	}

	combined = header + body;
}

} // End of namespace Shader

} // End of namespace Graphics
