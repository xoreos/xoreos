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
 *  Engine functions for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_SCRIPT_FUNCTIONS_H
#define ENGINES_KOTORBASE_SCRIPT_FUNCTIONS_H

#include "src/aurora/nwscript/types.h"

namespace Aurora {
	namespace NWScript {
		class FunctionContext;
		class Object;
	}
}

namespace Engines {

namespace KotORBase {

class Game;
class Area;
class Object;
class Event;

class Functions {
public:
	Functions(Game &game);
	virtual ~Functions();

protected:
	typedef void (Functions::*funcPtr)(Aurora::NWScript::FunctionContext &ctx);

	struct FunctionPointer {
		uint32_t id;
		const char *name;
		funcPtr func;
	};

	struct FunctionSignature {
		uint32_t id;
		Aurora::NWScript::Type returnType;
		Aurora::NWScript::Type parameters[15];
	};

	struct FunctionDefaults {
		uint32_t id;
		const Aurora::NWScript::Variable *defaults[14];
	};

	virtual void registerFunctions() = 0;

	// Utility methods

	void jumpTo(Object *object, float x, float y, float z);

	static int32_t getRandom(int min, int max, int32_t n = 1);

	static Common::UString formatFloat(float f, int width = 18, int decimals = 9);

	static Aurora::NWScript::Object *getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n);

	// Engine functions

	void getRunScriptVar(Aurora::NWScript::FunctionContext &ctx);

	void unimplementedFunction(Aurora::NWScript::FunctionContext &ctx);
	void executeScript(Aurora::NWScript::FunctionContext &ctx);

	// Math, functions_math.cpp

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

	// Strings, functions_string.cpp

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

	// Module functions, functions_module.cpp

	void getModule(Aurora::NWScript::FunctionContext &ctx);
	void getFirstPC(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);
	void getPCSpeaker(Aurora::NWScript::FunctionContext &ctx);
	void getIsConversationActive(Aurora::NWScript::FunctionContext &ctx);

	void startNewModule(Aurora::NWScript::FunctionContext &ctx);

	void setGlobalFadeOut(Aurora::NWScript::FunctionContext &ctx);
	void setGlobalFadeIn(Aurora::NWScript::FunctionContext &ctx);
	void setReturnStrref(Aurora::NWScript::FunctionContext &ctx);

	// General object functions, functions_object.cpp

	void getClickingObject(Aurora::NWScript::FunctionContext &ctx);
	void getEnteringObject(Aurora::NWScript::FunctionContext &ctx);
	void getExitingObject(Aurora::NWScript::FunctionContext &ctx);
	void getIsObjectValid(Aurora::NWScript::FunctionContext &ctx);
	void getIsPC(Aurora::NWScript::FunctionContext &ctx);
	void getObjectByTag(Aurora::NWScript::FunctionContext &ctx);
	void getMinOneHP(Aurora::NWScript::FunctionContext &ctx);
	void getCurrentHitPoints(Aurora::NWScript::FunctionContext &ctx);
	void getMaxHitPoints(Aurora::NWScript::FunctionContext &ctx);
	void getItemInSlot(Aurora::NWScript::FunctionContext &ctx);
	void getNearestCreature(Aurora::NWScript::FunctionContext &ctx);
	void getTag(Aurora::NWScript::FunctionContext &ctx);

	void getArea    (Aurora::NWScript::FunctionContext &ctx);
	void getLocation(Aurora::NWScript::FunctionContext &ctx);

	void jumpToLocation(Aurora::NWScript::FunctionContext &ctx);

	void setMinOneHP(Aurora::NWScript::FunctionContext &ctx);
	void setMaxHitPoints(Aurora::NWScript::FunctionContext &ctx);

	void getStandardFaction(Aurora::NWScript::FunctionContext &ctx);
	void changeToStandardFaction(Aurora::NWScript::FunctionContext &ctx);

	void createItemOnObject(Aurora::NWScript::FunctionContext &ctx);
	void destroyObject(Aurora::NWScript::FunctionContext &ctx);

	// Situated objects, functions_situated.cpp

	void getLocked(Aurora::NWScript::FunctionContext &ctx);
	void setLocked(Aurora::NWScript::FunctionContext &ctx);

	void getIsOpen(Aurora::NWScript::FunctionContext &ctx);

