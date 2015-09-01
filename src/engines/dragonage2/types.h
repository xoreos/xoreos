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
 *  Basic Dragon Age II types.
 */

#ifndef ENGINES_DRAGONAGE2_TYPES_H
#define ENGINES_DRAGONAGE2_TYPES_H

namespace Engines {

namespace DragonAge2 {

enum TextureQuality {
	kTextureQualityHigh   = 0,
	kTextureQualityMedium,

	kTextureQualityMAX
};

/** Multiple GDA as defined by m2da_*.gda. */
enum Worksheet {
	kWorksheetAnimations       =      0,
	kWorksheetAbilities        =      1,
	kWorksheetAppearances      =      2,
	kWorksheetClasses          =      3,
	kWorksheetProjectiles      =      4,
	kWorksheetRaces            =      5,
	kWorksheetPortraits        =      7,
	kWorksheetVisualEffects    =      8,
	kWorksheetWeaponTrailTypes =      9,
	kWorksheetSounds           =     10,
	kWorksheetLargeAppearances =     80,
	kWorksheetBodyParts        =     90,
	kWorksheetWaypoints        =    106,
	kWorksheetPlaceables       =  11101,
	kWorksheetItems            =  12000,
	kWorksheetItemVariations   =  12004
};

/** Object type, matches the bitfield in script.ldf */
enum ObjectType {
	kObjectTypeInvalid            = 0,

	kObjectTypeGUI                = 1 <<  0,
	kObjectTypeTile               = 1 <<  1,
	kObjectTypeModule             = 1 <<  2,
	kObjectTypeArea               = 1 <<  3,
	kObjectTypeStore              = 1 <<  4,
	kObjectTypeCreature           = 1 <<  5,
	kObjectTypeItem               = 1 <<  6,
	kObjectTypeTrigger            = 1 <<  7,
	kObjectTypeProjectile         = 1 <<  8,
	kObjectTypePlaceable          = 1 <<  9,
	kObjectTypeAreaOfEffectObject = 1 << 11,
	kObjectTypeWaypoint           = 1 << 12,
	kObjectTypeSound              = 1 << 14,
	kObjectTypeParty              = 1 << 15,
	kObjectTypeMapPatch           = 1 << 16,
	kObjectTypeVfx                = 1 << 17,
	kObjectTypeMap                = 1 << 18,

	kObjectTypeMAX,
	kObjectTypeAll                = 0xFFFFFFFF,

	kObjectTypeSelf               = 1 << 31  ///< Fake value to describe the calling object in a script.
};

/** Slot in a creature's inventory. */
enum InventorySlot {
	kInventorySlotMain           =  0,
	kInventorySlotOffhand        =  1,
	kInventorySlotRangedAmmo     =  2,
	kInventorySlotChest          =  4,
	kInventorySlotHead           =  5,
	kInventorySlotBoots          =  6,
	kInventorySlotGloves         =  7,
	kInventorySlotCloak          =  8,
	kInventorySlotRing1          =  9,
	kInventorySlotRing2          = 10,
	kInventorySlotNeck           = 11,
	kInventorySlotBelt           = 12,
	kInventorySlotBite           = 13,
	kInventorySlotShaleShoulders = 14,
	kInventorySlotShaleChest     = 15,
	kInventorySlotShaleRightArm  = 16,
	kInventorySlotShaleLeftArm   = 17,
	kInventorySlotDogWarpaint    = 18,
	kInventorySlotDogCollar      = 19,
	kInventorySlotPartyMember    = 20,

	kInventorySlotInvalid        = 255
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_TYPES_H
