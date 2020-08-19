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
 *  Dragon Age: Origins engine functions.
 */

#ifndef ENGINES_DRAGONAGE_SCRIPT_FUNCTIONS_H
#define ENGINES_DRAGONAGE_SCRIPT_FUNCTIONS_H

#include "src/aurora/nwscript/types.h"

namespace Aurora {
	namespace NWScript {
		class FunctionContext;
		class Object;
	}
}

namespace Engines {

namespace DragonAge {

class Game;
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
		const Aurora::NWScript::Variable *defaults[7];
	};

	static const FunctionPointer kFunctionPointers[];
	static const FunctionSignature kFunctionSignatures[];
	static const FunctionDefaults kFunctionDefaults[];


	Game *_game;

	void registerFunctions();

	// .--- Utility methods
	void jumpTo(DragonAge::Object *object, float x, float y, float z);

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

	void random     (Aurora::NWScript::FunctionContext &ctx);
	void randomFloat(Aurora::NWScript::FunctionContext &ctx);

	void intToFloat(Aurora::NWScript::FunctionContext &ctx);
	void floatToInt(Aurora::NWScript::FunctionContext &ctx);

	void vector(Aurora::NWScript::FunctionContext &ctx);

	void isVectorEmpty(Aurora::NWScript::FunctionContext &ctx);

	void getVectorMagnitude(Aurora::NWScript::FunctionContext &ctx);
	void getVectorNormalize(Aurora::NWScript::FunctionContext &ctx);

