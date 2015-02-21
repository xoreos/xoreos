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
 *  Basic NWN2 type definitions.
 */

#ifndef ENGINES_NWN2_TYPES_H
#define ENGINES_NWN2_TYPES_H

#include "src/common/types.h"

namespace Engines {

namespace NWN2 {

enum ObjectType {
	kObjectTypeNone         = 0     ,
	kObjectTypeCreature     = 1 << 0,
	kObjectTypeItem         = 1 << 1,
	kObjectTypeTrigger      = 1 << 2,
	kObjectTypeDoor         = 1 << 3,
	kObjectTypeAreaOfEffect = 1 << 4,
	kObjectTypeWaypoint     = 1 << 5,
	kObjectTypePlaceable    = 1 << 6,
	kObjectTypeStore        = 1 << 7,
	kObjectTypeEncounter    = 1 << 8,
	kObjectTypeAll          = 0x7FFF
};

enum Ability {
	kAbilityStrength     = 0,
	kAbilityDexterity    = 1,
	kAbilityConstitution = 2,
	kAbilityIntelligence = 3,
	kAbilityWisdom       = 4,
	kAbilityCharisma     = 5,
	kAbilityMAX
};

enum Gender {
	kGenderMale   = 0,
	kGenderFemale = 1,
	kGenderBoth   = 2,
	kGenderOther  = 3,
	kGenderNone   = 4
};

static const uint32 kObjectTypeInvalid = 0x7FFF;

static const uint32 kRaceInvalid    =  28;
static const uint32 kSubRaceInvalid = 255;
static const uint32 kClassInvalid   = 255;

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_TYPES_H
