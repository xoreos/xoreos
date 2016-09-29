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
 *  Basic Dragon Age: Origins type definitions.
 */

#ifndef ENGINES_DRAGONAGE_TYPES_H
#define ENGINES_DRAGONAGE_TYPES_H

namespace Engines {

namespace DragonAge {

enum TextureQuality {
	kTextureQualityHigh   = 0,
	kTextureQualityMedium,

	kTextureQualityMAX
};

/** Multiple GDA as defined by m2da_*.gda. */
enum Worksheet {
	kWorksheetAnimations      =   0,
	kWorksheetAbilities       =   1,
	kWorksheetAppearances     =   2,
	kWorksheetClasses         =   3,
	kWorksheetProjectiles     =   4,
	kWorksheetRaces           =   5,
	kWorksheetItems           =   6,
	kWorksheetPortraits       =   7,
	kWorksheetVisualEffects   =   8,
	kWorksheetSounds          =  10,
	kWorksheetNakedVariations =  80,
	kWorksheetPlaceables      =  93,
	kWorksheetWaypoints       = 106
};

/** Object type, matches the bitfield in script.ldf */
enum ObjectType {
	kObjectTypeInvalid            = 0,

	kObjectTypeGUI                = 1U <<  0,
	kObjectTypeTile               = 1U <<  1,
	kObjectTypeModule             = 1U <<  2,
	kObjectTypeArea               = 1U <<  3,
	kObjectTypeStore              = 1U <<  4,
	kObjectTypeCreature           = 1U <<  5,
	kObjectTypeItem               = 1U <<  6,
	kObjectTypeTrigger            = 1U <<  7,
	kObjectTypeProjectile         = 1U <<  8,
	kObjectTypePlaceable          = 1U <<  9,
	kObjectTypeAreaOfEffectObject = 1U << 11,
	kObjectTypeWaypoint           = 1U << 12,
	kObjectTypeSound              = 1U << 14,
	kObjectTypeParty              = 1U << 15,
	kObjectTypeMapPatch           = 1U << 16,
	kObjectTypeVfx                = 1U << 17,
	kObjectTypeMap                = 1U << 18,

	kObjectTypeMAX,
	kObjectTypeAll                = 0xFFFFFFFF,

	kObjectTypeSelf               = 1U << 31  ///< Fake value to describe the calling object in a script.
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

	kInventorySlotInvalid        = 255
};

/** Types for script events. */
enum EventType {
	// Objects
	kEventTypeInvalid                       =    0,
	kEventTypeSpellCastAt                   =    1,
	kEventTypeDamaged                       =    2,
	kEventTypeSpawn                         =    3,
	kEventTypeDeath                         =    4,
	kEventTypeMeleeAttackStart              =    5,
	kEventTypeInventoryAdded                =    6,
	kEventTypeInventoryRemoved              =    7,
	kEventTypeEnter                         =    8,
	kEventTypeExit                          =    9,

	// Creatures
	kEventTypeBlocked                       =   10,
	kEventTypeEquip                         =   11,
	kEventTypeUnequip                       =   12,
	kEventTypeFailToOpen                    =   13,

	// Placeables
	kEventTypeUse                           =   14,
	kEventTypeClick                         =   15,
	kEventTypeTrapTriggered                 =   16,
	kEventTypeTrapDisarmed                  =   17,

	// Other
	kEventTypeDialogue                      =   18,
	kEventTypeModuleStart                   =   19,
	kEventTypeModuleLoad                    =   20,
	kEventTypeListener                      =   21,
	kEventTypeLocked                        =   22,
	kEventTypeUnlocked                      =   23,
	kEventTypePlayerLevelUp                 =   24,
	kEventTypeModuleGetCharStage            =   63,

	// Perception
	kEventTypePerceptionAppear              =   25,
	kEventTypePerceptionDisappear           =   26,

	// Plot
	kEventTypeSetPlot                       =   27,
	kEventTypeGetPlot                       =   28,

	// Attack
	kEventTypeAttackImpact                  =   29,
	kEventTypeCombatInitiated               =   30,

	// Ability
	kEventTypeAbilityCastImpact             =   31,
	kEventTypeAbilityCastStart              =   32,

	// Rules
	kEventTypeApplyEffect                   =   33,
	kEventTypeRemoveEffect                  =   34,

	// AI
	kEventTypeCommandPending                =   35,
	kEventTypeCommandComplete               =   36,

