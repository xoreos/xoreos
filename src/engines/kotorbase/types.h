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
 *  Basic type definitions for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_TYPES_H
#define ENGINES_KOTORBASE_TYPES_H

namespace Engines {

namespace KotORBase {

/** Object type, matches the bitfield in nwscript.nss */
enum ObjectType {
	kObjectTypeCreature     = 1U << 0,
	kObjectTypeItem         = 1U << 1,
	kObjectTypeTrigger      = 1U << 2,
	kObjectTypeDoor         = 1U << 3,
	kObjectTypeAreaOfEffect = 1U << 4,
	kObjectTypeWaypoint     = 1U << 5,
	kObjectTypePlaceable    = 1U << 6,
	kObjectTypeStore        = 1U << 7,
	kObjectTypeEncounter    = 1U << 8,
	kObjectTypeSound        = 1U << 9,

	kObjectTypeInvalid      = 0x7FFF,
	kObjectTypeAll          = 0x7FFF,

	kObjectTypeMAX          = 0x7FFF,

	kObjectTypeArea         = 1U << 29, ///< Fake value for an area object.
	kObjectTypeModule       = 1U << 30, ///< Fake value for a module object.
	kObjectTypeSelf         = 1U << 31  ///< Fake value to describe the calling object in a script.
};

enum Script {
	kScriptAccelerate    =  0,
	kScriptActivateItem      ,
	kScriptAnimate           ,
	kScriptAquireItem        ,
	kScriptAttacked          ,
	kScriptBlocked           ,
	kScriptBreak             ,
	kScriptClick             ,
	kScriptClosed            ,
	kScriptCreate            ,
	kScriptDamaged           ,
	kScriptDeath             ,
	kScriptDialogue          ,
	kScriptDisarm            ,
	kScriptDisturbed         ,
	kScriptEndDialogue       ,
	kScriptEndRound          ,
	kScriptEnter             ,
	kScriptExhausted         ,
	kScriptExit              ,
	kScriptFailToOpen        ,
	kScriptFire              ,
	kScriptHeartbeart        ,
	kScriptHeartbeat         ,
	kScriptHitBullet         ,
	kScriptHitFollower       ,
	kScriptHitObstacle       ,
	kScriptHitWorld          ,
	kScriptLock              ,
	kScriptModuleLoad        ,
	kScriptModuleStart       ,
	kScriptNotice            ,
	kScriptOpen              ,
	kScriptOpenStore         ,
	kScriptPlayerDeath       ,
	kScriptPlayerDying       ,
	kScriptPlayerLevelUp     ,
	kScriptPlayerRespawn     ,
	kScriptPlayerRest        ,
	kScriptRest              ,
	kScriptSpawn             ,
	kScriptSpellCastAt       ,
	kScriptTrackLoop         ,
	kScriptTrapTriggered     ,
	kScriptUnaquireItem      ,
	kScriptUnlock            ,
	kScriptUsed              ,
	kScriptUserdefined       ,

	kScriptMAX
};

enum Gender {
	kGenderMale = 0,
	kGenderFemale,
	kGenderBoth,
	kGenderOther,
	kGenderNone
};


enum Skin {
	kSkinA = 0,
	kSkinB,
	kSkinC,
	kSkinH, // KotOR 2

	kSkinMAX
};

enum Class {
	kClassSoldier = 0,
	kClassScout,
	kClassScoundrel,
	kClassJediGuardian,
	kClassJediConsular,
	kClassJediSentinel,
	kClassExpertDroid,
	kClassCombatDroid,
	kClassMinion,

	// KotOR 2 classes
	kClassTechSpecialist,
	kClassBountyHunter,
	kClassJediWeaponMaster,
	kClassJediMaster,
	kClassJediWatchMan,
	kClassSithMarauder,
	kClassSithLord,
	kClassSithAssassin,

	kClassInvalid = 255
};

enum Race {
	kRaceUnknown = 0,
	kRaceElf,
	kRaceGnome,
	kRaceHalfling,
	kRaceHalfElf,
	kRaceDroid,
	kRaceHuman,
	kRaceAll,
	kRaceInvalid
};

enum SubRace {
	kSubRaceNone = 0,
	kSubRaceWookie
};

enum Faction {
	kFactionInvalid = -1,

