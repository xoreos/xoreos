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
 *  Shader sources for internal shaders.
 */

#include "src/graphics/shader/shadercode.h"

namespace Graphics {

namespace Shader {
// ---------------------------------------------------------
const char vertexDefault3xText[] =
"#version 330\n\
\n\
layout(location = 0) in vec3 inPosition;\n\
layout(location = 1) in vec3 inNormal;\n\
layout(location = 3) in vec2 inTexCoord0;\n\
\n\
out vec3 frag_normal;\n\
out vec2 texCoords;\n\
\n\
uniform mat4 objectModelviewMatrix;\n\
\n\
uniform mat4 projectionMatrix;\n\
uniform mat4 modelviewMatrix;\n\
\n\
void main(void) {\n\
	mat4 mo = modelviewMatrix * objectModelviewMatrix;\n\
	vec4 vertex = mo * vec4(inPosition, 1.0f);\n\
\n\
	gl_Position = projectionMatrix * vertex;\n\
	frag_normal = mat3(mo) * inNormal;\n\
	texCoords = inTexCoord0;\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentDefault3xText[] =
"#version 330\n\
precision highp float;\n\
\n\
uniform sampler2D texture0;\n\
uniform float alpha;\n\
\n\
in vec2 texCoords;\n\
\n\
layout(location = 0) out vec4 outColor;\n\
\n\
void main(void) {\n\
	outColor = texture(texture0, texCoords) * vec4(1.0, 1.0, 1.0f, alpha);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentColor3xText[] =
"#version 330\n\
precision highp float;\n\
\n\
uniform vec4 color;\n\
uniform float alpha;\n\
\n\
void main(void) {\n\
	gl_FragColor = color * vec4(1.0, 1.0, 1.0f, alpha);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentEnv3xText[] =
"#version 330\n\
precision highp float;\n\
\n\
uniform sampler2D texture0;\n\
uniform samplerCube textureCube\n\
uniform float alpha;\n\
\n\
in vec3 frag_normal;\n\
in vec2 texCoords;\n\
\n\
layout(location = 0) out vec4 outColor;\n\
\n\
void main(void) {\n\
	vec4 diffuse = texture(texture0, texCoords);\n\
	outColor = texture(texture0, texCoords) * vec4(1.0, 1.0, 1.0f, alpha);\n\
}\n\
";
// ---------------------------------------------------------


// ---------------------------------------------------------
const char vertexDefault2xText[] =
"#version 120\n\
\n\
varying vec3 _normal;\n\
varying vec3 _position;\n\
varying vec2 _texCoords;\n\
\n\
uniform mat4 _objectModelviewMatrix;\n\
uniform mat4 _projectionMatrix;\n\
uniform mat4 _modelviewMatrix;\n\
\n\
void main(void) {\n\
	mat4 mo = (_modelviewMatrix * _objectModelviewMatrix);\n\
	vec4 vertex = mo * gl_Vertex;\n\
\n\
	gl_Position = _projectionMatrix * vertex;\n\
	_normal = mat3(mo) * vec3(vertex);\n\
	_position = vec3(_objectModelviewMatrix * gl_Vertex);\n\
	_texCoords = vec2(gl_MultiTexCoord0);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentDefault2xText[] =
"#version 120\n\
\n\
uniform sampler2D _texture0;\n\
uniform float _alpha;\n\
\n\
varying vec2 _texCoords;\n\
\n\
void main(void) {\n\
	gl_FragColor = texture2D(_texture0, _texCoords) * vec4(1.0, 1.0, 1.0f, _alpha);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentColor2xText[] =
"#version 120\n\
\n\
uniform vec4 _color;\n\
uniform float _alpha;\n\
\n\
varying vec2 _texCoords;\n\
void main(void) {\n\
	gl_FragColor = _color * vec4(1.0, 1.0, 1.0f, _alpha);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentEnv2xText[] =
"#version 120\n\
\n\
//uniform sampler2D texture0;\n\
uniform sampler2D _textureCube0;\n\
uniform float _alpha;\n\
\n\
varying vec3 _normal;\n\
varying vec3 _position;\n\
varying vec2 _texCoords;\n\
\n\
void main(void) {\n\
	vec3 incident = normalize(_position);\n\
	vec3 ray = reflect(incident, normalize(_normal));\n\
//	vec4 diffuse = texture2D(texture0, texCoords);\n\
//	gl_FragColor = textureCube(textureCube0, ray) * vec4(1.0, 1.0, 1.0f, alpha);\n\
//	gl_FragColor = textureCube(textureCube0, vec3(texCoords, 1.0f)) * vec4(1.0, 1.0, 1.0f, alpha);\n\
	gl_FragColor = texture2D(_textureCube0, _texCoords);\n\
}\n\
";
// ---------------------------------------------------------

} // End of namespace Shader

} // End of namespace Graphics
