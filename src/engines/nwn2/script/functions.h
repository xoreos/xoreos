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
 *  Neverwinter Nights 2 engine functions.
 */

#ifndef ENGINES_NWN2_SCRIPT_FUNCTIONS_H
#define ENGINES_NWN2_SCRIPT_FUNCTIONS_H

#include "src/aurora/nwscript/types.h"

namespace Aurora {
	namespace NWScript {
		class FunctionContext;
		class Object;
	}
}

namespace Engines {

namespace NWN2 {

class Game;
class Area;
class Object;

class Functions {
public:
	Functions(Game &game);
	~Functions();

private:
	typedef void (Functions::*funcPtr)(Aurora::NWScript::FunctionContext &ctx);

	struct FunctionPointer {
		uint32_t id;
		const char *name;
		funcPtr func;
	};

	struct FunctionSignature {
		uint32_t id;
		Aurora::NWScript::Type returnType;
		Aurora::NWScript::Type parameters[13];
	};

	struct FunctionDefaults {
		uint32_t id;
		const Aurora::NWScript::Variable *defaults[10];
	};

	static const FunctionPointer kFunctionPointers[];
	static const FunctionSignature kFunctionSignatures[];
	static const FunctionDefaults kFunctionDefaults[];


	Game *_game;

	void registerFunctions();

	// .--- Utility methods
	void jumpTo(NWN2::Object *object, Area *area, float x, float y, float z);

	void getGameDifficulty(Aurora::NWScript::FunctionContext &ctx);

	static int32_t getRandom(int min, int max, int32_t n = 1);

	void randomName(Aurora::NWScript::FunctionContext &ctx);

	static Common::UString formatFloat(float f, int width = 18, int decimals = 9);

	static Aurora::NWScript::Object *getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n);
	// '---

	// --- Engine functions ---

	void unimplementedFunction(Aurora::NWScript::FunctionContext &ctx);

	// .--- Math, functions_math.cpp
	void abs (Aurora::NWScript::FunctionContext &ctx);
	void fabs(Aurora::NWScript::FunctionContext &ctx);

	void cos(Aurora::NWScript::FunctionContext &ctx);
	void sin(Aurora::NWScript::FunctionContext &ctx);
	void tan(Aurora::NWScript::FunctionContext &ctx);

	void acos(Aurora::NWScript::FunctionContext &ctx);
	void asin(Aurora::NWScript::FunctionContext &ctx);
	void atan(Aurora::NWScript::FunctionContext &ctx);

	void log (Aurora::NWScript::FunctionContext &ctx);
	void pow (Aurora::NWScript::FunctionContext &ctx);
	void sqrt(Aurora::NWScript::FunctionContext &ctx);

	void random(Aurora::NWScript::FunctionContext &ctx);

	void feetToMeters(Aurora::NWScript::FunctionContext &ctx);
	void yardsToMeters(Aurora::NWScript::FunctionContext &ctx);

	void d2  (Aurora::NWScript::FunctionContext &ctx);
	void d3  (Aurora::NWScript::FunctionContext &ctx);
	void d4  (Aurora::NWScript::FunctionContext &ctx);
	void d6  (Aurora::NWScript::FunctionContext &ctx);
	void d8  (Aurora::NWScript::FunctionContext &ctx);
	void d10 (Aurora::NWScript::FunctionContext &ctx);
	void d12 (Aurora::NWScript::FunctionContext &ctx);
	void d20 (Aurora::NWScript::FunctionContext &ctx);
	void d100(Aurora::NWScript::FunctionContext &ctx);

	void intToFloat(Aurora::NWScript::FunctionContext &ctx);
	void floatToInt(Aurora::NWScript::FunctionContext &ctx);

