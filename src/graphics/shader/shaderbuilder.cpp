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
"	_normal = mat3(mo) * vec3(vertex);\n"
"	_position = vec3(_objectModelviewMatrix * gl_Vertex);\n"
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
"	vec4 texcube0_diffuse = textureCube(_textureCube0, vec3(_texCoords, 1.0f));\n"
"	fraggle = mix(fraggle, texcube0_diffuse, texcube0_diffuse.a);\n";

static const Common::UString body_texsphere_2frag =
"	vec4 texsphere0_diffuse = texture2D(_textureSphere0, _texCoords);\n"
"	fraggle = mix(fraggle, texsphere0_diffuse, texsphere0_diffuse.a);\n";


ShaderBuilder::ShaderBuilder() {
	this->genVertexShader(0);
}

ShaderBuilder::~ShaderBuilder() {
}

Common::UString ShaderBuilder::genVertexShader(uint32_t flags) {
	return header_default_2vert + body_default_2vert;
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

Common::UString ShaderBuilder::genFragmentShader(uint32_t flags) {
	Common::UString header = header_default_2frag;
	Common::UString body = body_default_start_2frag;
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