	// Area loading
	kEventTypeGameObjectsLoaded             =   37,
	kEventTypeAreaLoadPreLoadExit           =   38,
	kEventTypeAreaLoadPostLoadExit          =   39,
	kEventTypeAreaLoadSpecial               =   40,
	kEventTypeAreaLoadSaveSpecial           =   41,
	kEventTypeAreaLoadSavePreLoadExit       =   42,
	kEventTypeAreaLoadSavePostLoadExit      =   43,

	// Character creation
	kEventTypeChargenStart                  =   44,
	kEventTypeChargenScreenEntered          =   45,
	kEventTypeChargenSelectRace             =   46,
	kEventTypeChargenSelectClass            =   47,
	kEventTypeChargenSelectSoundset         =   48,
	kEventTypeChargenSelectName             =   49,
	kEventTypeChargenAssignAttributes       =   50,
	kEventTypeChargenAssignAbilities        =   51,
	kEventTypeChargenSelectLevelUpClass     =   52,
	kEventTypeChargenImportHero             =   53,
	kEventTypeChargenSelectGender           =   54,
	kEventTypeChargenSelectBackground       =   55,
	kEventTypeChargenEnd                    =   59,

	kEventTypeGameModeChange                =   60,
	kEventTypeDeathResParty                 =   61,
	kEventTypeModulePreSave                 =   62,
	kEventTypeManaStamDepleted              =   64,
	kEventTypeItemOnHit                     =   65,
	kEventTypePartyMemberAdded              =   66,
	kEventTypePartyMemberDropped            =   67,
	kEventTypeUsePlotAction                 =   68,
	kEventTypeChantersDonation              =   69,
	kEventTypeItemOnTestUsable              =   70,
	kEventTypePartyPickerClosed             =   71,
	kEventTypeLevelOfTheWeek                =   72,
	kEventTypeAbilityAcquired               =   73,
	kEventTypeAoeHeartbeat                  =   74,
	kEventTypeWorldMapClosed                =   75,
	kEventTypePopupResult                   =   76,
	kEventTypePlaceableCollision            =   77,
	kEventTypePlaceableOnclick              =   78,
	kEventTypeReachedWaypoint               =   79,
	kEventTypeAreaListPostLoad              =   80,
	kEventTypeHeartbeat2                    =   81,
	kEventTypeGiftItem                      =   82,
	kEventTypeLoadTacticsPreset             =   83,
	kEventTypeGuiOpened                     =   84,
	kEventTypeInventoryFull                 =   85,
	kEventTypeCreatureEntersDialogue        =   86,
	kEventTypeRubberBand                    =   87,
	kEventTypeGiveUp                        =   88,
	kEventTypeOnSelect                      =   89,
	kEventTypeOnOrderReceived               =   90,
	kEventTypeBeginTravel                   =   91,
	kEventTypeWorldmapPreTransition         =   92,
	kEventTypeAbilityProjectileLaunched     =   93,
	kEventTypePlayerCommandAdded            =   94,
	kEventTypeCharRecordOpened              =   95,
	kEventTypeOptionsChanged                =   96,
	kEventTypeRoamDistExceeded              =   97,
	kEventTypePlotCompleted                 =   98,
	kEventTypePartyMoneyChanged             =   99,
	kEventTypeCodexChanged                  =  100,
	kEventTypeUseAbilityImmediate           =  101,
	kEventTypeCraftItem                     =  102,
	kEventTypeWorldmapPostTransition        =  103,
	kEventTypePartyPickerInit               =  104,
	kEventTypeTrainingBegin                 = 1059,
	kEventTypeTrainingFollowerSelected      = 1060,
	kEventTypeTrainingGuiTacticsOpened      = 1061,
	kEventTypeTrainingGuiLevlupOpened       = 1062,
	kEventTypeTrainingGuiInventoryOpened    = 1063,
	kEventTypeTrainingQbarItemEquipped      = 1064,
	kEventTypeTrainingMaximumZoomIn         = 1065,
	kEventTypeTrainingWeaponEquipped        = 1067,
	kEventTypeTrainingDelayedNoaction       = 1068,
	kEventTypeTrainingUnpause               = 1069,
	kEventTypeTrainingItemUnequipped        = 1070,
	kEventTypeTrainingItemEquipped          = 1071,
	kEventTypeTrainingTacticPresetSelected  = 1072,
	kEventTypeTrainingMovementCommandIssued = 1073
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_TYPES_H
