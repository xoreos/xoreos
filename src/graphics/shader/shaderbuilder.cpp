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

namespace Graphics {

namespace Shader {


// ---------------------------------------------------------
static const Common::UString header_default_3vert =
"#version 330\n\n"
"layout(location = 0) in vec3 inPosition;\n"
"layout(location = 1) in vec3 inNormal;\n"
"layout(location = 3) in vec2 inTexCoord0;\n"
"out vec3 _normal;\n"
"out vec3 _position;\n"
"out vec2 _texCoords;\n"
"uniform mat4 _objectModelviewMatrix;\n"
"uniform mat4 _projectionMatrix;\n"
"uniform mat4 _modelviewMatrix;\n";

static const Common::UString header_default_3frag =
"#version 330\n\n"
"precision highp float;\n\n"
"uniform float _alpha;\n";

static const Common::UString header_colour_3frag =
"uniform vec4 _color;\n";

static const Common::UString header_texture_3frag =
"uniform sampler2D _texture0;\n";

static const Common::UString header_texcube_3frag =
"uniform samplerCube _textureCube0;\n";

static const Common::UString header_texsphere_3frag =
"uniform sampler2D _textureSphere0;\n";


static const Common::UString body_default_3vert =
"void main(void) {\n"
"	mat mo = (_modelviewMatrix * _objectModelviewMatrix);\n"
"	vec4 vertex = mo * vec4(inPosition, 1.0f);\n"
"	gl_Position = _projectionMatrix * vertex;\n"
"	_normal = mat3(mo) * vec3(inNormal);\n"
"	_position = vec3(vertex);\n"
"	_texCoords = inTexCoord0;\n"
"}\n";

static const Common::UString body_default_start_3frag =
"in vec3 _normal;\n"
"in vec3 _position;\n"
"in vec2 _texCoords;\n\n"
"layout(location = 0) out vec4 outColor;\n"
"void main(void) {\n"
"	vec4 fraggle = vec4(0.0, 0.0, 0.0, 0.0);\n"
"	float opacity = 1.0;\n";

static const Common::UString body_default_end_3frag =
"	fraggle.a = opacity * _alpha;\n"
"	outColor = fraggle;\n"
"}\n";

static const Common::UString body_colour_3frag =
"	opacity *= _color.a;\n"
"	fraggle = mix(fraggle, _color, _color.a);\n";

static const Common::UString body_texture_3frag =
"	vec4 texture0_diffuse = texture(_texture0, _texCoords);\n"
"	opacity *= texture0_diffuse.a;\n"
"	fraggle = mix(fraggle, texture0_diffuse, texture0_diffuse.a);\n";

static const Common::UString body_texcube_3frag =
"	vec3 u = normalize(_position);\n"
"	vec3 n = normalize(_normal);\n"
"	vec3 r = reflect(u, n);\n"
"	vec4 texcube0_diffuse = texture(_textureCube0, r);\n"
"	fraggle = mix(fraggle, texcube0_diffuse, texcube0_diffuse.a);\n";

static const Common::UString body_texsphere_3frag =
"	vec3 u = normalize(_position);\n"
"	vec3 n = normalize(_normal);\n"
"	vec3 r = reflect(u, n);\n"
"	float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));\n"
"	vec2 coords = vec2(r.x / m + 0.5, r.y / m + 0.5);\n"
"	vec4 texsphere0_diffuse = texture(_textureSphere0, coords);\n"
"	fraggle = mix(fraggle, texsphere0_diffuse, texsphere0_diffuse.a);\n";
// ---------------------------------------------------------



// ---------------------------------------------------------
static const Common::UString header_default_2vert =
"#version 120\n"
"varying vec3 _normal;\n"
"varying vec3 _position;\n"
"varying vec2 _texCoords;\n"
"uniform mat4 _objectModelviewMatrix;\n"
"uniform mat4 _projectionMatrix;\n"
"uniform mat4 _modelviewMatrix;\n";

static const Common::UString header_default_2frag =
"#version 120\n\n"
"uniform float _alpha;\n";

static const Common::UString header_colour_2frag =
"uniform vec4 _color;\n";

static const Common::UString header_texture_2frag =
"uniform sampler2D _texture0;\n";

static const Common::UString header_texcube_2frag =
"uniform samplerCube _textureCube0;\n";

static const Common::UString header_texsphere_2frag =
"uniform sampler2D _textureSphere0;\n";


static const Common::UString body_default_2vert =
"void main(void) {\n"
"	mat mo = (_modelviewMatrix * _objectModelviewMatrix);\n"
"	vec4 vertex = mo * gl_Vertex;\n"
"	gl_Position = _projectionMatrix * vertex;\n"
"	_normal = mat3(mo) * vec3(gl_Normal);\n"
"	_position = vec3(vertex);\n"
"	_texCoords = vec2(gl_MultiTexCoord0);\n"
"}\n";

static const Common::UString body_default_start_2frag =
"varying vec3 _normal;\n"
"varying vec3 _position;\n"
"varying vec2 _texCoords;\n\n"
"void main(void) {\n"
"	vec4 fraggle = vec4(0.0, 0.0, 0.0, 0.0);\n"
"	float opacity = 1.0;\n";

static const Common::UString body_default_end_2frag =
"	fraggle.a = opacity * _alpha;\n"
"	gl_FragColor = fraggle;\n"
"}\n";

static const Common::UString body_colour_2frag =
"	opacity *= _color.a;\n"
"	fraggle = mix(fraggle, _color, _color.a);\n";

static const Common::UString body_texture_2frag =
"	vec4 texture0_diffuse = texture2D(_texture0, _texCoords);\n"
"	opacity *= texture0_diffuse.a;\n"
"	fraggle = mix(fraggle, texture0_diffuse, texture0_diffuse.a);\n";

static const Common::UString body_texcube_2frag =
"	vec3 u = normalize(_position);\n"
"	vec3 n = normalize(_normal);\n"
"	vec3 r = reflect(u, n);\n"
"	vec4 texcube0_diffuse = textureCube(_textureCube0, r);\n"
//"	vec4 texcube0_diffuse = texture2D(_textureCube0, _texCoords);"
//"	fraggle = texcube0_diffuse;\n";
"	fraggle = vec4(1.0, 0.0, 0.0, 1.0);\n";
//"	fraggle = mix(fraggle, texcube0_diffuse, texcube0_diffuse.a);\n";

static const Common::UString body_texsphere_2frag =
"	vec3 u = normalize(_position);\n"
"	vec3 n = normalize(_normal);\n"
"	vec3 r = reflect(u, n);\n"
"	float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));\n"
"	vec2 coords = vec2(r.x / m + 0.5, r.y / m + 0.5);\n"
"	vec4 texsphere0_diffuse = texture2D(_textureSphere0, coords);\n"
"	fraggle = mix(fraggle, texsphere0_diffuse, texsphere0_diffuse.a);\n";
// ---------------------------------------------------------

ShaderBuilder::ShaderBuilder() {
}

ShaderBuilder::~ShaderBuilder() {
}

Common::UString ShaderBuilder::genVertexShader(uint32_t flags, bool isGL3) {
	if (isGL3) {
		return header_default_3vert + body_default_3vert;
	} else {
		return header_default_2vert + body_default_2vert;
	}
}

Common::UString ShaderBuilder::genVertexShaderName(uint32_t flags) {
	Common::UString name = "";
	if (flags & ShaderBuilder::ENV_CUBE_PRE) {
		name += "env_cube_pre.";
	}

	if (flags & ShaderBuilder::ENV_SPHERE_PRE) {
		name += "env_sphere_pre.";
	}

	if (flags & ShaderBuilder::COLOUR) {
		name += "colour.";
	}

	if (flags & ShaderBuilder::TEXTURE) {
		name += "texture.";
	}

	if (flags & ShaderBuilder::ENV_CUBE_POST) {
		name += "env_cube_post.";
	}

	if (flags & ShaderBuilder::ENV_SPHERE_POST) {
		name += "env_sphere_post.";
	}

	name += "vert";

	return name;
}

Common::UString ShaderBuilder::genFragmentShader(uint32_t flags, bool isGL3) {
	Common::UString header;
	Common::UString body;

	if (isGL3) {
		header = header_default_3frag;
		body = body_default_start_3frag;

		if (flags & ShaderBuilder::ENV_CUBE_PRE) {
			header += header_texcube_3frag;
			body += body_texcube_3frag;
		}

		if (flags & ShaderBuilder::ENV_SPHERE_PRE) {
			header += header_texsphere_3frag;
			body += body_texsphere_3frag;
		}

		if (flags & ShaderBuilder::COLOUR) {
			header += header_colour_3frag;
			body += body_colour_3frag;
		}

		if (flags & ShaderBuilder::TEXTURE) {
			header += header_texture_3frag;
			body += body_texture_3frag;
		}

		if (flags & ShaderBuilder::ENV_CUBE_POST) {
			header += header_texcube_3frag;
			body += body_texcube_3frag;
		}

		if (flags & ShaderBuilder::ENV_SPHERE_POST) {
			header += header_texsphere_3frag;
			body += body_texsphere_3frag;
		}

		body += body_default_end_3frag;
	} else {
		header = header_default_2frag;
		body = body_default_start_2frag;

		if (flags & ShaderBuilder::ENV_CUBE_PRE) {
			header += header_texcube_2frag;
			body += body_texcube_2frag;
		}

		if (flags & ShaderBuilder::ENV_SPHERE_PRE) {
			header += header_texsphere_2frag;
			body += body_texsphere_2frag;
		}

		if (flags & ShaderBuilder::COLOUR) {
			header += header_colour_2frag;
			body += body_colour_2frag;
		}

		if (flags & ShaderBuilder::TEXTURE) {
			header += header_texture_2frag;
			body += body_texture_2frag;
		}

		if (flags & ShaderBuilder::ENV_CUBE_POST) {
			header += header_texcube_2frag;
			body += body_texcube_2frag;
		}

		if (flags & ShaderBuilder::ENV_SPHERE_POST) {
			header += header_texsphere_2frag;
			body += body_texsphere_2frag;
		}

		body += body_default_end_2frag;
	}

	return header + body;
}

Common::UString ShaderBuilder::genFragmentShaderName(uint32_t flags) {
	Common::UString name = "";
	if (flags & ShaderBuilder::ENV_CUBE_PRE) {
		name += "env_cube_pre.";
	}

	if (flags & ShaderBuilder::ENV_SPHERE_PRE) {
		name += "env_sphere_pre.";
	}

	if (flags & ShaderBuilder::COLOUR) {
		name += "colour.";
	}

	if (flags & ShaderBuilder::TEXTURE) {
		name += "texture.";
	}

	if (flags & ShaderBuilder::ENV_CUBE_POST) {
		name += "env_cube_post.";
	}

	if (flags & ShaderBuilder::ENV_SPHERE_POST) {
		name += "env_sphere_post.";
	}

	name += "frag";

	return name;
}

} // End of namespace Shader

} // End of namespace Graphics