	kFactionHostile1 = 1,
	kFactionFriendly1,
	kFactionHostile2,
	kFactionFriendly2,
	kFactionNeutral,
	kFactionInsane,
	kFactionPTatTuskan,
	kFactionGlbXor,
	kFactionSurrender1,
	kFactionSurrender2,
	kFactionPredator,
	kFactionPrey,
	kFactionEndarSpire,
	kFactionRancor,
	kFactionGizka1,
	kFactionGizka2,

	// Kotor 2 related factions.
	kFactionSelfLoathing = 21,
	kFactionOneOnOne,
	kFactionPartyPuppet
};

enum Skill {
	kSkillComputerUse = 0,
	kSkillDemolitions,
	kSkillStealth,
	kSkillAwareness,
	kSkillPersuade,
	kSkillRepair,
	kSkillSecurity,
	kSkillTreatInjury,

	kSkillMAX
};

enum Ability {
	kAbilityStrength = 0,
	kAbilityDexterity,
	kAbilityConstitution,
	kAbilityIntelligence,
	kAbilityWisdom,
	kAbilityCharisma
};

enum InventorySlot {
	kInventorySlotHead         = 0,
	kInventorySlotBody         = 1,
	kInventorySlotHands        = 3,
	kInventorySlotRightWeapon  = 4,
	kInventorySlotLeftWeapon   = 5,
	kInventorySlotLeftArm      = 7,
	kInventorySlotRightArm     = 8,
	kInventorySlotImplant      = 9,
	kInventorySlotBelt         = 10,
	kInventorySlotCWeaponL     = 14,
	kInventorySlotCWeaponR     = 15,
	kInventorySlotCWeaponB     = 16,
	kInventorySlotCArmour      = 17,
	kInventorySlotRightWeapon2 = 18,
	kInventorySlotLeftWeapon2  = 19,

	kInventorySlotMAX          = 20,
	kInventorySlotInvalid      = 255
};

enum ActionType {
	kActionMoveToPoint   = 0,
	kActionPickUpItem    = 1,
	kActionDropItem      = 2,
	kActionAttackObject  = 3,
	kActionCastSpell     = 4,
	kActionOpenDoor      = 5,
	kActionCloseDoor     = 6,
	kActionDialogObject  = 7,
	kActionDisableTrap   = 8,
	kActionRecoverTrap   = 9,
	kActionFlagTrap      = 10,
	kActionExamineTrap   = 11,
	kActionSetTrap       = 12,
	kActionOpenLock      = 13,
	kActionLock          = 14,
	kActionUseObject     = 15,
	kActionAnimalEmpathy = 16,
	kActionRest          = 17,
	kActionTaunt         = 18,
	kActionItemCastSpell = 19,
	kActionCounterSpell  = 31,
	kActionHeal          = 33,
	kActionPickpocket    = 34,
	kActionFollow        = 35,
	kActionWait          = 36,
	kActionSit           = 37,
	kActionFollowLeader  = 38,
	kActionQueueEmpty    = 65534,
	kActionInvalid       = 65535
};

enum CreatureType {
	kCreatureTypeRacialType             = 0,
	kCreatureTypePlayerChar             = 1,
	kCreatureTypeClass                  = 2,
	kCreatureTypeReputation             = 3,
	kCreatureTypeIsAlive                = 4,
	kCreatureTypeHasSpellEffect         = 5,
	kCreatureTypeDoesNotHaveSpellEffect = 6,
	kCreatureTypePerception             = 7,

	kCreatureTypeInvalid                = 255
};

enum ReputationType {
	kReputationTypeFriend  = 0,
	kReputationTypeEnemy   = 1,
	kReputationTypeNeutral = 2
};

enum Perception {
	kPerceptionSeenAndHeard       = 0,
	kPerceptionNotSeenAndNotHeard = 1,
	kPerceptionHeardAndNotSeen    = 2,
	kPerceptionSeenAndNotHeard    = 3,
	kPerceptionNotHeard           = 4,
	kPerceptionHeard              = 5,
	kPerceptionNotSeen            = 6,
	kPerceptionSeen               = 7
};

enum WeaponWield {
	kWeaponWieldBaton  = 1,
	kWeaponWieldSword  = 2,
	kWeaponWieldStaff  = 3,
	kWeaponWieldPistol = 4,
	kWeaponWieldRifle  = 5,
	kWeaponWieldHeavy  = 6
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_TYPES_H
