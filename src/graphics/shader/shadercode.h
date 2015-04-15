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

#ifndef GRAPHICS_SHADER_SHADERCODE_H
#define GRAPHICS_SHADER_SHADERCODE_H

#include "src/common/types.h"

namespace Graphics {

namespace Shader {

extern const char vertexDefault3xText[];
extern const char fragmentDefault3xText[];
extern const char fragmentColor3xText[];

extern const char vertexDefault2xText[];
extern const char fragmentDefault2xText[];
extern const char fragmentColor2xText[];

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERCODE_H