	void getLastOpenedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastClosedBy(Aurora::NWScript::FunctionContext &ctx);
	void getLastUsedBy  (Aurora::NWScript::FunctionContext &ctx);

	// Actions, functions_action.cpp

	void getCurrentAction(Aurora::NWScript::FunctionContext &ctx);

	void assignCommand(Aurora::NWScript::FunctionContext &ctx);
	void delayCommand (Aurora::NWScript::FunctionContext &ctx);
	void actionStartConversation(Aurora::NWScript::FunctionContext &ctx);
	void actionOpenDoor(Aurora::NWScript::FunctionContext &ctx);
	void actionCloseDoor(Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToObject(Aurora::NWScript::FunctionContext &ctx);
	void actionFollowLeader(Aurora::NWScript::FunctionContext &ctx);
	void clearAllActions(Aurora::NWScript::FunctionContext &ctx);

	// Sound, functions_sound.cpp

	void musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx);

	void musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx);
	void musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx);

	void soundObjectPlay(Aurora::NWScript::FunctionContext &ctx);
	void soundObjectStop(Aurora::NWScript::FunctionContext &ctx);

	// Movies, functions_movie.cpp

	void playMovie(Aurora::NWScript::FunctionContext &ctx);

	// Creatures, functions_creatures.cpp

	void getGender(Aurora::NWScript::FunctionContext &ctx);
	void getLevelByClass(Aurora::NWScript::FunctionContext &ctx);

	void getLevelByPosition(Aurora::NWScript::FunctionContext &ctx);
	void getClassByPosition(Aurora::NWScript::FunctionContext &ctx);

	void getRacialType(Aurora::NWScript::FunctionContext &ctx);
	void getSubRace(Aurora::NWScript::FunctionContext &ctx);

	void getHasSkill(Aurora::NWScript::FunctionContext &ctx);
	void getSkillRank(Aurora::NWScript::FunctionContext &ctx);

	void getAbilityScore(Aurora::NWScript::FunctionContext &ctx);

	// Global values, functions_global.cpp

	void getGlobalBoolean(Aurora::NWScript::FunctionContext &ctx);
	void setGlobalBoolean(Aurora::NWScript::FunctionContext &ctx);
	void getGlobalNumber(Aurora::NWScript::FunctionContext &ctx);
	void setGlobalNumber(Aurora::NWScript::FunctionContext &ctx);

	// Local variables, functions_local.cpp

	void getLocalBoolean(Aurora::NWScript::FunctionContext &ctx);
	void setLocalBoolean(Aurora::NWScript::FunctionContext &ctx);
	void getLocalNumber(Aurora::NWScript::FunctionContext &ctx);
	void setLocalNumber(Aurora::NWScript::FunctionContext &ctx);

	// Party, functions_party.cpp

	void getPartyMemberByIndex(Aurora::NWScript::FunctionContext &ctx);
	void getSoloMode(Aurora::NWScript::FunctionContext &ctx);
	void getCommandable(Aurora::NWScript::FunctionContext &ctx);

	void isObjectPartyMember(Aurora::NWScript::FunctionContext &ctx);
	void isAvailableCreature(Aurora::NWScript::FunctionContext &ctx);

	void setPartyLeader(Aurora::NWScript::FunctionContext &ctx);
	void setCommandable(Aurora::NWScript::FunctionContext &ctx);

	void showPartySelectionGUI(Aurora::NWScript::FunctionContext &ctx);
	void addAvailableNPCByTemplate(Aurora::NWScript::FunctionContext &ctx);

	// Events, functions_events.cpp

	void signalEvent(Aurora::NWScript::FunctionContext &ctx);
	void eventUserDefined(Aurora::NWScript::FunctionContext &ctx);
	void getUserDefinedEventNumber(Aurora::NWScript::FunctionContext &ctx);

	// Time, functions_time.cpp

	void getTimeHour(Aurora::NWScript::FunctionContext &ctx);
	void getTimeMinute(Aurora::NWScript::FunctionContext &ctx);
	void getTimeSecond(Aurora::NWScript::FunctionContext &ctx);
	void getTimeMillisecond(Aurora::NWScript::FunctionContext &ctx);

	// Parameters, functions_parameters.cpp

	void getScriptParameter(Aurora::NWScript::FunctionContext &ctx);
	void getScriptStringParameter(Aurora::NWScript::FunctionContext &ctx);

private:
	Game *_game;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_SCRIPT_FUNCTIONS_H
