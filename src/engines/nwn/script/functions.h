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
 *  Neverwinter Nights engine functions.
 */

#ifndef ENGINES_NWN_SCRIPT_FUNCTIONS_H
#define ENGINES_NWN_SCRIPT_FUNCTIONS_H

#include "src/aurora/nwscript/types.h"

namespace Aurora {
	namespace NWScript {
		class FunctionContext;
		class Object;
	}
}

namespace Engines {

namespace NWN {

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
		Aurora::NWScript::Type parameters[8];
	};

	struct FunctionDefaults {
		uint32_t id;
		const Aurora::NWScript::Variable *defaults[6];
	};

	static const FunctionPointer kFunctionPointers[];
	static const FunctionSignature kFunctionSignatures[];
	static const FunctionDefaults kFunctionDefaults[];


	Game *_game;

	void registerFunctions();

	// .--- Utility methods
	void jumpTo(NWN::Object *object, Area *area, float x, float y, float z);

	static int32_t getRandom(int min, int max, int32_t n = 1);

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

	void setCustomToken(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Module functions, functions_module.cpp
	void getModule(Aurora::NWScript::FunctionContext &ctx);

	void location(Aurora::NWScript::FunctionContext &ctx);

	void getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx);

	void startNewModule(Aurora::NWScript::FunctionContext &ctx);
	void endGame       (Aurora::NWScript::FunctionContext &ctx);

	void getFirstPC(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);
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

	void playAnimation(Aurora::NWScript::FunctionContext &ctx);

	void jumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void jumpToObject  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Situated objects, functions_situated.cpp
	void getLocked(Aurora::NWScript::FunctionContext &ctx);
	void setLocked(Aurora::NWScript::FunctionContext &ctx);

	void getIsOpen(Aurora::NWScript::FunctionContext &ctx);

	void getLastOpenedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastClosedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastUsedBy  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Creatures, functions_creature.cpp
	void getAbilityScore(Aurora::NWScript::FunctionContext &ctx);
	void getSkillRank   (Aurora::NWScript::FunctionContext &ctx);
	void getHasFeat     (Aurora::NWScript::FunctionContext &ctx);

	void getClassByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByClass   (Aurora::NWScript::FunctionContext &ctx);

	void getXP(Aurora::NWScript::FunctionContext &ctx);

	void getIsDead(Aurora::NWScript::FunctionContext &ctx);

	void getIsPC(Aurora::NWScript::FunctionContext &ctx);
	void getIsDM(Aurora::NWScript::FunctionContext &ctx);

	void getGender    (Aurora::NWScript::FunctionContext &ctx);
	void getRacialType(Aurora::NWScript::FunctionContext &ctx);

	void getHitDice(Aurora::NWScript::FunctionContext &ctx);

	void getLawChaosValue    (Aurora::NWScript::FunctionContext &ctx);
	void getGoodEvilValue    (Aurora::NWScript::FunctionContext &ctx);
	void getAlignmentLawChaos(Aurora::NWScript::FunctionContext &ctx);
	void getAlignmentGoodEvil(Aurora::NWScript::FunctionContext &ctx);

	void getCommandable(Aurora::NWScript::FunctionContext &ctx);
	void setCommandable(Aurora::NWScript::FunctionContext &ctx);

	void getMaster   (Aurora::NWScript::FunctionContext &ctx);
	void getAssociate(Aurora::NWScript::FunctionContext &ctx);
	void getHenchman (Aurora::NWScript::FunctionContext &ctx);

	void addHenchman(Aurora::NWScript::FunctionContext &ctx);
	void removeHenchman(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Actions, functions_action.cpp
	void assignCommand(Aurora::NWScript::FunctionContext &ctx);
	void delayCommand (Aurora::NWScript::FunctionContext &ctx);

	void executeScript(Aurora::NWScript::FunctionContext &ctx);

	void actionDoCommand(Aurora::NWScript::FunctionContext &ctx);

	void actionOpenDoor (Aurora::NWScript::FunctionContext &ctx);
	void actionCloseDoor(Aurora::NWScript::FunctionContext &ctx);

	void actionSpeakString      (Aurora::NWScript::FunctionContext &ctx);
	void actionStartConversation(Aurora::NWScript::FunctionContext &ctx);

	void actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx);

	void actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionJumpToObject  (Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToObject  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Conversations, functions_conversation.cpp
	void speakString        (Aurora::NWScript::FunctionContext &ctx);
	void speakStringByStrRef(Aurora::NWScript::FunctionContext &ctx);

	void speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx);

	void beginConversation(Aurora::NWScript::FunctionContext &ctx);

	void getPCSpeaker  (Aurora::NWScript::FunctionContext &ctx);
	void getLastSpeaker(Aurora::NWScript::FunctionContext &ctx);

	void isInConversation(Aurora::NWScript::FunctionContext &ctx);

	void getListenPatternNumber(Aurora::NWScript::FunctionContext &ctx);
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

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SCRIPT_FUNCTIONS_H
