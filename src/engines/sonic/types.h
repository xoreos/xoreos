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
 *  Basic Sonic Chronicles: The Dark Brotherhood types and defines.
 */

#ifndef ENGINES_SONIC_TYPES_H
#define ENGINES_SONIC_TYPES_H

#include "src/common/types.h"

namespace Engines {

namespace Sonic {

static const float kScreenWidth  = 256.0f;
static const float kScreenHeight = 192.0f;

static const float kTopScreenX = -128.0f;
static const float kTopScreenY =    0.0f;

static const float kBottomScreenX = -128.0f;
static const float kBottomScreenY = -192.0f;

static const float kCameraHeight = 50.0f;
static const float kCameraAngle  = 45.0f;

enum ObjectType {
	kObjectTypeInvalid      = 0,
	kObjectTypeModule       = 1U << 0,
	kObjectTypeArea         = 1U << 1,
	kObjectTypePlaceable    = 1U << 2,
	kObjectTypeItem         = 1U << 3,
	kObjectTypeCreature     = 1U << 4,

	kObjectTypeAll          = 0x7FFF,

	kObjectTypeMAX          = 0x7FFF
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_TYPES_H