	void getArraySize(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Strings, functions_string.cpp
	void printToLog        (Aurora::NWScript::FunctionContext &ctx);
	void printToLogAndFlush(Aurora::NWScript::FunctionContext &ctx);

	void logTrace    (Aurora::NWScript::FunctionContext &ctx);
	void printWarning(Aurora::NWScript::FunctionContext &ctx);

	void DEBUG_printToScreen(Aurora::NWScript::FunctionContext &ctx);
	void printToLogWindow   (Aurora::NWScript::FunctionContext &ctx);

	void printInteger (Aurora::NWScript::FunctionContext &ctx);
	void printFloat   (Aurora::NWScript::FunctionContext &ctx);
	void printString  (Aurora::NWScript::FunctionContext &ctx);
	void printObject  (Aurora::NWScript::FunctionContext &ctx);
	void printVector  (Aurora::NWScript::FunctionContext &ctx);
	void printResource(Aurora::NWScript::FunctionContext &ctx);

	void intToString     (Aurora::NWScript::FunctionContext &ctx);
	void floatToString   (Aurora::NWScript::FunctionContext &ctx);
	void objectToString  (Aurora::NWScript::FunctionContext &ctx);
	void vectorToString  (Aurora::NWScript::FunctionContext &ctx);
	void resourceToString(Aurora::NWScript::FunctionContext &ctx);

	void toString(Aurora::NWScript::FunctionContext &ctx);

	void intToHexString(Aurora::NWScript::FunctionContext &ctx);

	void stringToInt   (Aurora::NWScript::FunctionContext &ctx);
	void stringToFloat (Aurora::NWScript::FunctionContext &ctx);
	void stringToVector(Aurora::NWScript::FunctionContext &ctx);

	void charToInt(Aurora::NWScript::FunctionContext &ctx);
	void intToChar(Aurora::NWScript::FunctionContext &ctx);

	void isStringEmpty  (Aurora::NWScript::FunctionContext &ctx);
	void getStringLength(Aurora::NWScript::FunctionContext &ctx);

	void stringUpperCase(Aurora::NWScript::FunctionContext &ctx);
	void stringLowerCase(Aurora::NWScript::FunctionContext &ctx);

	void stringRight(Aurora::NWScript::FunctionContext &ctx);
	void stringLeft (Aurora::NWScript::FunctionContext &ctx);

	void insertString (Aurora::NWScript::FunctionContext &ctx);
	void subString    (Aurora::NWScript::FunctionContext &ctx);
	void findSubString(Aurora::NWScript::FunctionContext &ctx);

	void getStringByStringId(Aurora::NWScript::FunctionContext &ctx);
	void getTlkTableString  (Aurora::NWScript::FunctionContext &ctx);

	void getCurrentScriptName    (Aurora::NWScript::FunctionContext &ctx);
	void getCurrentScriptResource(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Module functions, functions_module.cpp
	void getModule(Aurora::NWScript::FunctionContext &ctx);

	void getHero(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- General object functions, functions_object.cpp
	void isObjectValid(Aurora::NWScript::FunctionContext &ctx);

	void getLocalInt     (Aurora::NWScript::FunctionContext &ctx);
	void getLocalFloat   (Aurora::NWScript::FunctionContext &ctx);
	void getLocalString  (Aurora::NWScript::FunctionContext &ctx);
	void getLocalObject  (Aurora::NWScript::FunctionContext &ctx);
	void getLocalResource(Aurora::NWScript::FunctionContext &ctx);
	void getLocalLocation(Aurora::NWScript::FunctionContext &ctx);
	void getLocalPlayer  (Aurora::NWScript::FunctionContext &ctx);
	void getLocalEvent   (Aurora::NWScript::FunctionContext &ctx);
	void getLocalCommand (Aurora::NWScript::FunctionContext &ctx);
	void getLocalEffect  (Aurora::NWScript::FunctionContext &ctx);

	void getLocalItemProperty(Aurora::NWScript::FunctionContext &ctx);

	void setLocalInt     (Aurora::NWScript::FunctionContext &ctx);
	void setLocalFloat   (Aurora::NWScript::FunctionContext &ctx);
	void setLocalString  (Aurora::NWScript::FunctionContext &ctx);
	void setLocalObject  (Aurora::NWScript::FunctionContext &ctx);
	void setLocalResource(Aurora::NWScript::FunctionContext &ctx);
	void setLocalLocation(Aurora::NWScript::FunctionContext &ctx);
	void setLocalPlayer  (Aurora::NWScript::FunctionContext &ctx);
	void setLocalEvent   (Aurora::NWScript::FunctionContext &ctx);
	void setLocalCommand (Aurora::NWScript::FunctionContext &ctx);
	void setLocalEffect  (Aurora::NWScript::FunctionContext &ctx);

	void setLocalItemProperty(Aurora::NWScript::FunctionContext &ctx);

	void getObjectType(Aurora::NWScript::FunctionContext &ctx);

	void getTag   (Aurora::NWScript::FunctionContext &ctx);
	void getResRef(Aurora::NWScript::FunctionContext &ctx);
	void getName  (Aurora::NWScript::FunctionContext &ctx);
	void setName  (Aurora::NWScript::FunctionContext &ctx);

	void getArea    (Aurora::NWScript::FunctionContext &ctx);
	void getPosition(Aurora::NWScript::FunctionContext &ctx);

	void getDistanceBetween(Aurora::NWScript::FunctionContext &ctx);

	void getObjectByTag(Aurora::NWScript::FunctionContext &ctx);

	void getNearestObject     (Aurora::NWScript::FunctionContext &ctx);
	void getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx);

	void UT_getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Creatures, functions_creature.cpp
	void isHero(Aurora::NWScript::FunctionContext &ctx);
	// '---

	// .--- Events, functions_event.cpp
	void getCurrentEvent(Aurora::NWScript::FunctionContext &ctx);

	void isEventValid(Aurora::NWScript::FunctionContext &ctx);

	void getEventType   (Aurora::NWScript::FunctionContext &ctx);
	void getEventCreator(Aurora::NWScript::FunctionContext &ctx);
	void getEventTarget (Aurora::NWScript::FunctionContext &ctx);

	void setEventType   (Aurora::NWScript::FunctionContext &ctx);
	void setEventCreator(Aurora::NWScript::FunctionContext &ctx);

	void handleEvent(Aurora::NWScript::FunctionContext &ctx);
	// '---
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_SCRIPT_FUNCTIONS_H
