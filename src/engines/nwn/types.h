/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/types.h
 *  Basic NWN type definitions.
 */

#ifndef ENGINES_NWN_TYPES_H
#define ENGINES_NWN_TYPES_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"

namespace Engines {

namespace NWN {

class Creature;

struct CharacterKey {
	Common::UString name;
	uint number;

	CharacterKey(const Common::UString &na = "", uint nu = 0);

	bool operator<(const CharacterKey &key) const;
};

class CharacterID {
public:
	CharacterID();
	~CharacterID();

	bool empty() const;

	uint getNumber() const;

	const Creature &operator*() const;
	const Creature *operator->() const;

	void clear();

private:
	typedef std::map<CharacterKey, Creature *> CharMap;

	bool _empty;
	CharMap::iterator _char;

	CharacterID(CharMap::iterator c);

	void set(CharMap::iterator c);

	friend class CharacterStore;
};


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

enum Script {
	kScriptAcquireItem       = 0,
	kScriptUnacquireItem        ,
	kScriptActivateItem         ,
	kScriptEnter                ,
	kScriptExit                 ,
	kScriptCutsceneAbort        ,
	kScriptHeartbeat            ,
	kScriptModuleLoad           ,
	kScriptModuleStart          ,
	kScriptPlayerChat           ,
	kScriptPlayerDeath          ,
	kScriptPlayerDying          ,
	kScriptPlayerEquipItem      ,
	kScriptPlayerUnequipItem    ,
	kScriptPlayerLevelUp        ,
	kScriptPlayerRest           ,
	kScriptPlayerRespan         ,
	kScriptUserdefined          ,
	kScriptClick                ,
	kScriptOpen                 ,
	kScriptClosed               ,
	kScriptDamaged              ,
	kScriptDeath                ,
	kScriptDisarm               ,
	kScriptLock                 ,
	kScriptUnlock               ,
	kScriptAttacked             ,
	kScriptSpellCastAt          ,
	kScriptTrapTriggered        ,
	kScriptDialogue             ,
	kScriptDisturbed            ,
	kScriptEndRound             ,
	kScriptBlocked              ,
	kScriptNotice               ,
	kScriptRested               ,
	kScriptSpawn                ,
	kScriptMAX
};

enum SFFSounds {
	kSSFAttack            =  0,
	kSSFBattlecry1        =  1,
	kSSFBattlecry2        =  2,
	kSSFBattlecry3        =  3,
	kSSFHealMe            =  4,
	kSSFHelp              =  5,
	kSSFEnemiesSighted    =  6,
	kSSFFlee              =  7,
	kSSFTaunt             =  8,
	kSSFGuardMe           =  9,
	kSSFHold              = 10,
	kSSFAttackGrunt1      = 11,
	kSSFAttackGrunt2      = 12,
	kSSFAttackGrunt3      = 13,
	kSSFPainGrunt1        = 14,
	kSSFPainGrunt2        = 15,
	kSSFPainGrunt3        = 16,
	kSSFNearDeath         = 17,
	kSSFDeath             = 18,
	kSSFPoisoned          = 19,
	kSSFSpellFailed       = 20,
	kSSFWeaponIneffective = 21,
	kSSFFollowMe          = 22,
	kSSFLookHere          = 23,
	kSSFGroupParty        = 24,
	kSSFMoveOver          = 25,
	kSSFPickLock          = 26,
	kSSFSearch            = 27,
	kSSFGoStealthy        = 28,
	kSSFCanDo             = 29,
	kSSFCannotDo          = 30,
	kSSFTaskComplete      = 31,
	kSSFEncumbered        = 32,
	kSSFSelected          = 33,
	kSSFHello             = 34,
	kSSFYes               = 35,
	kSSFNo                = 36,
	kSSFStop              = 37,
	kSSFRest              = 38,
	kSSFBored             = 39,
	kSSFGoodbye           = 40,
	kSSFThankYou          = 41,
	kSSFLaugh             = 42,
	kSSFCuss              = 43,
	kSSFCheer             = 44,
	kSSFSomethingToSay    = 45,
	kSSFGoodIdea          = 46,
	kSSFBadIdea           = 47,
	kSSFThreaten          = 48
};

enum GameDifficulty {
	kDifficultyVeryEasy  = 0, // Not in the options dialog?!?
	kDifficultyEasy      = 1,
	kDifficultyNormal    = 2,
	kDifficultyDifficult = 3,
	kDifficultyCoreRules = 4  // "Very difficult"
};

enum AssociateType {
	kAssociateTypeNone            = 0,
	kAssociateTypeHenchman        = 1,
	kAssociateTypeAnimalCompanion = 2,
	kAssociateTypeFamiliar        = 3,
	kAssociateTypeSummoned        = 4,
	kAssociateTypeDominated       = 5
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

enum Alignment {
	kAlignmentAll     = 0,
	kAlignmentNeutral = 1,
	kAlignmentLawful  = 2,
	kAlignmentChaotic = 3,
	kAlignmentGood    = 4,
	kAlignmentEvil    = 5
};

static const uint32 kActionInvalid     = 0xFFFF;
static const uint32 kObjectTypeInvalid = 0x7FFF;

static const uint32 kGenderNone   =   4;
static const uint32 kRaceInvalid  =  28;
static const uint32 kClassInvalid = 255;

Alignment getAlignmentLawChaos(uint8 lawChaos);
Alignment getAlignmentGoodEvil(uint8 goodEvil);

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_TYPES_H
