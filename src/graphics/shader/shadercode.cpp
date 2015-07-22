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
layout(location = 3) in vec2 inTexCoord0;\n\
\n\
out vec2 texCoords;\n\
\n\
uniform mat4 objectModelviewMatrix;\n\
\n\
uniform mat4 projectionMatrix;\n\
uniform mat4 modelviewMatrix;\n\
\n\
void main(void) {\n\
  vec4 vertex = (modelviewMatrix * objectModelviewMatrix) * vec4(inPosition, 1.0f);\n\
\n\
  gl_Position = projectionMatrix * vertex;\n\
  texCoords = inTexCoord0;\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentDefault3xText[] =
"#version 330\n\
precision highp float;\n\
\n\
uniform sampler2D texture0;\n\
\n\
in vec2 texCoords;\n\
\n\
layout(location = 0) out vec4 outColor;\n\
\n\
void main(void) {\n\
  outColor = texture(texture0, texCoords);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentColor3xText[] =
"#version 330\n\
precision highp float;\n\
\n\
uniform vec4 color;\n\
\n\
void main(void) {\n\
  gl_FragColor = color;\n\
}\n\
";
// ---------------------------------------------------------


// ---------------------------------------------------------
const char vertexDefault2xText[] =
"#version 120\n\
\n\
varying vec2 texCoords;\n\
\n\
uniform mat4 objectModelviewMatrix;\n\
uniform mat4 projectionMatrix;\n\
uniform mat4 modelviewMatrix;\n\
\n\
void main(void) {\n\
  vec4 vertex = (modelviewMatrix * objectModelviewMatrix) * gl_Vertex;\n\
\n\
  gl_Position = projectionMatrix * vertex;\n\
  texCoords = vec2(gl_MultiTexCoord0);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentDefault2xText[] =
"#version 120\n\
\n\
uniform sampler2D texture0;\n\
\n\
varying vec2 texCoords;\n\
\n\
void main(void) {\n\
  gl_FragColor = texture2D(texture0, texCoords);\n\
}\n\
";
// ---------------------------------------------------------
const char fragmentColor2xText[] =
"#version 120\n\
\n\
uniform vec4 color;\n\
\n\
void main(void) {\n\
  gl_FragColor = color;\n\
}\n\
";
// ---------------------------------------------------------

} // End of namespace Shader

} // End of namespace Graphics
