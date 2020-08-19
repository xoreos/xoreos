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
 *  Jade Empire engine functions.
 */

#ifndef ENGINES_JADE_SCRIPT_FUNCTIONS_H
#define ENGINES_JADE_SCRIPT_FUNCTIONS_H

#include "src/aurora/nwscript/types.h"

namespace Aurora {
	class TwoDAFile;
	namespace NWScript {
		class FunctionContext;
		class Object;
	}
}

namespace Engines {

namespace Jade {

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
		Aurora::NWScript::Type parameters[12];
	};

	struct FunctionDefaults {
		uint32_t id;
		const Aurora::NWScript::Variable *defaults[11];
	};

	static const FunctionPointer kFunctionPointers[];
	static const FunctionSignature kFunctionSignatures[];
	static const FunctionDefaults kFunctionDefaults[];


	Game *_game;

	void registerFunctions();

	// .--- Utility methods
	void jumpTo(Jade::Object *object, Area *area, float x, float y, float z);

	static int32_t getRandom(int min, int max, int32_t n = 1);

	static Common::UString formatFloat(float f, int width = 18, int decimals = 9);

	static Aurora::NWScript::Object *getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n);

	const Aurora::TwoDAFile &findTable(int32_t nr);
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

	void setCustomToken(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Game functions, functions_game.cpp
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
	void location(Aurora::NWScript::FunctionContext &ctx);

	void getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx);

	void getPlayer(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- General object functions, functions_object.cpp
	void getEnteringObject(Aurora::NWScript::FunctionContext &ctx);
	void getExitingObject (Aurora::NWScript::FunctionContext &ctx);

	void getIsObjectValid(Aurora::NWScript::FunctionContext &ctx);

	void getIsPC(Aurora::NWScript::FunctionContext &ctx);

	void getLocalInt   (Aurora::NWScript::FunctionContext &ctx);
	void getLocalBool  (Aurora::NWScript::FunctionContext &ctx);
	void getLocalFloat (Aurora::NWScript::FunctionContext &ctx);
	void getLocalString(Aurora::NWScript::FunctionContext &ctx);
	void getLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void setLocalInt   (Aurora::NWScript::FunctionContext &ctx);
	void setLocalBool  (Aurora::NWScript::FunctionContext &ctx);
	void setLocalFloat (Aurora::NWScript::FunctionContext &ctx);
	void setLocalString(Aurora::NWScript::FunctionContext &ctx);
	void setLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void getObjectType(Aurora::NWScript::FunctionContext &ctx);

	void getTag (Aurora::NWScript::FunctionContext &ctx);

	void getArea    (Aurora::NWScript::FunctionContext &ctx);
	void getLocation(Aurora::NWScript::FunctionContext &ctx);

	void getPosition(Aurora::NWScript::FunctionContext &ctx);

	void getDistanceToObject(Aurora::NWScript::FunctionContext &ctx);

	void getObjectByTag       (Aurora::NWScript::FunctionContext &ctx);
	void getWaypointByTag     (Aurora::NWScript::FunctionContext &ctx);
	void getNearestObject     (Aurora::NWScript::FunctionContext &ctx);

	void playAnimation(Aurora::NWScript::FunctionContext &ctx);

	void jumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void jumpToObject  (Aurora::NWScript::FunctionContext &ctx);

	void getObjectConversationResref(Aurora::NWScript::FunctionContext &ctx);
	void getPriorActivation         (Aurora::NWScript::FunctionContext &ctx);

	void setObjectNoCollide(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Creatures, functions_creature.cpp
	void getAutoBalance(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Actions, functions_action.cpp
	void assignCommand(Aurora::NWScript::FunctionContext &ctx);
	void delayCommand (Aurora::NWScript::FunctionContext &ctx);

	void executeScript(Aurora::NWScript::FunctionContext &ctx);

	void actionDoCommand(Aurora::NWScript::FunctionContext &ctx);

	void actionOpenDoor (Aurora::NWScript::FunctionContext &ctx);
	void actionCloseDoor(Aurora::NWScript::FunctionContext &ctx);

	void actionSpeakStringByStrRef(Aurora::NWScript::FunctionContext &ctx);
	void actionStartConversation  (Aurora::NWScript::FunctionContext &ctx);

	void actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx);

	void actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx);
	void actionJumpToObject  (Aurora::NWScript::FunctionContext &ctx);
	void actionMoveToObject  (Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Events, functions_event.cpp
	void eventUserDefined(Aurora::NWScript::FunctionContext &ctx);

	void signalEvent(Aurora::NWScript::FunctionContext &ctx);

	void getUserDefinedEventNumber(Aurora::NWScript::FunctionContext &ctx);
	void setUserDefinedEventNumber(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- 2DA Reads, functions_2da.cpp
	void get2DANumRows  (Aurora::NWScript::FunctionContext &ctx);
	void get2DANumColumn(Aurora::NWScript::FunctionContext &ctx);

	void get2DAEntryIntByString   (Aurora::NWScript::FunctionContext &ctx);
	void get2DAEntryFloatByString (Aurora::NWScript::FunctionContext &ctx);
	void get2DAEntryStringByString(Aurora::NWScript::FunctionContext &ctx);

	void get2DAEntryInt   (Aurora::NWScript::FunctionContext &ctx);
	void get2DAEntryFloat (Aurora::NWScript::FunctionContext &ctx);
	void get2DAEntryString(Aurora::NWScript::FunctionContext &ctx);
	// '---
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_SCRIPT_FUNCTIONS_H
