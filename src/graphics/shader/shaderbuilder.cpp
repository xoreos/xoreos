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

void ShaderBuilder::addVertexShaderString(Common::UString &header, Common::UString &body, uint32 passType, uint32 blendType, bool isGL3) {
	UNUSED(blendType);  // Kept as a parameter for now, for consistency and possible future changes.
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