	void vector         (Aurora::NWScript::FunctionContext &ctx);
	void vectorMagnitude(Aurora::NWScript::FunctionContext &ctx);
	void vectorNormalize(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Strings, functions_string.cpp
	void writeTimestampedLogEntry(Aurora::NWScript::FunctionContext &ctx);

	void sendMessageToPC(Aurora::NWScript::FunctionContext &ctx);

	void printInteger(Aurora::NWScript::FunctionContext &ctx);
	void printFloat  (Aurora::NWScript::FunctionContext &ctx);
	void printString (Aurora::NWScript::FunctionContext &ctx);
	void printObject (Aurora::NWScript::FunctionContext &ctx);
	void printVector (Aurora::NWScript::FunctionContext &ctx);

	void intToString   (Aurora::NWScript::FunctionContext &ctx);
	void floatToString (Aurora::NWScript::FunctionContext &ctx);
	void objectToString(Aurora::NWScript::FunctionContext &ctx);

	void intToHexString(Aurora::NWScript::FunctionContext &ctx);

	void stringToInt  (Aurora::NWScript::FunctionContext &ctx);
	void stringToFloat(Aurora::NWScript::FunctionContext &ctx);

	void getStringLength(Aurora::NWScript::FunctionContext &ctx);

	void getStringUpperCase(Aurora::NWScript::FunctionContext &ctx);
	void getStringLowerCase(Aurora::NWScript::FunctionContext &ctx);

	void getStringRight(Aurora::NWScript::FunctionContext &ctx);
	void getStringLeft (Aurora::NWScript::FunctionContext &ctx);

	void insertString (Aurora::NWScript::FunctionContext &ctx);
	void getSubString (Aurora::NWScript::FunctionContext &ctx);
	void findSubString(Aurora::NWScript::FunctionContext &ctx);

	void getStringByStrRef(Aurora::NWScript::FunctionContext &ctx);

	void get2DAString(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Campaign functions, functions_campaign.cpp
	void getOnePartyMode(Aurora::NWScript::FunctionContext &ctx);
	void getIsSinglePlayer(Aurora::NWScript::FunctionContext &ctx);

	void getGlobalInt   (Aurora::NWScript::FunctionContext &ctx);
	void getGlobalBool  (Aurora::NWScript::FunctionContext &ctx);
	void getGlobalString(Aurora::NWScript::FunctionContext &ctx);
	void getGlobalFloat (Aurora::NWScript::FunctionContext &ctx);

	void setGlobalInt   (Aurora::NWScript::FunctionContext &ctx);
	void setGlobalBool  (Aurora::NWScript::FunctionContext &ctx);
	void setGlobalString(Aurora::NWScript::FunctionContext &ctx);
	void setGlobalFloat (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Module functions, functions_module.cpp
	void getModule(Aurora::NWScript::FunctionContext &ctx);

	void location(Aurora::NWScript::FunctionContext &ctx);

	void getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx);

	void startNewModule(Aurora::NWScript::FunctionContext &ctx);

	void getFirstPC(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);

	void getJournalQuestExperience(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- General object functions, functions_object.cpp
	void getClickingObject(Aurora::NWScript::FunctionContext &ctx);
	void getEnteringObject(Aurora::NWScript::FunctionContext &ctx);
	void getExitingObject (Aurora::NWScript::FunctionContext &ctx);

	void getIsObjectValid(Aurora::NWScript::FunctionContext &ctx);

	void getLocalInt   (Aurora::NWScript::FunctionContext &ctx);
	void getLocalFloat (Aurora::NWScript::FunctionContext &ctx);
	void getLocalString(Aurora::NWScript::FunctionContext &ctx);
	void getLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void setLocalInt   (Aurora::NWScript::FunctionContext &ctx);
	void setLocalFloat (Aurora::NWScript::FunctionContext &ctx);
	void setLocalString(Aurora::NWScript::FunctionContext &ctx);
	void setLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void getObjectType(Aurora::NWScript::FunctionContext &ctx);

	void getTag (Aurora::NWScript::FunctionContext &ctx);
	void getName(Aurora::NWScript::FunctionContext &ctx);

	void getArea    (Aurora::NWScript::FunctionContext &ctx);
	void getLocation(Aurora::NWScript::FunctionContext &ctx);

	void getPosition(Aurora::NWScript::FunctionContext &ctx);

	void getDistanceToObject(Aurora::NWScript::FunctionContext &ctx);

	void getObjectByTag       (Aurora::NWScript::FunctionContext &ctx);
	void getWaypointByTag     (Aurora::NWScript::FunctionContext &ctx);
	void getNearestObject     (Aurora::NWScript::FunctionContext &ctx);
	void getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx);
	void getNearestCreature   (Aurora::NWScript::FunctionContext &ctx);

	void getCurrentHitPoints(Aurora::NWScript::FunctionContext &ctx);
	void getMaxHitPoints    (Aurora::NWScript::FunctionContext &ctx);

	void getIsTrapped      (Aurora::NWScript::FunctionContext &ctx);
	void getTrapActive     (Aurora::NWScript::FunctionContext &ctx);
	void getTrapBaseType   (Aurora::NWScript::FunctionContext &ctx);
	void getTrapCreator    (Aurora::NWScript::FunctionContext &ctx);
	void getTrapDetectable (Aurora::NWScript::FunctionContext &ctx);
	void getTrapDetectDC   (Aurora::NWScript::FunctionContext &ctx);
	void getTrapDetectedBy (Aurora::NWScript::FunctionContext &ctx);
	void getTrapDisarmable (Aurora::NWScript::FunctionContext &ctx);
	void getTrapDisarmDC   (Aurora::NWScript::FunctionContext &ctx);
	void getTrapFlagged    (Aurora::NWScript::FunctionContext &ctx);
	void getTrapKeyTag     (Aurora::NWScript::FunctionContext &ctx);
	void getTrapOneShot    (Aurora::NWScript::FunctionContext &ctx);
	void getTrapRecoverable(Aurora::NWScript::FunctionContext &ctx);

	void setTrapActive     (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDetectable (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDetectDC   (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDetectedBy (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDisabled   (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDisarmable (Aurora::NWScript::FunctionContext &ctx);
	void setTrapDisarmDC   (Aurora::NWScript::FunctionContext &ctx);
	void setTrapKeyTag     (Aurora::NWScript::FunctionContext &ctx);
	void setTrapOneShot    (Aurora::NWScript::FunctionContext &ctx);
	void setTrapRecoverable(Aurora::NWScript::FunctionContext &ctx);

	void createTrapOnObject(Aurora::NWScript::FunctionContext &ctx);

	void jumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void jumpToObject  (Aurora::NWScript::FunctionContext &ctx);

	void getUseableFlag(Aurora::NWScript::FunctionContext &ctx);
	void setUseableFlag(Aurora::NWScript::FunctionContext &ctx);

	void setAssociateListenPatterns(Aurora::NWScript::FunctionContext &ctx);
	void getIsListening            (Aurora::NWScript::FunctionContext &ctx);
	void setListening              (Aurora::NWScript::FunctionContext &ctx);
	void setListenPattern          (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Situated objects, functions_situated.cpp
	void getLocked         (Aurora::NWScript::FunctionContext &ctx);
	void getLockLockable   (Aurora::NWScript::FunctionContext &ctx);
	void getLockKeyRequired(Aurora::NWScript::FunctionContext &ctx);
	void getLockKeyTag     (Aurora::NWScript::FunctionContext &ctx);
	void getLockLockDC     (Aurora::NWScript::FunctionContext &ctx);
	void getLockUnlockDC   (Aurora::NWScript::FunctionContext &ctx);

	void setLocked         (Aurora::NWScript::FunctionContext &ctx);
	void setLockLockable   (Aurora::NWScript::FunctionContext &ctx);
	void setLockKeyRequired(Aurora::NWScript::FunctionContext &ctx);
	void setLockKeyTag     (Aurora::NWScript::FunctionContext &ctx);
	void setLockLockDC     (Aurora::NWScript::FunctionContext &ctx);
	void setLockUnlockDC   (Aurora::NWScript::FunctionContext &ctx);

	void getKeyRequiredFeedbackMessage(Aurora::NWScript::FunctionContext &ctx);
	void setKeyRequiredFeedbackMessage(Aurora::NWScript::FunctionContext &ctx);

	void getIsOpen(Aurora::NWScript::FunctionContext &ctx);

	void getLastOpenedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastClosedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastUsedBy  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Items and inventory, functions_inventory.cpp
	void getHasInventory      (Aurora::NWScript::FunctionContext &ctx);

	void getDroppableFlag     (Aurora::NWScript::FunctionContext &ctx);
	void getIdentified        (Aurora::NWScript::FunctionContext &ctx);
	void getItemCursedFlag    (Aurora::NWScript::FunctionContext &ctx);
	void getPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx);
	void getStolenFlag        (Aurora::NWScript::FunctionContext &ctx);
	void getInfiniteFlag      (Aurora::NWScript::FunctionContext &ctx);

	void setDroppableFlag     (Aurora::NWScript::FunctionContext &ctx);
	void setIdentified        (Aurora::NWScript::FunctionContext &ctx);
	void setItemCursedFlag    (Aurora::NWScript::FunctionContext &ctx);
	void setPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx);
	void setStolenFlag        (Aurora::NWScript::FunctionContext &ctx);
	void setInfiniteFlag      (Aurora::NWScript::FunctionContext &ctx);

	void getBaseItemType   (Aurora::NWScript::FunctionContext &ctx);
	void getItemIcon       (Aurora::NWScript::FunctionContext &ctx);
	void getItemStackSize  (Aurora::NWScript::FunctionContext &ctx);
	void getNumStackedItems(Aurora::NWScript::FunctionContext &ctx);
	void getItemCharges    (Aurora::NWScript::FunctionContext &ctx);

	void setItemIcon     (Aurora::NWScript::FunctionContext &ctx);
	void setItemStackSize(Aurora::NWScript::FunctionContext &ctx);
	void setItemCharges  (Aurora::NWScript::FunctionContext &ctx);

	void getStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx);
	void getStoreGold        (Aurora::NWScript::FunctionContext &ctx);
	void getStoreMaxBuyPrice (Aurora::NWScript::FunctionContext &ctx);

	void setStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx);
	void setStoreGold        (Aurora::NWScript::FunctionContext &ctx);
	void setStoreMaxBuyPrice (Aurora::NWScript::FunctionContext &ctx);

	void getFirstItemInInventory(Aurora::NWScript::FunctionContext &ctx);
	void getNextItemInInventory (Aurora::NWScript::FunctionContext &ctx);
	void getItemInSlot          (Aurora::NWScript::FunctionContext &ctx);

	void getFirstItemProperty  (Aurora::NWScript::FunctionContext &ctx);
	void getNextItemProperty   (Aurora::NWScript::FunctionContext &ctx);
	void getItemHasItemProperty(Aurora::NWScript::FunctionContext &ctx);

	void createItemOnObject(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Item properties, functions_itemproperty.cpp
	void getItemPropertyType          (Aurora::NWScript::FunctionContext &ctx);
	void getItemPropertySubType       (Aurora::NWScript::FunctionContext &ctx);
	void getItemPropertyParam1        (Aurora::NWScript::FunctionContext &ctx);
	void getItemPropertyParam1Value   (Aurora::NWScript::FunctionContext &ctx);
	void getItemPropertyCostTable     (Aurora::NWScript::FunctionContext &ctx);
	void getItemPropertyCostTableValue(Aurora::NWScript::FunctionContext &ctx);
	void getIsItemPropertyValid       (Aurora::NWScript::FunctionContext &ctx);

	void itemPropertyAbilityBonus            (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyACBonus                 (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyACBonusVsAlign          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyACBonusVsDmgType        (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyACBonusVsRace           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyACBonusVsSAlign         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyEnhancementBonus        (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyEnhancementBonusVsAlign (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyEnhancementBonusVsRace  (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyEnhancementBonusVsSAlign(Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyEnhancementPenalty      (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyWeightReduction         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusFeat               (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusLevelSpell         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyCastSpell               (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageBonus             (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageBonusVsAlign      (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageBonusVsRace       (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageBonusVsSAlign     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageImmunity          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamagePenalty           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageResistance        (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageVulnerability     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDarkvision              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDecreaseAbility         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDecreaseAC              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDecreaseSkill           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyContainerReducedWeight  (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyExtraMeleeDamageType    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyExtraRangeDamageType    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyHaste                   (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyHolyAvenger             (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyImprovedEvasion         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusSpellResistance    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyImmunityMisc            (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusSavingThrow        (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusSavingThrowVsX     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyKeen                    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyLight                   (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyMaxRangeStrengthMod     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyNoDamage                (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyOnHitProps              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyReducedSavingThrow      (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyReducedSavingThrowVsX   (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyRegeneration            (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertySkillBonus              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertySpellImmunitySpecific   (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertySpellImmunitySchool     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyThievesTools            (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyAttackBonus             (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyAttackBonusVsAlign      (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyAttackBonusVsRace       (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyAttackBonusVsSAlign     (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyAttackPenalty           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyUnlimitedAmmo           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyLimitUseByAlign         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyLimitUseByClass         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyLimitUseByRace          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyLimitUseBySAlign        (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyTrap                    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyTrueSeeing              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyOnMonsterHitProperties  (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyTurnResistance          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyFreeAction              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyMonsterDamage           (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyImmunityToSpellLevel    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertySpecialWalk             (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyHealersKit              (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyWeightIncrease          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyBonusHitpoints          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyVampiricRegeneration    (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyMassiveCritical         (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyOnHitCastSpell          (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyVisualEffect            (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyArcaneSpellFailure      (Aurora::NWScript::FunctionContext &ctx);
	void itemPropertyDamageReduction         (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Creatures, functions_creature.cpp
	void getAbilityScore(Aurora::NWScript::FunctionContext &ctx);
	void getSkillRank   (Aurora::NWScript::FunctionContext &ctx);
	void getHasFeat     (Aurora::NWScript::FunctionContext &ctx);

	void featAdd(Aurora::NWScript::FunctionContext &ctx);

	void getClassByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByClass   (Aurora::NWScript::FunctionContext &ctx);

	void getXP(Aurora::NWScript::FunctionContext &ctx);

	void getIsDead(Aurora::NWScript::FunctionContext &ctx);
	void getImmortal(Aurora::NWScript::FunctionContext &ctx);
	void getPlotFlag(Aurora::NWScript::FunctionContext &ctx);
	void getLootable(Aurora::NWScript::FunctionContext &ctx);
	void getIsCreatureDisarmable(Aurora::NWScript::FunctionContext &ctx);

	void setImmortal(Aurora::NWScript::FunctionContext &ctx);
	void setPlotFlag(Aurora::NWScript::FunctionContext &ctx);
	void setLootable(Aurora::NWScript::FunctionContext &ctx);

	void addJournalQuestEntry   (Aurora::NWScript::FunctionContext &ctx);
	void removeJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx);
	void getJournalEntry        (Aurora::NWScript::FunctionContext &ctx);

	void getIsPC(Aurora::NWScript::FunctionContext &ctx);
	void getIsDM(Aurora::NWScript::FunctionContext &ctx);

	void getGender    (Aurora::NWScript::FunctionContext &ctx);
	void getRacialType(Aurora::NWScript::FunctionContext &ctx);

	void getHitDice(Aurora::NWScript::FunctionContext &ctx);

	void getLawChaosValue    (Aurora::NWScript::FunctionContext &ctx);
	void getGoodEvilValue    (Aurora::NWScript::FunctionContext &ctx);
	void getAlignmentLawChaos(Aurora::NWScript::FunctionContext &ctx);
	void getAlignmentGoodEvil(Aurora::NWScript::FunctionContext &ctx);

	void getIsRosterMember   (Aurora::NWScript::FunctionContext &ctx);
	void getFirstRosterMember(Aurora::NWScript::FunctionContext &ctx);
	void getNextRosterMember (Aurora::NWScript::FunctionContext &ctx);
	void getFactionLeader    (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Roster and party, functions_roster.cpp
	void getRosterNPCPartyLimit(Aurora::NWScript::FunctionContext &ctx);
	void setRosterNPCPartyLimit(Aurora::NWScript::FunctionContext &ctx);

	void getIsRosterMemberAvailable  (Aurora::NWScript::FunctionContext &ctx);
	void getIsRosterMemberCampaignNPC(Aurora::NWScript::FunctionContext &ctx);
	void getIsRosterMemberSelectable (Aurora::NWScript::FunctionContext &ctx);

	void setIsRosterMemberAvailable  (Aurora::NWScript::FunctionContext &ctx);
	void setIsRosterMemberCampaignNPC(Aurora::NWScript::FunctionContext &ctx);
	void setIsRosterMemberSelectable (Aurora::NWScript::FunctionContext &ctx);

	void addRosterMemberByTemplate(Aurora::NWScript::FunctionContext &ctx);

	void getPartyName (Aurora::NWScript::FunctionContext &ctx);
	void getPartyMotto(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Actions, functions_action.cpp
	void assignCommand(Aurora::NWScript::FunctionContext &ctx);
	void delayCommand (Aurora::NWScript::FunctionContext &ctx);

	void executeScript(Aurora::NWScript::FunctionContext &ctx);

	void actionDoCommand(Aurora::NWScript::FunctionContext &ctx);

	void actionOpenDoor (Aurora::NWScript::FunctionContext &ctx);
	void actionCloseDoor(Aurora::NWScript::FunctionContext &ctx);

	void actionSpeakString(Aurora::NWScript::FunctionContext &ctx);

	void actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionJumpToObject  (Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToObject  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Conversations, functions_conversation.cpp
	void speakString        (Aurora::NWScript::FunctionContext &ctx);
	void speakStringByStrRef(Aurora::NWScript::FunctionContext &ctx);

	void speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Sound, functions_sound.cpp
	void playVoiceChat    (Aurora::NWScript::FunctionContext &ctx);
	void playSoundByStrRef(Aurora::NWScript::FunctionContext &ctx);
	void playSound        (Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx);
	// '---
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_SCRIPT_FUNCTIONS_H
