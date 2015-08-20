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
 *  Basic Neverwinter Nights 2 type definitions.
 */

#ifndef ENGINES_NWN2_TYPES_H
#define ENGINES_NWN2_TYPES_H

#include "src/common/types.h"

namespace Engines {

namespace NWN2 {

/** Object type, matches the bitfield in nwscript.nss */
enum ObjectType {
	kObjectTypeCreature     =  1,
	kObjectTypeItem         =  2,
	kObjectTypeTrigger      =  3,
	kObjectTypeDoor         =  4,
	kObjectTypeAreaOfEffect =  5,
	kObjectTypeWaypoint     =  6,
	kObjectTypePlaceable    =  7,
	kObjectTypeStore        =  8,
	kObjectTypeEncounter    =  9,
	kObjectTypeLight        = 10,
	kObjectTypePlacedEffect = 11,

	kObjectTypeMAX,

	kObjectTypeModule       = 0x7FFC, ///< Fake value for a module object.
	kObjectTypeArea         = 0x7FFD, ///< Fake value for an area object.
	kObjectTypeSelf         = 0x7FFE, ///< Fake value to describe the calling object in a script.
	kObjectTypeInvalid      = 0x7FFF,

	kObjectTypeAll          = 0x7FFF
};

enum Script {
	kScriptAcquireItem       = 0,
	kScriptActiveItem           ,
	kScriptAttacked             ,
	kScriptBlocked              ,
	kScriptClick                ,
	kScriptClient               ,
	kScriptClosed               ,
	kScriptCutsceneAbort        ,
	kScriptDamaged              ,
	kScriptDeath                ,
	kScriptDialogue             ,
	kScriptDisarm               ,
	kScriptDisturbed            ,
	kScriptEndRound             ,
	kScriptEnter                ,
	kScriptExhausted            ,
	kScriptExit                 ,
	kScriptFailToOpen           ,
	kScriptHeartbeat            ,
	kScriptLock                 ,
	kScriptModuleLoad           ,
	kScriptModuleStart          ,
	kScriptNotice               ,
	kScriptOpen                 ,
	kScriptPCLoaded             ,
	kScriptPlayerChat           ,
	kScriptPlayerDeath          ,
	kScriptPlayerDying          ,
	kScriptPlayerEquipItem      ,
	kScriptPlayerLevelUp        ,
	kScriptPlayerRespanw        ,
	kScriptPlayerRest           ,
	kScriptPlayerUnequipItem    ,
	kScriptRested               ,
	kScriptShout                ,
	kScriptSpawn                ,
	kScriptSpellCastAt          ,
	kScriptStoreClose           ,
	kScriptStoreOpen            ,
	kScriptTrapTriggered        ,
	kScriptUnaquireItem         ,
	kScriptUnlock               ,
	kScriptUsed                 ,
	kScriptUserdefined          ,
	kScriptMAX
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

static const uint32 kRaceInvalid    =  28;
static const uint32 kSubRaceInvalid = 255;
static const uint32 kClassInvalid   = 255;

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_TYPES_H
