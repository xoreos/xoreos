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
 *  Basic Aurora graphics types.
 */

#ifndef GRAPHICS_AURORA_TYPES_H
#define GRAPHICS_AURORA_TYPES_H

#include <map>

#include "src/common/ptrmap.h"
#include "src/common/ustring.h"

#include "src/graphics/types.h"

namespace Graphics {

namespace Aurora {

/** Identifier used for the monospaced system font. */
extern const char *kSystemFontMono;

const float kHAlignLeft   = 0.0f;
const float kHAlignCenter = 0.5f;
const float kHAlignRight  = 1.0f;

const float kVAlignTop     = 1.0f;
const float kVAlignMiddle  = 0.5f;
const float kVAlignBottom  = 0.0f;

/** The display type of a model. */
enum ModelType {
	kModelTypeObject   = kRenderableTypeObject,  ///< A real object in the game world.
	kModelTypeGUIFront = kRenderableTypeGUIFront ///< An element of the front GUI.
};

class Model;
class ModelNode;
class Text;
class GUIQuad;

typedef Common::PtrMap<Common::UString, class Model, Common::UString::iless> ModelCache;

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TYPES_H
