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
 *  Basic Dragon Age: Origins types.
 */

#ifndef ENGINES_DRAGONAGE_TYPES_H
#define ENGINES_DRAGONAGE_TYPES_H

namespace Engines {

namespace DragonAge {

/** Object type, matches the bitfield in script.ldf */
enum ObjectType {
	kObjectTypeGUI                = 0,
	kObjectTypeTile               = 1,
	kObjectTypeModule             = 2,
	kObjectTypeArea               = 3,
	kObjectTypeStore              = 4,
	kObjectTypeCreature           = 5,
	kObjectTypeItem               = 6,
	kObjectTypeTrigger            = 7,
	kObjectTypeProjectile         = 8,
	kObjectTypePlaceable          = 9,
	kObjectTypeAreaOfEffectObject = 11,
	kObjectTypeWaypoint           = 12,
	kObjectTypeSound              = 14,
	kObjectTypeParty              = 15,
	kObjectTypeMapPatch           = 16,
	kObjectTypeVfx                = 17,
	kObjectTypeMap                = 18,

	kObjectTypeMAX,
	kObjectTypeInvalid            = 0xFFFFFFFF
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_TYPES_H
