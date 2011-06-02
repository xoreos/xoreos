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

/** @file engines/nwn/script/functions.cpp
 *  NWN script functions.
 */

#include "boost/bind.hpp"

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/configman.h"

#include "aurora/talkman.h"
#include "aurora/ssffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"
#include "aurora/nwscript/ncsfile.h"

#include "engines/aurora/tokenman.h"

#include "engines/nwn/types.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"
#include "engines/nwn/object.h"
#include "engines/nwn/waypoint.h"
#include "engines/nwn/door.h"
#include "engines/nwn/creature.h"
#include "engines/nwn/location.h"

#include "engines/nwn/script/functions.h"

using Aurora::kObjectIDInvalid;

// NWScript
using Aurora::NWScript::kTypeVoid;
using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeEngineType;
using Aurora::NWScript::kTypeVector;
using Aurora::NWScript::kTypeScriptState;
using Aurora::NWScript::createSignature;
using Aurora::NWScript::createDefaults;

namespace Engines {

namespace NWN {

ObjectDistanceSort::ObjectDistanceSort(const Object &target) {
	target.getPosition(xt, yt, zt);
}

bool ObjectDistanceSort::operator()(Object *a, Object *b) {
	return getDistance(*a) < getDistance(*b);
}

float ObjectDistanceSort::getDistance(Object &a) {
	float x, y, z;
	a.getPosition(x, y, z);

	return ABS(x - xt) + ABS(y - yt) + ABS(z - zt);
}


ScriptFunctions::Defaults::Defaults() {
	int0             = new Aurora::NWScript::Variable(0);
	int1             = new Aurora::NWScript::Variable(1);
	int100           = new Aurora::NWScript::Variable(100);
	int18            = new Aurora::NWScript::Variable(18);
	int9             = new Aurora::NWScript::Variable(9);
	intm1            = new Aurora::NWScript::Variable(-1);
	intMale          = new Aurora::NWScript::Variable(Aurora::kGenderMale);
	intObjectTypeAll = new Aurora::NWScript::Variable(kObjectTypeAll);

	intACAll            = new Aurora::NWScript::Variable(0); // TODO
	intACDodge          = new Aurora::NWScript::Variable(0); // TODO
	intAlignAll         = new Aurora::NWScript::Variable(0); // TODO
	intAttackMisc       = new Aurora::NWScript::Variable(0); // TODO
	intCameraSnap       = new Aurora::NWScript::Variable(0); // TODO
	intClassInvalid     = new Aurora::NWScript::Variable(0); // TODO
	intCreatureLevel    = new Aurora::NWScript::Variable(0); // TODO
	intDamageMagical    = new Aurora::NWScript::Variable(0); // TODO
	intDamageNormal     = new Aurora::NWScript::Variable(0); // TODO
	intFactionHostile   = new Aurora::NWScript::Variable(0); // TODO
	intFirstGenericMale = new Aurora::NWScript::Variable(0); // TODO
	intIPAmmo           = new Aurora::NWScript::Variable(0); // TODO
	intMetaMagicAny     = new Aurora::NWScript::Variable(0); // TODO
	intMissNormal       = new Aurora::NWScript::Variable(0); // TODO
	intObjectCreature   = new Aurora::NWScript::Variable(0); // TODO
	intPackageInvalid   = new Aurora::NWScript::Variable(0); // TODO
	intPersZoneActive   = new Aurora::NWScript::Variable(0); // TODO
	intProjPathDefault  = new Aurora::NWScript::Variable(0); // TODO
	intSaveAll          = new Aurora::NWScript::Variable(0); // TODO
	intSaveNone         = new Aurora::NWScript::Variable(0); // TODO
	intSpellsGeneral    = new Aurora::NWScript::Variable(0); // TODO
	intSpellsAll        = new Aurora::NWScript::Variable(0); // TODO
	intTalk             = new Aurora::NWScript::Variable(0); // TODO
	intVFXNone          = new Aurora::NWScript::Variable(0); // TODO

	float0_0   = new Aurora::NWScript::Variable(0.0f);
	float1_0   = new Aurora::NWScript::Variable(1.0f);
	float2_0   = new Aurora::NWScript::Variable(2.0f);
	float30_0  = new Aurora::NWScript::Variable(30.f);
	float40_0  = new Aurora::NWScript::Variable(40.f);
	float180_0 = new Aurora::NWScript::Variable(180.f);
	floatm1_0  = new Aurora::NWScript::Variable(-1.0f);

	floatMedium = new Aurora::NWScript::Variable(0.0f); // TODO

	object0     = new Aurora::NWScript::Variable((Aurora::NWScript::Object *) 0);
	stringEmpty = new Aurora::NWScript::Variable("");
	vector0     = new Aurora::NWScript::Variable(kTypeVector);
}

ScriptFunctions::Defaults::~Defaults() {
	delete int0;
	delete int1;
	delete int100;
	delete int18;
	delete int9;
	delete intm1;
	delete intMale;
	delete intObjectTypeAll;

	delete intACAll;
	delete intACDodge;
	delete intAlignAll;
	delete intAttackMisc;
	delete intCameraSnap;
	delete intClassInvalid;
	delete intCreatureLevel;
	delete intDamageMagical;
	delete intDamageNormal;
	delete intFactionHostile;
	delete intFirstGenericMale;
	delete intIPAmmo;
	delete intMetaMagicAny;
	delete intMissNormal;
	delete intObjectCreature;
	delete intPackageInvalid;
	delete intPersZoneActive;
	delete intProjPathDefault;
	delete intSaveAll;
	delete intSaveNone;
	delete intSpellsGeneral;
	delete intSpellsAll;
	delete intTalk;
	delete intVFXNone;

	delete float0_0;
	delete float1_0;
	delete float2_0;
	delete float30_0;
	delete float40_0;
	delete float180_0;
	delete floatm1_0;

	delete floatMedium;

	delete object0;
	delete stringEmpty;
	delete vector0;
}


ScriptFunctions::ScriptFunctions() {
	registerFunctions();
}

ScriptFunctions::~ScriptFunctions() {
	FunctionMan.clear();
}

void ScriptFunctions::setModule(Module *module) {
	_module = module;
}

Common::UString ScriptFunctions::floatToString(float f, int width, int decimals) {
	return Common::UString::sprintf("%*.*f", width, decimals, f);
}

void ScriptFunctions::registerFunctions() {
	Defaults defaults;

	registerFunctions000(defaults);
	registerFunctions100(defaults);
	registerFunctions200(defaults);
	registerFunctions300(defaults);
	registerFunctions400(defaults);
	registerFunctions500(defaults);
	registerFunctions600(defaults);
	registerFunctions700(defaults);
	registerFunctions800(defaults);
}

int32 ScriptFunctions::random(int min, int max, int32 n) {
	if (n < 1)
		n = 1;

	int32 r = 0;

	while (n-- > 0)
		r += std::rand() % (max - min + 1) + min;

	return r;
}

Aurora::NWScript::Object *ScriptFunctions::getPC() {
	if (!_module)
		return 0;

	return (Aurora::NWScript::Object *) _module->getPC();
}

Common::UString ScriptFunctions::gTag(const Aurora::NWScript::Object *o) {
	if (o)
		return Common::UString::sprintf("\"%s\"", o->getTag().c_str());

	return "0";
}

Object *ScriptFunctions::convertObject(Aurora::NWScript::Object *o) {
	Object *object = dynamic_cast<Object *>(o);
	if (!object || !object->loaded() || (object->getID() == kObjectIDInvalid))
		return 0;

	return object;
}

Waypoint *ScriptFunctions::convertWaypoint(Aurora::NWScript::Object *o) {
	Waypoint *waypoint = dynamic_cast<Waypoint *>(o);
	if (!waypoint || !waypoint->loaded() || (waypoint->getID() == kObjectIDInvalid))
		return 0;

	return waypoint;
}

Situated *ScriptFunctions::convertSituated(Aurora::NWScript::Object *o) {
	Situated *situated = dynamic_cast<Situated *>(o);
	if (!situated || !situated->loaded() || (situated->getID() == kObjectIDInvalid))
		return 0;

	return situated;
}

Door *ScriptFunctions::convertDoor(Aurora::NWScript::Object *o) {
	Door *door = dynamic_cast<Door *>(o);
	if (!door || !door->loaded() || (door->getID() == kObjectIDInvalid))
		return 0;

	return door;
}

Creature *ScriptFunctions::convertCreature(Aurora::NWScript::Object *o) {
	Creature *creature = dynamic_cast<Creature *>(o);
	if (!creature || !creature->loaded() || (creature->getID() == kObjectIDInvalid))
		return 0;

	return creature;
}

Creature *ScriptFunctions::convertPC(Aurora::NWScript::Object *o) {
	Creature *pc = dynamic_cast<Creature *>(o);
	if (!pc || !pc->loaded() || (pc->getID() == kObjectIDInvalid) || !pc->isPC())
		return 0;

	return pc;
}

Area *ScriptFunctions::convertArea(Aurora::NWScript::Object *o) {
	return dynamic_cast<Area *>(o);
}

Module *ScriptFunctions::convertModule(Aurora::NWScript::Object *o) {
	return dynamic_cast<Module *>(o);
}

Location *ScriptFunctions::convertLocation(Aurora::NWScript::EngineType *e) {
	return dynamic_cast<Location *>(e);
}

} // End of namespace NWN

} // End of namespace Engines
