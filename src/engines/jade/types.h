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
 *  Basic Jade Empire type definitions.
 */

#ifndef ENGINES_JADE_TYPES_H
#define ENGINES_JADE_TYPES_H

namespace Engines {

namespace Jade {

enum ObjectType {
	kObjectTypeInvalid         =  0,
	kObjectTypeContainer       =  1,
	kObjectTypeCreature        =  2,
	kObjectTypeDoor            =  3,
	kObjectTypeTrigger         =  4,
	kObjectTypePlayerCreature  =  5,
	kObjectTypeArea            =  7,
	kObjectTypeProjectile      =  8,
	kObjectTypePlaceable       =  9,
	kObjectTypeAreaOfEffect    = 10,
	kObjectTypeWaypoint        = 11,
	kObjectTypeSound           = 12,
	kObjectTypeCrowd           = 13,
	kObjectTypeMAX,

	kObjectTypeSelf         = 0x7FFE,
	kObjectTypeAll          = 0x7FFF
};

} // End of namespace Jade

} // End of namespace Engines

#endif /* ENGINES_JADE_TYPES_H */
